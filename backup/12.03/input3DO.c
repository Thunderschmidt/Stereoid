#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include "coldet.h"
#include "global.h"
#include "input3do.h"
#include "mat.h"
#define SCALE 0.4

typedef struct TAGOBJECT
{
    long VersionSignature;			//  0
    long NumberOfVertexes;			//  4
    long NumberOfPrimitives;		//  8
    long OffsetToselectionPrimitive;// 12
    fixed XFromParent;				// 16
    fixed YFromParent;				// 20
    fixed ZFromParent;				// 24
    long OffsetToObjectName;		// 28
    long Always_0;				    // 32
    long OffsetToVertexArray;		// 36
    long OffsetToPrimitiveArray;	// 40
    long OffsetToSiblingObject;		// 44
    long OffsetToChildObject;		// 48
}TAGOBJECT;

typedef struct TAGPRIMITIVE
{
    long ColorIndex;
    long NumberOfVertexes;
    long Always_0;
    long OffsetToVertexesndexArray;
    long OffsetToTextureName;
    long Unknown_1;
    long Unknown_2;
    long Unknown_3;    
}TAGPRIMITIVE;

typedef struct TAGVERTEX
{
    fixed x;
    fixed y;
    fixed z;
}TAGVERTEX;


TAGVERTEX       *tagVertex;
O3D             *o3d;
TAGPRIMITIVE    tagPrimitive;
TAGOBJECT       tagObject;
short           obj_anz,
                objnr=0;
                                
long filesize;
VECTOR min, max;

long checkoffset(long offset)
{
    if((offset<0)||(offset >=filesize))
    {
        printf("\n !!! OFFSET READ ERROR  !!!");
        return(0);
    }
    return(offset);
}
void count_object(long offset, FILE *in)
{
    long child, sibl, arr[60];
	fseek(in, offset, 0);
	fread(&tagObject.VersionSignature, 4, 13, in);
	sibl=checkoffset(tagObject.OffsetToSiblingObject); 
	child=checkoffset(tagObject.OffsetToChildObject); 
   	obj_anz++;
   	if (child > 0) count_object(child, in);
	if (sibl > 0) count_object(sibl, in);
}



