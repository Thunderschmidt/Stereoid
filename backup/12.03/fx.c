/*  Hier tummeln sich allerlei grafische Animationen
 *
 *
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "mat.h"
#include "input3DO.h"
#include "drawprim.h"
#include "actions.h"
#include "ster.h"
#include "fx.h"


float        *Fmult3;
volatile int *Chrono3;

void init_fx(float *fmult, volatile int *chrono)
{
    Fmult3=fmult;
    Chrono3=chrono;
}

void EXPLOSION1_CLASS::init(const char model_name[], int part_total)
{
    int i;
    VECTOR v1;
    lese_3do(&model, model_name);
    particle_total=part_total;
    particles=(MATRIX_f*)malloc(sizeof(MATRIX_f)*particle_total);
   	for (i=0; i<particle_total; i++)
	{
	   v1.x = rand() % 1024 - 512;
	   v1.y = rand() % 1024 - 512;
	   v1.z = rand() % 64 +  400;
	   normalize_vector_v(&v1);
	   particles[i]=align_matrix_z(v1);
	}
	total=0;
}

void EXPLOSION1_CLASS::draw()
{
    int i, j;
    float mult;
//    color16(c, 14);
    glDisable(GL_LIGHTING);

   	for (j=0; j<total; j++)
	{
   	   glPushMatrix();
   	   glMultMatrix_allegro(&explosion1[j]->m);
   	   for (i=0; i<explosion1[j]->total;i++)
   	   {
          glPushMatrix();
	      mult=explosion1[j]->particles[i]*explosion1[j]->mult;
	      glTranslatef(particles[i].v[2][0]*mult, particles[i].v[2][1]*mult, particles[i].v[2][2]*mult);
	      glMultMatrix_allegro(&particles[i]);
	      draw_object_alpha(&model.o3d[0], 240);	      
	      glPopMatrix();
       }
   	   glPopMatrix();
    }
        glEnable(GL_LIGHTING);

}

void EXPLOSION1_CLASS::kill(short i)
{
   free(explosion1[i]->particles);
   free(explosion1[i]);
   total--;
   if(total)explosion1[i]=explosion1[total];

}

void EXPLOSION1_CLASS::start(VECTOR *flightdir, MATRIX_f *m)
{
    int j;
    explosion1[total]=(EXPLOSION1*)malloc(sizeof(EXPLOSION1));
    explosion1[total]->particles=(float*)malloc(sizeof(float)*particle_total);
    explosion1[total]->m=*m;
    explosion1[total]->t=*Chrono3;
    explosion1[total]->mult=0.1;
    explosion1[total]->total=particle_total;
    explosion1[total]->flightdir=*flightdir;
    for(j=0;j<explosion1[total]->total;j++)
    {
       explosion1[total]->particles[j]=rand() % 1024;
       explosion1[total]->particles[j]/=2048;
       explosion1[total]->particles[j]+=0.1;
    }
    total++;
}

void EXPLOSION1_CLASS::proceed()
{
    int i, j;
    float add=0.3;
  	for (i=0; i<total; i++)
	{
	   add=(*Chrono3-explosion1[i]->t+100000);
	   add=25000/add;
	   accelerate(0, &explosion1[i]->speed, 30);
 	   explosion1[i]->m.t[0]-=explosion1[i]->flightdir.x**Fmult3;
 	   explosion1[i]->m.t[1]-=explosion1[i]->flightdir.y**Fmult3;
 	   explosion1[i]->m.t[2]-=explosion1[i]->flightdir.z**Fmult3;
   	   if (rand() % 2 ==1) explosion1[i]->total--;
   	   for(j=0;j<explosion1[i]->total;j++) explosion1[i]->mult+=add;
   	   if ((*Chrono3-explosion1[i]->t)>800)
       {
          kill(i);
          i--;
       }
    }
}
void FLUSH_CLASS::init(MATRIX_f *m)
{
    zero_m=m;
    total=0;
}
void FLUSH_CLASS::start(VECTOR *pos, char type)
{
    flush[total]=(FLUSH*)malloc(sizeof(FLUSH));
    flush[total]->pos=*pos;
    flush[total]->type=type;
    flush[total]->t=0;
    total++;    
}

void FLUSH_CLASS::kill(short i)
{
    delete flush[i];
    flush[i]=flush[total-1];
    total--;    
}

void FLUSH_CLASS::proceed()
{
    int i;
    for(i=0;i<total;i++)
    {
       flush[i]->t+=*Fmult3*1000;
       flush[i]->scale=2-flush[i]->t/196.0;
       if(flush[i]->t>300)kill(i);
    }
}

void FLUSH_CLASS::draw()
{
    int i;
       glDisable(GL_DEPTH_TEST);

    for(i=0;i<total;i++)
    {
       glPushMatrix();
	   glTranslatef(flush[i]->pos.x, flush[i]->pos.y, flush[i]->pos.z);
   	   glMultMatrix_allegro(zero_m);
       glScalef(flush[i]->scale, flush[i]->scale, flush[i]->scale);
       draw_flash();
       glPopMatrix();
    }
       glEnable(GL_DEPTH_TEST);

}   

void BACKGROUND_CLASS::init(const char model_name[], MATRIX_f *pcm)
{
	lese_3do(&model, model_name );//"3do/box8.3do"
	del_polys(&model);
	m=pcm;
}

void BACKGROUND_CLASS::draw()
{
    glLineWidth (1.25);
    color16(6);
    glPushMatrix();z
    glMultMatrix_allegro(m);
    draw_object_lines(&model.o3d[0]);
    glPopMatrix();
}

void DEBRIS_CLASS::init(const char model_name[], int debris_total, int rot_total, float max_rotation_speed)
{
   	lese_3do(&model, model_name);
   	total=debris_total;
   	rotation_total=rot_total;
   	
   	debris=(MATRIX_f*)malloc(sizeof(MATRIX_f)*total);
   	rotation=(MATRIX_f*)malloc(sizeof(MATRIX_f)*rotation_total);
   	
	float aufl=AUFL;
	int aufl2=AUFL, i;
	for (i=0; i<total; i++)
	{
        get_rotation_matrix_f(&debris[i], rand()%256, rand()%256, rand()%256);
		debris[i].t[0] = (rand() % aufl2)*(DBOX_X/aufl) - DBOX_X/2;
		debris[i].t[1] = (rand() % aufl2)*(DBOX_Y/aufl) - DBOX_Y/2;
		debris[i].t[2] =-(rand() % aufl2)*(DBOX_Z/aufl);

	}
	aufl=156;
	for(i=0;i<rotation_total;i++)
	{
       get_rotation_matrix_f(&rotation[i], (rand()%256-128)/max_rotation_speed, (rand()%256-128)/max_rotation_speed, (rand()%256-128)/max_rotation_speed);
    }
}

void DEBRIS_CLASS::proceed(MATRIX_f *m)
{
	int i;
	float aufl=AUFL;
	int aufl2=AUFL;

	for (i=0; i<total; i++)
	{
       matrix_mul_f(&rotation[i%rotation_total], &debris[i], &debris[i]);
       matrix_mul_f(&debris[i], m, &debris[i]);
       if (debris[i].t[0] <  (-DBOX_X/2))
       {
          debris[i].t[0]+=DBOX_X;
	      debris[i].t[1] = (rand() % aufl2)*(DBOX_Y/aufl) - DBOX_Y/2;
		  debris[i].t[2] =-(rand() % aufl2)*(DBOX_Z/aufl);
       }

       else if (debris[i].t[0] >= (DBOX_X/2))
       {
          debris[i].t[0]-=DBOX_X;
     	  debris[i].t[1] = (rand() % aufl2)*(DBOX_Y/aufl) - DBOX_Y/2;
		  debris[i].t[2] =-(rand() % aufl2)*(DBOX_Z/aufl);
	   }	  
       if (debris[i].t[1] <  (-DBOX_Y/2))
       {
   		  debris[i].t[0] = (rand() % aufl2)*(DBOX_X/aufl) - DBOX_X/2;
          debris[i].t[1]+=DBOX_Y;
		  debris[i].t[2] =-(rand() % aufl2)*(DBOX_Z/aufl);
       }
       else if (debris[i].t[1] >=(DBOX_Y/2))
              {
   		  debris[i].t[0] = (rand() % aufl2)*(DBOX_X/aufl) - DBOX_X/2;
          debris[i].t[1]-=DBOX_Y;
		  debris[i].t[2] =-(rand() % aufl2)*(DBOX_Z/aufl);
       }
       if (debris[i].t[2] <  (-DBOX_Z -DBOX_ZZ) )
       {
   		  debris[i].t[0] = (rand() % aufl2)*(DBOX_X/aufl) - DBOX_X/2;
		  debris[i].t[1] = (rand() % aufl2)*(DBOX_Y/aufl) - DBOX_Y/2;
          debris[i].t[2]+=DBOX_Z;
       }
       else if (debris[i].t[2] >= 0 )
       {
   		  debris[i].t[0] = (rand() % aufl2)*(DBOX_X/aufl) - DBOX_X/2;
		  debris[i].t[1] = (rand() % aufl2)*(DBOX_Y/aufl) - DBOX_Y/2;
          debris[i].t[2]-=DBOX_Z;
       }
	}
}

void DEBRIS_CLASS::draw()
{
    int i;
   	for (i = 0; i < MAXDEBRIS; i++)
	{
	   glPushMatrix();
       glMultMatrix_allegro(&debris[i]);
       draw_object(&model.o3d[0]);
	   glPopMatrix();	
	}
}


void EXPLOSION2_CLASS::init(int part_total, MATRIX_f *m)
{
    int i;
    VECTOR v1;
    zero_m=m;
    particle_total=part_total;
    particles=(VECTOR*)malloc(sizeof(VECTOR)*particle_total);
   	for (i=0; i<particle_total; i++)
	{
	   particles[i].x=0; 
	   while (((particles[i].x*particles[i].x + 
                particles[i].y*particles[i].y +
                particles[i].z*particles[i].z  ) > 262144) || !particles[i].x)
	   {
		  particles[i].x = rand() % 1024 - 512;
		  particles[i].y = rand() % 1024 - 512;
		  particles[i].z = rand() % 1024 - 512;
	   }
	   normalize_vector_v(&particles[i]);
	}
	total=0;
}


void EXPLOSION2_CLASS::draw()
{
    int i, j;
    float mult;
    color16(15);
          glDisable(GL_DEPTH_TEST);

	for (j=0; j<total; j++)
	{
	   glPushMatrix();
	   glTranslatef(explosion2[j]->pos.x, explosion2[j]->pos.y, explosion2[j]->pos.z);
	   glMultMatrix_allegro(zero_m);
	   for (i=0; i<explosion2[j]->total;i++)
	   {
	      mult=explosion2[j]->particles[i]*explosion2[j]->mult;
	      glTranslatef(particles[i].x*mult, particles[i].y*mult, particles[i].z*mult);
	      glBegin(GL_QUADS);
	      glVertex3f(-0.1, 0.1,0);
	      glVertex3f(-0.1,-0.1,0);
	      glVertex3f( 0.1,-0.1,0);
	      glVertex3f( 0.1, 0.1,0);
	      glEnd();
       }
	   glPopMatrix();
    }
      glEnable(GL_DEPTH_TEST);

}

void EXPLOSION2_CLASS::kill(short i)
{
    free(explosion2[i]->particles);
    free(explosion2[i]);
    explosion2[i]=explosion2[total-1];
    total--;
}

void EXPLOSION2_CLASS::proceed()
{
    int i, j;
    float add;
  	for (i=0; i<total; i++)
	{
	   add=(*Chrono3-explosion2[i]->t+400);
	   add=8/add;
	   accelerate(0, &explosion2[i]->speed, 30);
 	   explosion2[i]->pos.x-=explosion2[i]->m.v[2][0]*explosion2[i]->speed**Fmult3;
  	   explosion2[i]->pos.y-=explosion2[i]->m.v[2][1]*explosion2[i]->speed**Fmult3;
   	   explosion2[i]->pos.z-=explosion2[i]->m.v[2][2]*explosion2[i]->speed**Fmult3;
   	   if (rand() % 2 ==1) explosion2[i]->total--;
   	   for(j=0;j<explosion2[i]->total;j++) explosion2[i]->mult+=add;
   	   if ((*Chrono3-explosion2[i]->t)>1800)
       {
          kill(i);
          i--;
       }
    }
}



void EXPLOSION2_CLASS::start(MATRIX_f *m, float *speed, UNIT *property)
{
    int  j;
    explosion2[total]=(EXPLOSION2*)malloc(sizeof(EXPLOSION2));
    explosion2[total]->particles=(float*)malloc(sizeof(float)*particle_total);
    explosion2[total]->pos.x=m->t[0];
    explosion2[total]->pos.y=m->t[1];
    explosion2[total]->pos.z=m->t[2];
    explosion2[total]->t=*Chrono3;
    explosion2[total]->mult=0.1;
    explosion2[total]->total=particle_total;
    explosion2[total]->m=*m;
    explosion2[total]->speed=*speed;
    explosion2[total]->property=property;
    for(j=0;j<explosion2[total]->total;j++)
    {
       explosion2[total]->particles[j]=rand() % 1024;
       explosion2[total]->particles[j]/=2048;
    }
    total++;
}

void SHOT_CLASS::init(char model_name[])//"3do/laser.3do"
{
 	lese_3do(&model, model_name);
 	total=0;
}

void SHOT_CLASS::kill(short i)
{
   delete shot[i];
//   free(shot[i]);
   total--;
   if(total)
   {
      shot[i]=shot[total];
      shot[i]->index=i;
   }
}


void SHOT_CLASS::proceed()
{
   int i;
   for (i=0; i<total; i++)
   {
       printf("%d\n", total);

//      vector_sub((VECTOR*)&shot[i]->m.t[0], &v_mul((VECTOR*)&shot[i]->m.v[2][0], *Fmult3), (VECTOR*)&shot[i]->m.t[0]);
	  shot[i]->m.t[0]-=shot[i]->m.v[2][0]*shot[i]->speed**Fmult3;
	  shot[i]->m.t[1]-=shot[i]->m.v[2][1]*shot[i]->speed**Fmult3;
	  shot[i]->m.t[2]-=shot[i]->m.v[2][2]*shot[i]->speed**Fmult3;
//             printf("oha\n");
//      if(!shot[i]->active)
	  if ((*Chrono3-shot[i]->t>SHOTTIME) || !shot[i]->active)
      {
         kill(i);
         i--;
      }  
//	  else if (*Chrono3-shot[i]->t>SHOTTIME)
//      {
//         shot[i]->active=FALSE;   
//      }
   }
}

void SHOT_CLASS::start(UNIT *u, const char name[], char type, signed char sign)
{
    MATRIX_f m;      
    VECTOR pos;
    shot[total]= new SHOT;
    shot[total]->index=total;
//    printf("%d, %d\n", total, shot[total]);
//    if(!shot[total])allegro_message("%d", shot[total]);
    shot[total]->type=type;
    shot[total]->property=u;
    get_object_matrix_world(&shot[total]->m, u, get_object_by_name(&u->udata->model, name));
    get_y_rotate_matrix_f(&m, u->gunangel*-sign);
    pos=trans_matrix_to_vector(&shot[total]->m);
    zero_matrix(&shot[total]->m);
    matrix_mul_f(&shot[total]->m, &m, &shot[total]->m);
    get_z_rotate_matrix_f(&m, 9.24*sign);
    matrix_mul_f(&shot[total]->m, &m, &shot[total]->m);
    translate_matrix_v(&shot[total]->m, &pos);
    shot[total]->speed=SHOTSPEED+u->speed;
    shot[total]->t=*Chrono3;
    shot[total]->active=TRUE;
    total++;
//    printf("test\n");
}


void SHOT_CLASS::draw()
{
   int i, p, j=0;
   glDisable(GL_LIGHTING);
	for (i=0; i<total; i++)
	if(shot[i]->rendered)
	{
	   glPushMatrix();
	   glMultMatrix_allegro(&shot[i]->m);
	   draw_object(&model.o3d[0]);
	   glPopMatrix();
	}
    glEnable(GL_LIGHTING);
}


void EXPLOSION3_CLASS::init(const char model_name[], int part_total)
{
    int i;
    VECTOR v1;
    lese_3do(&model, model_name);
    particle_total=part_total;
    particles=(MATRIX_f*)malloc(sizeof(MATRIX_f)*particle_total);
   	for (i=0; i<particle_total; i++)
	{
	   v1.x = rand() % 1024 - 512;
	   v1.y = rand() % 1024 - 512;
	   v1.z = rand() % 64 +  400;
	   normalize_vector_v(&v1);
	   particles[i]=align_matrix_z(v1);
	}
	total=0;
}

void EXPLOSION3_CLASS::draw()
{
    int i, j;
    float mult;
    glDisable(GL_LIGHTING);

   	for (j=0; j<total; j++)
	{
   	   glPushMatrix();
   	   glMultMatrix_allegro(&explosion1[j]->m);
   	   for (i=0; i<explosion1[j]->total;i++)
   	   {
          glPushMatrix();
	      mult=explosion1[j]->particles[i]*explosion1[j]->mult;
	      glTranslatef(particles[i].v[2][0]*mult, particles[i].v[2][1]*mult, particles[i].v[2][2]*mult);
	      glMultMatrix_allegro(&particles[i]);
	      draw_object_alpha(&model.o3d[0], 240);	      
	      glPopMatrix();
       }
   	   glPopMatrix();
    }
        glEnable(GL_LIGHTING);

}

void EXPLOSION3_CLASS::kill(short i)
{
   free(explosion1[i]->particles);
   free(explosion1[i]);
   total--;
   if(total)explosion1[i]=explosion1[total];

}

void EXPLOSION3_CLASS::start(VECTOR *flightdir, MATRIX_f *m)
{
    int j;
    explosion1[total]=(EXPLOSION1*)malloc(sizeof(EXPLOSION1));
    explosion1[total]->particles=(float*)malloc(sizeof(float)*particle_total);
    explosion1[total]->m=*m;
    explosion1[total]->t=*Chrono3;
    explosion1[total]->mult=0.1;
    explosion1[total]->total=particle_total;
    explosion1[total]->flightdir=*flightdir;
    for(j=0;j<explosion1[total]->total;j++)
    {
       explosion1[total]->particles[j]=rand() % 1024;
       explosion1[total]->particles[j]/=2048;
       explosion1[total]->particles[j]+=0.1;
    }
    total++;
}

void EXPLOSION3_CLASS::proceed()
{
    int i, j;
    float add=0.3;
  	for (i=0; i<total; i++)
	{
	   add=(*Chrono3-explosion1[i]->t+100000);
	   add=25000/add;
	   accelerate(0, &explosion1[i]->speed, 30);
 	   explosion1[i]->m.t[0]-=explosion1[i]->flightdir.x**Fmult3;
 	   explosion1[i]->m.t[1]-=explosion1[i]->flightdir.y**Fmult3;
 	   explosion1[i]->m.t[2]-=explosion1[i]->flightdir.z**Fmult3;
   	   if (rand() % 2 ==1) explosion1[i]->total--;
   	   for(j=0;j<explosion1[i]->total;j++) explosion1[i]->mult+=add;
   	   if ((*Chrono3-explosion1[i]->t)>800)
       {
          kill(i);
          i--;
       }
    }
}