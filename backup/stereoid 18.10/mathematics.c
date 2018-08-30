/* mathematics.c
 * In dieser Datei sind alle vereinfachenden mathematischen Funktionen
 * enthalten.
 */
 
 
#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
//#include <GL/glext.h>
#include <GL/glu.h>
#include "global.h"
#include "mathematics.h"

GLfloat glm[16];
float        *Fmult2;

void init_mathematics(float *fmult)
{
    Fmult2=fmult;
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

void invert_matrix(MATRIX_f *in, MATRIX_f *out)
{
	out->v[0][0]=in->v[0][0]; out->v[0][1]=in->v[1][0]; out->v[0][2]=in->v[2][0];
	out->v[1][0]=in->v[0][1]; out->v[1][1]=in->v[1][1]; out->v[1][2]=in->v[2][1];
	out->v[2][0]=in->v[0][2]; out->v[2][1]=in->v[1][2]; out->v[2][2]=in->v[2][2];
}

bool accelerate(float soll, float *ist, float a)
{
	a*=*Fmult2;
	if (*ist<soll )
	{
		*ist+=a;
		if (*ist > soll)
        {
           *ist=soll;
           return(TRUE);
        }
	}
	if (*ist > soll)
	{
		*ist-=a;
		if (*ist < soll)
        {
           *ist=soll;
           return(TRUE);
        }
	}
    return(FALSE);
}

void get_matrix_delta(MATRIX_f *m_act, MATRIX_f *m_old, MATRIX_f *m_delta)
{
    MATRIX_f m;
    invert_matrix(m_old, &m);
    matrix_mul_f(m_act, &m, &m);
    invert_matrix(&m, m_delta);    
}

float get_approaching_speed(MATRIX_f *m1, float *speed1, MATRIX_f *m2, float *speed2)
{
   VECTOR heading1, heading2;
   get_heading(m1, m2, &heading1);
   get_heading(m2, m1, &heading2);
   return(heading1.z**speed1+heading2.z**speed2);
}

void glMultMatrix_allegro(MATRIX_f *m)
{
    allegro_gl_MATRIX_f_to_GLfloat(m, glm);
    glMultMatrixf(glm);
}

void zero_matrix(MATRIX_f *m)
{
    m->t[0]=0;
    m->t[1]=0;
    m->t[2]=0;
}

VECTOR trans_matrix_to_vector(MATRIX_f *m)
{
    VECTOR pos;
    pos.x=m->t[0];
    pos.y=m->t[1];
    pos.z=m->t[2];
    return(pos);
}

void translate_matrix_v(MATRIX_f *m, VECTOR *pos)
{
               
   m->t[0]=pos->x;
   m->t[1]=pos->y;
   m->t[2]=pos->z;
}

float distance(VECTOR *pos1, VECTOR *pos2)
{
   return(vector_length_f(pos1->x-pos2->x, pos1->y-pos2->y, pos1->z-pos2->z));
}