void read_object(long offset, FILE *in, short o, MODEL *model)
{
	int i, v, j=0;
	long child, sibl;
	float radius=0;
	char color, lastcolor;
	o3d[o].single_colored=TRUE;
/****************************LESE HEADER****************************/

	fseek(in, offset, 0);										//lese Object-Header
	fread(&tagObject.VersionSignature, sizeof(tagObject), 1, in);

	o3d[o].pos.x=fixtof(tagObject.XFromParent)*SCALE;
	o3d[o].pos.y=fixtof(tagObject.YFromParent)*SCALE;
	o3d[o].pos.z=fixtof(tagObject.ZFromParent)*SCALE;
	o3d[o].vert_anz=tagObject.NumberOfVertexes;
	o3d[o].poly_anz=tagObject.NumberOfPrimitives;
	o3d[o].sibl_o=0;
	o3d[o].child_o=0;
	fseek(in, tagObject.OffsetToObjectName,0);
	fgets(o3d[o].name, 32, in); //lese Namen

//printf(" %s, %d \n", o3d[o].name, o3d[o].visible);
/***********************LESE PUNKTKOORDINATEN***********************/

	tagVertex=(TAGVERTEX*)malloc(tagObject.NumberOfVertexes*sizeof(TAGVERTEX));		  //öffne Zwischenppeicher für Punktkoordinaten
	o3d[o].vert=(VECTOR*)malloc(tagObject.NumberOfVertexes*sizeof(VECTOR));	  //öffne Speicher für Punktkoordinaten
	fseek(in, tagObject.OffsetToVertexArray, 0);						  //springe zum Punktkoordinaten-Offset
	fread(&tagVertex[0].x, sizeof(TAGVERTEX), tagObject.NumberOfVertexes, in);//lese Punktkoordinaten ein 

	for (v=0; v<tagObject.NumberOfVertexes; v++)
	{
		if((o3d[o].vert[v].x=fixtof(tagVertex[v].x)*SCALE)>max.x) max.x=o3d[o].vert[v].x;
		if((o3d[o].vert[v].y=fixtof(tagVertex[v].y)*SCALE)>max.y) max.y=o3d[o].vert[v].y;
		if((o3d[o].vert[v].z=fixtof(tagVertex[v].z)*SCALE)>max.z) max.z=o3d[o].vert[v].z;

		if(o3d[o].vert[v].x<min.x) min.x=o3d[o].vert[v].x;
		if(o3d[o].vert[v].y<min.y) min.y=o3d[o].vert[v].y;
		if(o3d[o].vert[v].z<min.z) min.z=o3d[o].vert[v].z;
		
		if(!o)
		{
		   radius=o3d[o].vert[v].x*o3d[o].vert[v].x+o3d[o].vert[v].y*o3d[o].vert[v].y+o3d[o].vert[v].z*o3d[o].vert[v].z;
           if(model->radius<radius)model->radius=radius;
        }
	}
	free(tagVertex);

/*************************LESE INDEX-ARRAY**************************/

	o3d[o].ind_anz = (char*)malloc(tagObject.NumberOfPrimitives*sizeof(char));  //öffne Speicher für Indizes
	o3d[o].color   = (char*)malloc(tagObject.NumberOfPrimitives*sizeof(char));
	o3d[o].i       =(short*)malloc(tagObject.NumberOfPrimitives*sizeof(short)*64);  //ACHTUNG!!!!!!!!!
	for (i=0; i<tagObject.NumberOfPrimitives; i++)						
	{
		fseek(in, tagObject.OffsetToPrimitiveArray+sizeof(TAGPRIMITIVE)*i, 0);	//springe zum 3D-Primitive-Offset
		fread(&tagPrimitive.ColorIndex, sizeof(TAGPRIMITIVE), 1, in);			//lese 3D-Primitives ein 
      lastcolor=color;
		color=o3d[o].color[i] = 15-(tagPrimitive.ColorIndex-80);					//die Farbe
      if(o3d[o].single_colored)if(i<0)if(lastcolor!=color)o3d[o].single_colored=FALSE;
      o3d[o].ind_anz[i] = tagPrimitive.NumberOfVertexes;				        //meistens 4
		fseek(in, tagPrimitive.OffsetToVertexesndexArray, 0);					//springe zum Vertex-Index-Array
		fread(&o3d[o].i[j], sizeof(short), tagPrimitive.NumberOfVertexes, in);
		j+=tagPrimitive.NumberOfVertexes;                                        //inkrementiere Inmdex-Counter
		if(tagPrimitive.NumberOfVertexes>o3d[o].facetype)o3d[o].facetype=tagPrimitive.NumberOfVertexes;
	}
	if (o3d[o].facetype>POLYGON)o3d[o].facetype=POLYGON;
    realloc(o3d[o].i, j*sizeof(short));                                         //passe Speicher an
/*************************LESE UNTEROBJEKTE*************************/

	sibl=checkoffset(tagObject.OffsetToSiblingObject); 
	child=checkoffset(tagObject.OffsetToChildObject);
 
  	if (child> 0)   //lese untergeordnetes Objekt
	{
		objnr++;
		o3d[o].child_o=objnr;
		read_object(child, in, objnr, model);
	} 
	if (sibl > 0)   //lese gleichwertiges Objekt
	{
		objnr++;
		o3d[o].sibl_o=objnr;
		read_object(sibl, in, objnr, model);
	}
}


char line_ok(int l, int i0, int i1, short *line)
{
	int z;
	for(z=0; z<l; z++)
	{
		if  (i0==line[z*2]) if (i1==line[z*2+1])return(0);
		if  (i0==line[z*2+1]) if (i1==line[z*2])return(0);
		if  (i0==i1) return(0);
	}
	return(1);
}

void find_lines()
{
	int o,           //Objekt-Nr.
        p,           //Polygon-Nr.
        l,           //Linien-Nr.
        i,           //Zähler-Variable
        i1,          //Linien-Index 1
        i2,          //Linien-Index 2
        po;          //wird zum Polygon-Index addiert, "polygon offset"          
	for (o=0; o<obj_anz; o++)
	{
        o3d[o].line=(short*)malloc(o3d[o].poly_anz*32+1000);  //reserviert Speicher für Linien-Indizes, pro Linie 4 Bytes
	    l=0;
	    po=0;
	    for (p=0; p<o3d[o].poly_anz; p++)
	    {
//	       if(o3d[o].ind_anz[p]>2)              //selektiert fehlerhafte polygone aus
	       for (i=0; i<o3d[o].ind_anz[p]; i++)
	       {
		      i1=o3d[o].i[i+po];
		      i2=o3d[o].i[i+po+1];
		      if(i==(o3d[o].ind_anz[p]-1))i2=o3d[o].i[po];
		      if(line_ok(l, i1, i2, o3d[o].line))
              { 
//                 printf("object %s: face %d i1:%d i2:%d \n", o3d[o].name, p, i1, i2);
                 o3d[o].line[l*2]=i1;
                 o3d[o].line[l*2+1]=i2;
                 l++;
              }
           }
           po+=o3d[o].ind_anz[p];  		   //setze polygon-offset auf nächstes polygon
        }
        o3d[o].line_anz=l;
        realloc(o3d[o].line, o3d[o].line_anz*4);
    }

}

