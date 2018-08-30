/* Hier stehen Funktionen, die die Kollisionsabfragen ausführen
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
#include "cd.h"

float *FmultX;

void init_cd(float *FmultX1)
{
    FmultX=FmultX1;
}


void o3d_to_collision_model()
{
}

void cd_missile_fighter(short u1, short u2)
{
}

void cd_missile_object(short u1, short u2)
{
}

void cd_missile_missile(short u1, short u2)
{
}

bool cd_shot_unit(SHOT *sht, UNIT *u)
{
    float  mult, speed1, speed2;
    float app_speed;
    int checks_per_frame;
    VECTOR pos;
    bool hit=0;
    int j;
    static GLfloat glm[16];
    app_speed=get_approaching_speed(&sht->m, &sht->speed, &u->m, &u->speed);
    if(app_speed<0)app_speed*=-1;
    checks_per_frame=app_speed**FmultX/CHECKDISTANCE;
    if(checks_per_frame<1)checks_per_frame=1; 
    pos=trans_matrix_to_vector(&sht->m);

    speed1=sht->speed**FmultX/checks_per_frame;
    speed2=u->speed**FmultX/checks_per_frame;

    if(u->object[0].moved)
    {
       MATRIX_f m=u->object[0].m;
       matrix_mul_f(&m, &u->m, &m);
       allegro_gl_MATRIX_f_to_GLfloat(&m, glm);
    }
    else allegro_gl_MATRIX_f_to_GLfloat(&u->m, glm);
    glm[12]=u->m.t[0];
    glm[13]=u->m.t[1];
    glm[14]=u->m.t[2];
            
   for(j=0;j<checks_per_frame;j++)
    {
       u->udata->model.colmodel->setTransform (glm); 
       hit=u->udata->model.colmodel->rayCollision (&pos.x, &sht->m.v[2][0], 1, -SHOTLENGTH, SHOTLENGTH+CHECKDISTANCE);
       if(hit)break;
       pos.x-=sht->m.v[2][0]*speed1;
       pos.y-=sht->m.v[2][1]*speed1;
       pos.z-=sht->m.v[2][2]*speed1;
       glm[12]-=u->m.v[2][0]*speed2;
       glm[13]-=u->m.v[2][1]*speed2;
       glm[14]-=u->m.v[2][2]*speed2;
    } 
    return(hit);
    
}


bool cd_missile_unit(UNIT *u1, UNIT *u2)
{
    float  mult, speed1, speed2;
    float app_speed;
    int checks_per_frame;
    VECTOR pos;
    bool hit=0;
    int j;
    static GLfloat glm[16];
    app_speed=get_approaching_speed(&u1->m, &u1->speed, &u2->m, &u2->speed);
    if(app_speed<0)app_speed*=-1;
    checks_per_frame=app_speed**FmultX/CHECKDISTANCE;
    if(checks_per_frame<1)checks_per_frame=1; 
    pos=trans_matrix_to_vector(&u1->m);
    speed1=u1->speed**FmultX/checks_per_frame;
    speed2=u2->speed**FmultX/checks_per_frame;
    if(u2->object[0].moved)
    {
       MATRIX_f m=u2->object[0].m;
       matrix_mul_f(&m, &u2->m, &m);
       allegro_gl_MATRIX_f_to_GLfloat(&m, glm);
    }
    else allegro_gl_MATRIX_f_to_GLfloat(&u2->m, glm);
    glm[12]=u2->m.t[0];
    glm[13]=u2->m.t[1];
    glm[14]=u2->m.t[2];
             
   for(j=0;j<checks_per_frame;j++)
    {
       u2->udata->model.colmodel->setTransform (glm); 
       hit=u2->udata->model.colmodel->rayCollision (&pos.x, &u1->m.v[2][0], 1, -MISSILELENGTH, MISSILELENGTH+CHECKDISTANCE);
       if(hit)break;
       pos.x-=u1->m.v[2][0]*speed1;
       pos.y-=u1->m.v[2][1]*speed1;
       pos.z-=u1->m.v[2][2]*speed1;
       glm[12]-=u2->m.v[2][0]*speed2;
       glm[13]-=u2->m.v[2][1]*speed2;
       glm[14]-=u2->m.v[2][2]*speed2;
    } 
    return(hit);
}

bool check_col(MATRIX_f *m1, MATRIX_f *m2, float bounding)
{
    if (((m1->t[0]-m2->t[0])<bounding) && ((m1->t[0]-m2->t[0])> -bounding))
    if (((m1->t[1]-m2->t[1])<bounding) && ((m1->t[1]-m2->t[1])> -bounding))
    if (((m1->t[2]-m2->t[2])<bounding) && ((m1->t[2]-m2->t[2])> -bounding))
    return(TRUE);
    return(FALSE);
}

