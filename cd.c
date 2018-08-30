/* Hier stehen Funktionen, die die Kollisionsabfragen ausführen
 *
 */

#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "harbst.h"
#include "mat.h"
#include "cd.h"

#define MAXCOLMODELS 32
#define CHECKDISTANCE 1      //pro Meter eine Kollisionsabfrage

COLMODEL_CLASS *colmodel[MAXCOLMODELS];
int colmodel_anz = 0;      


COLMODEL_CLASS* assign_colmodel(const char name[])
{
    int i;
    for(i=0;i<colmodel_anz;i++)
    if(!strcmp(colmodel[i]->name, name))
    {
       return (colmodel[i]);
    }

    colmodel[colmodel_anz]=new(COLMODEL_CLASS);
    MODEL *model=load_3do(name);
    int p, j=0;
    colmodel[colmodel_anz]->ColModel3d=newCollisionModel3D();
    for(p=0;p<model->o3d[0].poly_anz;p++)
    {
       colmodel[colmodel_anz]->ColModel3d->addTriangle(&model->o3d[0].vert[model->o3d[0].i[j]].x,
                                                       &model->o3d[0].vert[model->o3d[0].i[j+1]].x,
                                                       &model->o3d[0].vert[model->o3d[0].i[j+2]].x);
       j+=model->o3d[0].ind_anz[p];
    }
    colmodel[colmodel_anz]->ColModel3d->finalize();
    colmodel[colmodel_anz]->n=model->o3d[0].n;  //muß eigentlich kopiert werden, nicht nur referenziell
    colmodel[colmodel_anz]->name=name;
    colmodel_anz++;
//    allegro_message(name);
    return (colmodel[colmodel_anz-1]);
}




bool COLMODEL_CLASS::check_colmodel(MATRIX_f *m1, VECTOR *flightdir1, COLMODEL_CLASS *colmodel2, MATRIX_f *m2, VECTOR *flightdir2)
{
    int t1, t2, iteration=0;
    float col_point[3];
    bool hit=0;
    static GLfloat glm1[16], glm2[16];
    allegro_gl_MATRIX_f_to_GLfloat(m1, glm1);
    allegro_gl_MATRIX_f_to_GLfloat(m2, glm2);
    colmodel2->ColModel3d->setTransform(glm2);
    this->ColModel3d->setTransform(glm1);
    return(this->ColModel3d->collision(colmodel2->ColModel3d));
}
//    if (this->ColModel3d->collision(comodel2->ColModel3d))
//    {
//       hit=TRUE;
//       u1->m.t[0]+=u1->flightdir.x*get_fmult()/2;
//       u1->m.t[1]+=u1->flightdir.y*get_fmult()/2;
//       u1->m.t[2]+=u1->flightdir.z*get_fmult()/2;
//       u1->udata->model.colmodel->setTransform (glm1);
//       if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
//       {
//          u1->m.t[0]+=u1->flightdir.x*get_fmult()/4;
//          u1->m.t[1]+=u1->flightdir.y*get_fmult()/4;
//          u1->m.t[2]+=u1->flightdir.z*get_fmult()/4;
//          u1->udata->model.colmodel->setTransform (glm1);
//          if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
//          {
//             u1->m.t[0]+=u1->flightdir.x*get_fmult()/4;
//             u1->m.t[1]+=u1->flightdir.y*get_fmult()/4;
//             u1->m.t[2]+=u1->flightdir.z*get_fmult()/4;
//          }
//       }
//       else
//       {
//          u1->m.t[0]-=u1->flightdir.x*get_fmult()/4;
//          u1->m.t[1]-=u1->flightdir.y*get_fmult()/4;
//          u1->m.t[2]-=u1->flightdir.z*get_fmult()/4;
//          u1->udata->model.colmodel->setTransform (glm1);
//          if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
//          {
//             u1->m.t[0]+=u1->flightdir.x*get_fmult()/4;
//             u1->m.t[1]+=u1->flightdir.y*get_fmult()/4;
//             u1->m.t[2]+=u1->flightdir.z*get_fmult()/4;
//          }
//       }
//    }
//    return(hit);
//}

bool COLMODEL_CLASS::check_projectile(MATRIX_f *pro_m, float pro_speed, float pro_length, MATRIX_f *col_m, float col_speed)
{
    float  mult, speed1, speed2;
    float app_speed;
    int checks_per_frame;
    float pro_pos[3];
    bool hit=0;
    int j;
    static GLfloat glm[16];
    
    app_speed=get_approaching_speed(pro_m, &pro_speed, col_m, &col_speed);  //Annäherungsgeschwindigkeit m/s
    
    if(app_speed<0)app_speed*=-1;
    checks_per_frame=app_speed*get_fmult()/CHECKDISTANCE;
    if(checks_per_frame<1)checks_per_frame=1; 
    pro_pos[0]=(pro_m->t[0]);pro_pos[1]=(pro_m->t[1]);pro_pos[2]=(pro_m->t[2]);
    speed1=pro_speed*get_fmult()/checks_per_frame;
    speed2=col_speed*get_fmult()/checks_per_frame;
    allegro_gl_MATRIX_f_to_GLfloat(col_m, glm);
    glm[12]=col_m->t[0];
    glm[13]=col_m->t[1];
    glm[14]=col_m->t[2];
             
   for(j=0;j<checks_per_frame;j++)
    {
       this->ColModel3d->setTransform (glm); 
       hit=this->ColModel3d->rayCollision (&pro_pos[0], &pro_m->v[2][0], 1, -pro_length, pro_length+CHECKDISTANCE);
       if(hit)break;
       pro_pos[0]-=pro_m->v[2][0]*speed1;
       pro_pos[1]-=pro_m->v[2][1]*speed1;
       pro_pos[2]-=pro_m->v[2][2]*speed1;
       glm[12]-=col_m->v[2][0]*speed2;
       glm[13]-=col_m->v[2][1]*speed2;
       glm[14]-=col_m->v[2][2]*speed2;
    } 
    return(hit);
}


bool check_col(MATRIX_f *m1, MATRIX_f *m2, float bounding)  //Funktion zum schnellen BoundingBox Kollisionstest
{
    if (((m1->t[0]-m2->t[0])<bounding) && ((m1->t[0]-m2->t[0])> -bounding))
    if (((m1->t[1]-m2->t[1])<bounding) && ((m1->t[1]-m2->t[1])> -bounding))
    if (((m1->t[2]-m2->t[2])<bounding) && ((m1->t[2]-m2->t[2])> -bounding))
    return(TRUE);
    return(FALSE);
}