void calc_normals(const char datei[])
{
    int o, p, i;
   	for (o=0; o<obj_anz; o++)
   	{
      o3d[o].n=(VECTOR*)malloc(o3d[o].poly_anz*sizeof(VECTOR));
      i=0;
      for(p=0; p<o3d[o].poly_anz; p++)
      {
         if(o3d[o].ind_anz[p]>=3)
         {  
            o3d[o].n[p]=get_normal_vector(&o3d[o].vert[o3d[o].i[i]], &o3d[o].vert[o3d[o].i[i+1]], &o3d[o].vert[o3d[o].i[i+2]]);                
 /*           cross_product_f(
            o3d[o].vert[o3d[o].i[i+1]].x-o3d[o].vert[o3d[o].i[i]].x,
            o3d[o].vert[o3d[o].i[i+1]].y-o3d[o].vert[o3d[o].i[i]].y,
            o3d[o].vert[o3d[o].i[i+1]].z-o3d[o].vert[o3d[o].i[i]].z,
            o3d[o].vert[o3d[o].i[i+2]].x-o3d[o].vert[o3d[o].i[i]].x,
            o3d[o].vert[o3d[o].i[i+2]].y-o3d[o].vert[o3d[o].i[i]].y,
            o3d[o].vert[o3d[o].i[i+2]].z-o3d[o].vert[o3d[o].i[i]].z,
            &o3d[o].n[p].x,
            &o3d[o].n[p].y,
            &o3d[o].n[p].z);
            normalize_vector_f(&o3d[o].n[p].x, &o3d[o].n[p].y, &o3d[o].n[p].z); */
          }
          else
          {
             printf("WARNING: in file %s, object %s: face %d consists of only %d point(s)\n", datei, o3d[o].name, p, o3d[o].ind_anz[p]);
             o3d[o].n[p].x=0;
             o3d[o].n[p].y=0;
             o3d[o].n[p].z=0;
          }
          i+=o3d[o].ind_anz[p];
       }
    }
}

void del_polys(MODEL *mod)
{
    int i;
    for(i=0;i<mod->obj_anz;i++)
    {
        free(mod->o3d[i].color);
        free(mod->o3d[i].i);
        free(mod->o3d[i].n);
    }
}

short init_collision_model(MODEL *mod, O3D *o3d)
{
    int p, j=0;
    mod->colmodel = newCollisionModel3D();
    for(p=0;p<o3d->poly_anz;p++)
    {
       mod->colmodel->addTriangle(&o3d->vert[o3d->i[j]].x,
                                  &o3d->vert[o3d->i[j+1]].x,
                                  &o3d->vert[o3d->i[j+2]].x);
       j+=o3d->ind_anz[p];
    }
    mod->colmodel->finalize();
    mod->n=o3d->n;
}

int lese_3do(MODEL *model, const char datei[])
{
    objnr=0;
    min.x=0;min.y=0; min.z=0;max=min;
    obj_anz=0;
	FILE *in = stdin;
	if ( (in = fopen(datei,"rb")) == NULL )
	{
		fprintf(stderr, " Konnte %s nicht finden \n", datei);
	}
	fseek(in, 0, SEEK_END);
	filesize=ftell(in);
	count_object(0, in);
	o3d=(O3D*)malloc(obj_anz*sizeof(O3D));
	read_object(0, in, 0, model);
	fclose(in);
	find_lines();
   	calc_normals(datei);
    model->obj_anz = obj_anz;
    model->o3d = o3d;
    model->max = max;
    model->min = min;
    model->size=max.x-min.x;
    model->radius=sqrt(model->radius);
    if(model->size<(max.y-min.y)) model->size=max.y-min.y;
    if(model->size<(max.z-min.z)) model->size=max.z-min.z;
	return(0);
}
