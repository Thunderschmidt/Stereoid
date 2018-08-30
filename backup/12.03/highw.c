/*  Hier werden die Datenautobahnen errechnet und gemalt
 *
 *
 */

#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "mat.h"
#include "drawprim.h"
#include "ster.h"
#include "highw.h"

HIGHWAY highway[2];
int highway_anz=0;


void draw_highways()
{
    int i, j;
    glLineWidth (1.0);

    for (i=0; i<highway_anz; i++)
    {
       color16(11); 
       glEnable(GL_DEPTH_TEST);
       glDisable(GL_POLYGON_OFFSET_FILL);
       glBegin(GL_LINE_STRIP);
       for (j=0; j<highway[i].segment_anz; j++)glVertex3fv(&highway[i].segment[j].x);
       glEnd();

       glBegin(GL_LINE_STRIP);
       for (j=0; j<highway[i].segment_anz; j++)
           glVertex3f(highway[i].segment[j].x+highway[i].left[j].x*highway[i].scale[j]*1.4,
                     highway[i].segment[j].y+highway[i].left[j].y*highway[i].scale[j]*1.4,
                     highway[i].segment[j].z+highway[i].left[j].z*highway[i].scale[j]*1.4);
            
       glEnd();

       glBegin(GL_LINE_STRIP);
       for (j=0; j<highway[i].segment_anz; j++)
           glVertex3f(highway[i].segment[j].x-highway[i].left[j].x*highway[i].scale[j],
                     highway[i].segment[j].y-highway[i].left[j].y*highway[i].scale[j],
                     highway[i].segment[j].z-highway[i].left[j].z*highway[i].scale[j]);
            
       glEnd();
       glBegin(GL_LINES);
       for (j=0; j<highway[i].segment_anz; j++)
       {
           glVertex3f(highway[i].segment[j].x-highway[i].left[j].x*highway[i].scale[j],
                     highway[i].segment[j].y-highway[i].left[j].y*highway[i].scale[j],
                     highway[i].segment[j].z-highway[i].left[j].z*highway[i].scale[j]);
           glVertex3f(highway[i].segment[j].x+highway[i].left[j].x*highway[i].scale[j]*1.4,
                     highway[i].segment[j].y+highway[i].left[j].y*highway[i].scale[j]*1.4,
                     highway[i].segment[j].z+highway[i].left[j].z*highway[i].scale[j]*1.4);
       }
       glEnd();
       glEnable(GL_POLYGON_OFFSET_FILL);
       glPolygonOffset(1, 1);
       color16(4); 
     
       glBegin(GL_QUADS);
       for (j=0; j<(highway[i].segment_anz-1); j++)
       {
           glVertex3f(highway[i].segment[j].x-highway[i].left[j].x*highway[i].scale[j],
                     highway[i].segment[j].y-highway[i].left[j].y*highway[i].scale[j],
                     highway[i].segment[j].z-highway[i].left[j].z*highway[i].scale[j]);
           glVertex3f(highway[i].segment[j].x+highway[i].left[j].x*highway[i].scale[j],
                     highway[i].segment[j].y+highway[i].left[j].y*highway[i].scale[j],
                     highway[i].segment[j].z+highway[i].left[j].z*highway[i].scale[j]);
           glVertex3f(highway[i].segment[j+1].x+highway[i].left[j+1].x*highway[i].scale[j+1],
                     highway[i].segment[j+1].y+highway[i].left[j+1].y*highway[i].scale[j+1],
                     highway[i].segment[j+1].z+highway[i].left[j+1].z*highway[i].scale[j+1]);
           glVertex3f(highway[i].segment[j+1].x-highway[i].left[j+1].x*highway[i].scale[j+1],
                     highway[i].segment[j+1].y-highway[i].left[j+1].y*highway[i].scale[j+1],
                     highway[i].segment[j+1].z-highway[i].left[j+1].z*highway[i].scale[j+1]);

       }
       glEnd();
       color16(6); 

       glBegin(GL_QUADS);
       for (j=0; j<(highway[i].segment_anz-1); j++)
       {
           glVertex3f(highway[i].segment[j].x+highway[i].left[j].x*highway[i].scale[j],
                     highway[i].segment[j].y+highway[i].left[j].y*highway[i].scale[j],
                     highway[i].segment[j].z+highway[i].left[j].z*highway[i].scale[j]);
           glVertex3f(highway[i].segment[j].x+highway[i].left[j].x*highway[i].scale[j]*1.3,
                     highway[i].segment[j].y+highway[i].left[j].y*highway[i].scale[j]*1.3,
                     highway[i].segment[j].z+highway[i].left[j].z*highway[i].scale[j]*1.3);
           glVertex3f(highway[i].segment[j+1].x+highway[i].left[j+1].x*highway[i].scale[j+1]*1.3,
                     highway[i].segment[j+1].y+highway[i].left[j+1].y*highway[i].scale[j+1]*1.3,
                     highway[i].segment[j+1].z+highway[i].left[j+1].z*highway[i].scale[j+1]*1.3);
           glVertex3f(highway[i].segment[j+1].x+highway[i].left[j+1].x*highway[i].scale[j+1],
                     highway[i].segment[j+1].y+highway[i].left[j+1].y*highway[i].scale[j+1],
                     highway[i].segment[j+1].z+highway[i].left[j+1].z*highway[i].scale[j+1]);

       }
       glEnd();

    }
}

