/* 
 * In dieser Datei sind alle vereinfachenden mathematischen Funktionen
 * enthalten.
 */
 
 
#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "coldet.h"
#include "harbst.h"
#include "cd.h"
#include "global.h"
#include "mat.h"

void invert_matrix(MATRIX_f *in, MATRIX_f *out)
{
	out->v[0][0]=in->v[0][0]; out->v[0][1]=in->v[1][0]; out->v[0][2]=in->v[2][0];
	out->v[1][0]=in->v[0][1]; out->v[1][1]=in->v[1][1]; out->v[1][2]=in->v[2][1];
	out->v[2][0]=in->v[0][2]; out->v[2][1]=in->v[1][2]; out->v[2][2]=in->v[2][2];
	out->t[0]=-in->t[0];out->t[1]=-in->t[1];out->t[2]=-in->t[2];
}


void get_matrix_delta(MATRIX_f *m_act, MATRIX_f *m_old, MATRIX_f *m_delta)
{
    MATRIX_f m;
    invert_matrix(m_old, &m);
    matrix_mul_f(m_act, &m, &m);
    invert_matrix(&m, m_delta);
}



void zero_matrix(MATRIX_f *m) //lösche den Translationsteil einer MATRIX_f
{
    m->t[0]=0;
    m->t[1]=0;
    m->t[2]=0;
}




float distance(VECTOR *pos1, VECTOR *pos2)
{
   return(vector_length_f(pos1->x - pos2->x, pos1->y - pos2->y, pos1->z - pos2->z));
}




void cross_product_v(VECTOR *pos0, VECTOR *pos1, VECTOR *pos2, VECTOR *out)
{
    VECTOR normal;
    cross_product_f(
    pos1->x-pos0->x,
    pos1->y-pos0->y,
    pos1->z-pos0->z,
    pos2->x-pos0->x,
    pos2->y-pos0->y,
    pos2->z-pos0->z,
    &out->x,
    &out->y,
    &out->z);
}



VECTOR vect(float x, float y, float z)
{
   VECTOR ret;
   ret.x=x;
   ret.y=y;
   ret.z=z;
   return(ret);
}

float vector_product_v(VECTOR *v1, VECTOR *v2)
{
   return(v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}


MATRIX_f matr(float x, float y, float z, float xrot, float yrot, float zrot)
{
    MATRIX_f m;
    get_transformation_matrix_f(&m, 1.0, xrot, yrot, zrot, x, y, z);
    return(m);
}

void vector_add(VECTOR *in1, VECTOR *in2, VECTOR *out)
{
    out->x=in1->x+in2->x;
    out->y=in1->y+in2->y;
    out->z=in1->z+in2->z;
}
void vector_sub(VECTOR *in1, VECTOR *in2, VECTOR *out)
{
    out->x=in1->x-in2->x;
    out->y=in1->y-in2->y;
    out->z=in1->z-in2->z;
}

void vector_mul(VECTOR *v1, float mul)
{
    v1->x*=mul;
    v1->y*=mul;
    v1->z*=mul;
}


VECTOR v_mul(VECTOR *in, float mul)
{
    VECTOR out=*in;
    out.x*=mul;
    out.y*=mul;
    out.z*=mul;
    return(out);
}

void apply_matrix_v(MATRIX_f *m, VECTOR *in, VECTOR *out)
{
    apply_matrix_f(m, in->x, in->y, in->z, &out->x, &out->y, &out->z);
}

void normalize_vector_v(VECTOR *v1)
{
   normalize_vector_f(&v1->x, &v1->y, &v1->z);
}


bool spot_is_visible(VECTOR *heading, float bounding) //ist ein Punkt auf dem Schim sichtbar?
{
   if (heading->z > 0)
   {
      if (heading->x < (XHEADING+bounding))
      if (heading->x >-(XHEADING+bounding))
      if (heading->y < (YHEADING+bounding))
      if (heading->y >-(YHEADING+bounding))
      {    
         return(TRUE);
      }
   }
   return(FALSE);
}

float vector_length_v(VECTOR *v1)
{
   return(vector_length_f(v1->x, v1->y, v1->z));
}

VECTOR random_vector()
{
    VECTOR out;
    out.x=rand() % 2048 - 1024;
    out.y=rand() % 2048 - 1024;
    out.z=rand() % 2048 - 1024;
    normalize_vector_v(&out);
    return(out);
}

MATRIX_f align_matrix_z(VECTOR z) //liefert an Vector z ausgerichtete Matrix zurück
{
    MATRIX_f m=identity_matrix_f;
    VECTOR v;
    v=random_vector();
    
    cross_product_f(
    z.x, z.y, z.z,
    v.x, v.y, v.z,
   &m.v[0][0],&m.v[0][1],&m.v[0][2]);
  
   cross_product_f(
    z.x, z.y, z.z,
    m.v[0][0], m.v[0][1], m.v[0][2],
   &m.v[1][0],&m.v[1][1],&m.v[1][2]);
   
   m.v[2][0]=z.x;
   m.v[2][1]=z.y;
   m.v[2][2]=z.z;
   return(m);
}


void get_heading(MATRIX_f *m1, MATRIX_f *m2, VECTOR *out)
{
    MATRIX_f m=*m1;
    m.t[0]=0;m.t[1]=0;m.t[2]=0;
	out->x=m1->t[0]-m2->t[0];
	out->y=m1->t[1]-m2->t[1];
	out->z=m1->t[2]-m2->t[2];
	apply_matrix_f(&m, out->x, out->y, out->z, &out->x, &out->y, &out->z);
	normalize_vector_f(&out->x, &out->y, &out->z);
}

void get_heading(MATRIX_f *m1, VECTOR *in, VECTOR *out)
{
   MATRIX_f m=*m1;
   zero_matrix(&m);
   vector_sub((VECTOR*)&m1->t[0], in, out);
   apply_matrix_v(&m, out, out);
	normalize_vector_v(out);
}

float get_approaching_speed(MATRIX_f *m1, float *speed1, MATRIX_f *m2, float *speed2)
{
   VECTOR heading1, heading2;
   get_heading(m1, m2, &heading1);
   get_heading(m2, m1, &heading2);
   return(heading1.z**speed1+heading2.z**speed2);
}

VECTOR operator+(VECTOR a, VECTOR b)
{
    VECTOR c;
    c.x=a.x+b.x;
    c.y=a.y+b.y;
    c.z=a.z+b.z;
    return(c);
}

VECTOR operator+=(VECTOR a, VECTOR b)
{
    VECTOR c;
    c.x=a.x+b.x;
    c.y=a.y+b.y;
    c.z=a.z+b.z;
    return(c);
}

VECTOR operator*(VECTOR a, float b)
{
    VECTOR c;
    c.x=a.x*b;
    c.y=a.y*b;
    c.z=a.z*b;
    return(c);
}