void init_highways()
{
    int i=0;
    int j=0;
    VECTOR v1, v2;
    float cosi;
//    highway[i].segment[j]=vect(0,     300000,  0);j++;
    highway[i].segment[j]=vect(-76,     0,  40);j++;
    highway[i].segment[j]=vect(150,    0,  40);j++;
    highway[i].segment[j]=vect(400,    0, 9);j++;
    highway[i].segment[j]=vect(600,   -3, -15);j++;
    highway[i].segment[j]=vect(800,    8, -17);j++;
    highway[i].segment[j]=vect(1000,  12, -33);j++;
    highway[i].segment[j]=vect(1200,  25,   0);j++;
    highway[i].segment[j]=vect(1400,  37,   0);j++;
    highway[i].segment[j]=vect(1600,  45,   0);j++;
    highway[i].segment[j]=vect(1800,  55,   4);j++;
    highway[i].segment[j]=vect(2000,  78,   6);j++;
    highway[i].segment[j]=vect(2200,  83,   8);j++;
    highway[i].segment[j]=vect(2400,  96,   9);j++;
    highway[i].segment[j]=vect(2600, 100,  12);j++;
    highway[i].segment[j]=vect(2800, 110,  13);j++;
    highway[i].segment[j]=vect(3000, 130 , 15);j++;
    highway[i].segment[j]=vect(320000, 130 , 15);j++;
    highway[i].segment_anz=17;i++; j=0;
    highway_anz++;
 //   highway[i].segment[j]=vect(0,  0, -300000);j++;
//    highway[i].segment[j]=vect(0,  0,  0);j++;
//    highway[i].segment[j]=vect(3,  10,  200);j++;
//    highway[i].segment[j]=vect(0,  20,  400);j++;
//    highway[i].segment[j]=vect(-15,  1,  600);j++;
//    highway[i].segment[j]=vect(0,  20,  800);j++;
//    highway[i].segment[j]=vect(10,  40,  1000);j++;
//    highway[i].segment[j]=vect(20,  60,  1200);j++;
//    highway[i].segment[j]=vect(10,  40,  1400);j++;
//    highway[i].segment[j]=vect(10,  30,  1600);j++;
//    highway[i].segment[j]=vect(0,    20,  180000);j++;
//    highway[i].segment_anz=11;i++;
//    highway_anz++;
    
    
    for(i=0;i<highway_anz;i++)
    for(j=0;j<highway[i].segment_anz;j++)
    {
       highway[i].up[j]=vect(0,-1,0);    //Der Up-Vektor für alle Elemente
       if(j==(highway[i].segment_anz-1))       //Fürs letzte Element gilt der vorherige Front-Vektor
       {
          highway[i].fro[j]=highway[i].fro[j-1];
       }
       else
       {
          highway[i].fro[j].x=highway[i].segment[j+1].x-highway[i].segment[j].x;
          highway[i].fro[j].y=highway[i].segment[j+1].y-highway[i].segment[j].y;
          highway[i].fro[j].z=highway[i].segment[j+1].z-highway[i].segment[j].z;
          normalize_vector_f(&highway[i].fro[j].x, &highway[i].fro[j].y, &highway[i].fro[j].z);
       }
          highway[i].fro[j].x=highway[i].segment[j+1].x-highway[i].segment[j].x;

       if((j>0)&&(j<(highway[i].segment_anz-1)))
       {
          cross_product_f(highway[i].segment[j-1].x-highway[i].segment[j+1].x,
                          highway[i].segment[j-1].y-highway[i].segment[j+1].y,       
                          highway[i].segment[j-1].z-highway[i].segment[j+1].z,       
                          highway[i].up[j].x,
                          highway[i].up[j].y,
                          highway[i].up[j].z,
                          &highway[i].left[j].x,
                          &highway[i].left[j].y,
                          &highway[i].left[j].z);
       }
       else if(j==(highway[i].segment_anz-1))
       {
          cross_product_f(highway[i].segment[j-1].x-highway[i].segment[j].x,
                          highway[i].segment[j-1].y-highway[i].segment[j].y,       
                          highway[i].segment[j-1].z-highway[i].segment[j].z,       
                          highway[i].up[j].x,
                          highway[i].up[j].y,
                          highway[i].up[j].z,
                          &highway[i].left[j].x,
                          &highway[i].left[j].y,
                          &highway[i].left[j].z);
       }
       else if(j==0)
       {
          cross_product_f(highway[i].segment[j].x-highway[i].segment[j+1].x,
                          highway[i].segment[j].y-highway[i].segment[j+1].y,       
                          highway[i].segment[j].z-highway[i].segment[j+1].z,       
                          highway[i].up[j].x,
                          highway[i].up[j].y,
                          highway[i].up[j].z,
                          &highway[i].left[j].x,
                          &highway[i].left[j].y,
                          &highway[i].left[j].z);
       }
       normalize_vector_f(&highway[i].left[j].x, &highway[i].left[j].y, &highway[i].left[j].z);
       if((j>0)&&(j<(highway[i].segment_anz-1)))
       {
           v1.x=highway[i].segment[j+1].x-highway[i].segment[j].x;
           v1.y=highway[i].segment[j+1].y-highway[i].segment[j].y;
           v1.z=highway[i].segment[j+1].z-highway[i].segment[j].z;
           normalize_vector_f(&v1.x, &v1.y, &v1.z);
           cosi=vector_product_v(&v1, &highway[i].left[j]);
           highway[i].scale[j]=HIGHWAYWIDTH/sin(acos(cosi));

       }
       else highway[i].scale[j]=HIGHWAYWIDTH;
    }
}

