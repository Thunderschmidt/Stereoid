/* actions.c
 * Dies ist die Bibliothek, die die ACTION - structs bearbeitet und
 * die Eigenschaften der Unterobjekte der units verändern kann.
 * Durch die Befehle wait und wait_for können viele Eigenschaften zeitgesteuert
 * verändert werden.
 * Außerdem läßt sich durch
 * start_function(UNIT *unit, ANIM_FUNCT anim_funct)
 * eine Funktion zeitverzögert aufrufen. Geignet für stereotype
 * Bewegungen
 *
 * Diese Bibliothek ist noch zu sehr von anderem Stereoid-Code abhängig.
 * Ziel ist, "unit" herauszuwerfen, da andere Stereo-Programme vielleicht nicht mehr mit dem unit-struct arbeiten werden
 */

#define MAXMODELS 32
#define MAXACTIONS 256

//#include <allegro.h>
//#include "input3do.h"


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "harbst.h"
#include "input3DO.h"
#include "Trans_3D.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////        TIMER         //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

volatile int chrono = 0;
float        fcount, fmult, fps_rate = 60.0;
int          frame_count = 0,
             frame_count_time = 0;


void the_timer(void)
{
	chrono++;
} END_OF_FUNCTION(the_timer);

void proceed_timer()
{
    frame_count++;
    fmult=(chrono-fcount)/1000;
    fcount=chrono;
    if (frame_count >= 15)
	{
		fps_rate = frame_count * 1000.0 / (chrono - frame_count_time);
		frame_count -= 15;
		frame_count_time = chrono;
	}
}

float get_fps(){return(fps_rate);}
float get_fmult(){return(fmult);}
int get_chrono(){return(chrono);}

void init_timer()
{
    install_int(the_timer, 1);
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);
    fcount=chrono;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////        MATH         ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

MATR_STACK_CLASS matr_stack;  //zur Simulation von OpenGL Matrixoperationen
GLfloat glm[16];
 

void glMultMatrix_allegro(MATRIX_f *m) //multipliziere Allegro-Matrix mit OpenGL-Modelview-Matrix
{
    allegro_gl_MATRIX_f_to_GLfloat(m, glm);
    glMultMatrixf(glm);
}

void glMultMatrix_allegro_inv(MATRIX_f *m)//multipliziere inverse Allegro-Matrix mit OpenGL-Modelview-Matrix
{
    glm[0]=m->v[0][0];
    glm[1]=m->v[1][0];
    glm[2]=m->v[2][0];
    
    glm[4]=m->v[0][1];
    glm[5]=m->v[1][1];
    glm[6]=m->v[2][1];
    
    glm[8]=m->v[0][2];
    glm[9]=m->v[1][2];
    glm[10]=m->v[2][2];
    
    glm[12]=0;
    glm[13]=0;
    glm[14]=0;
    glMultMatrixf(glm);
    glTranslatef(-m->t[0],-m->t[1],-m->t[2]);
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

bool accelerate(float soll, float *ist, float a)
{
	a*=fmult;
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
    if (*ist==soll) return(TRUE);
    return(FALSE);
}

void MATR_STACK_CLASS::mul_alg(MATRIX_f *m) //Klasse für simulierten OpenGL-Stack. Wird für Unterobjekte benötigt
{
     matrix_mul_f(&actual, m, &actual);
}

void MATR_STACK_CLASS::translate(VECTOR *v)
{
    actual.t[0]+=v->x*actual.v[0][0];
    actual.t[1]+=v->x*actual.v[0][1];
    actual.t[2]+=v->x*actual.v[0][2];
     
    actual.t[0]+=v->y*actual.v[1][0];
    actual.t[1]+=v->y*actual.v[1][1];
    actual.t[2]+=v->y*actual.v[1][2];

    actual.t[0]+=v->z*actual.v[2][0];
    actual.t[1]+=v->z*actual.v[2][1];
    actual.t[2]+=v->z*actual.v[2][2];
}

void MATR_STACK_CLASS::mul(MATRIX_f *m)
{
     translate((VECTOR*)&m->t[0]);
     MATRIX_f m2=*m;
     m2.t[0]=0;m2.t[1]=0;m2.t[2]=0;
     matrix_mul_f(&m2, &actual, &actual);     
}

void MATR_STACK_CLASS::push()
{
    stack[total]= new MATRIX_f;
    *stack[total]=actual;
    total++;
}

void MATR_STACK_CLASS::pop()
{
    if(total>0)
    { 
    total--;
    actual=*stack[total];
    delete stack[total];
    }
}


void MATR_STACK_CLASS::loadIdentity()
{
    int i;
    for(i=0; i<total; i++)delete stack[i];
    total=0;
    actual=identity_matrix_f;
}

MATRIX_f MATR_STACK_CLASS::get()
{
    return(actual);
}

MATR_STACK_CLASS::MATR_STACK_CLASS()
{
    total=0;
}
                        

////////////////////////////////////////////////////////////////////////////////
////////////////////////////       MODEL3D       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct ACTION
{
    short index;         //index
    bool active;         
    signed char status;  //ACT_ACTIVE, ACT_INACTIVE, ACT_FINISHED
    signed char type;    //ACT_TURN, ACT_MOVE, ACT_WAIT, ACT_WAITFORACT_TURN, ACT_WAITFORACT_MOVE
    MODEL3D_CLASS *model3d;          //welches model3d?
    short o;             //welches Objejkt?
    char axis;           //x- y- z-Achse?
    int t;               //für wait-Actions
    float amount;        //Winkel oder Translation
    float speed;         //Geschwindigkeit der Bewegung
    ACTION *link;        //nächste Aktion (bei Wait und Waitfor)
    ACTION *waitforaction; //für Aktionstyp ACT_WAITFOR: warte auf Beendigung dieser Aktion in dieser Verknüpfung
    ACTION *whowaitsforme; //gibt es eine Funktion, die auf mich wartet? Wenn ja, welche?
    ANIM_FUNCT anim_funct; //Funktionspointer für Schlaufen
} ACTION; 

ACTION act_none;
ACTION *ACT_NONE=&act_none;
ACTION a_actions[MAXACTIONS]; //alle aktiven Actions
ACTION *a_action[MAXACTIONS]; //alle inaktiven Actions
ACTION i_actions[MAXACTIONS];
ACTION *i_action[MAXACTIONS];
volatile int a_action_anz;
volatile int i_action_anz;
MODEL *models[MAXMODELS];      //Zeiger auf alle geladenen Models
int models_anz=0;


MODEL* load_3do(const char name[])
{
    int i;
    for (i=0; i<models_anz; i++)
    {
        if(!strcmp(name, models[i]->name)) return (models[i]);
    }


    models[models_anz]= new MODEL;
    lese_3do(models[models_anz], name);
    models_anz++;
    return(models[models_anz-1]);

}

void unload_3do(const char name[])
{
}

int MODEL3D_CLASS::prepare_object_matrices()
{
    short o;
    free(this->object);
    this->object=(OBJECT*)malloc(this->model->obj_anz*sizeof(OBJECT));
    for(o=0;o<this->model->obj_anz;o++)
    {
       this->object[o].m=identity_matrix_f;
       this->object[o].moved=FALSE;
       this->object[o].visible=TRUE;
       this->object[o].grid_is_visible=TRUE;
       this->object[o].fill_is_visible=TRUE;
       this->object[o].winkel[0]=0;
       this->object[o].winkel[1]=0;
       this->object[o].winkel[2]=0;
       this->object[o].single_colored=FALSE;
       this->object[o].alpha=255;
    }
}

 
void MODEL3D_CLASS::assign_3do(const char name[])
{
   this->model=load_3do(name);
   this->prepare_object_matrices();   
}

MODEL3D_CLASS::~MODEL3D_CLASS()
{
    free(this->object);                              
}

bool MODEL3D_CLASS::get_object_matrix_recursive(int o, int o_soll)
{
    matr_stack.push();
    matr_stack.translate(&this->model->o3d[o].pos);
    if(this->object[o].moved)matr_stack.mul(&this->object[o].m);
    if(o==o_soll) return(TRUE);
    else
    {
    if(this->model->o3d[o].child_o)
    {
       if(get_object_matrix_recursive(this->model->o3d[o].child_o, o_soll)) return(TRUE);
    }
    matr_stack.pop();

    if(this->model->o3d[o].sibl_o)
    {
       if(get_object_matrix_recursive(this->model->o3d[o].sibl_o, o_soll)) return(TRUE);
    }
    }
    return(FALSE);    
}

MATRIX_f MODEL3D_CLASS::get_object_matrix_world(int o, MATRIX_f *m)
{
    matr_stack.loadIdentity();
    matr_stack.mul_alg(m);
    this->get_object_matrix_recursive(0, o);
    return(matr_stack.get());
}


MATRIX_f MODEL3D_CLASS::get_object_matrix(int o)
{
    matr_stack.loadIdentity();
    this->get_object_matrix_recursive(0, o);
    return(matr_stack.get());
}



char MODEL3D_CLASS::get_object_by_name(const char name[])
{
   char o;
   for(o=0;o<this->model->obj_anz;o++) if(!strcmp(name, this->model->o3d[o].name)) return(o);
   return(0);
}


void init_actions()
{
    int i;
    for(i=0;i<MAXACTIONS;i++)
    {
       a_actions[i].index=i;
       i_actions[i].index=i;

       a_actions[i].type=ACT_INACTIVE;
       i_actions[i].type=ACT_INACTIVE;
            
       a_actions[i].active=TRUE;
       i_actions[i].active=FALSE;

       a_actions[i].link=ACT_NONE;
       i_actions[i].link=ACT_NONE;

       a_actions[i].waitforaction=ACT_NONE;
       i_actions[i].waitforaction=ACT_NONE;

       a_actions[i].whowaitsforme=ACT_NONE;
       i_actions[i].whowaitsforme=ACT_NONE;
       
       a_action[i]=&a_actions[i];
       i_action[i]=&i_actions[i];

    }
 //   i_action_anz=i_anz;
 //   a_action_anz=a_anz;
}

int get_a_action_anz()
{
    return (a_action_anz);
}    
int get_i_action_anz()
{
    return (i_action_anz);
}    

void kill_a_action(int i)
{
    ACTION *temp=a_action[i];
    a_action[i]= a_action[a_action_anz-1];
    a_action[a_action_anz-1]=temp;

    a_action[a_action_anz-1]->index=a_action_anz-1;
    a_action[i]->index=i;
    a_action_anz--;
}

void kill_i_action(int i)
{
    ACTION *temp=i_action[i];
    i_action[i]=i_action[i_action_anz-1];
    i_action[i_action_anz-1]=temp;

    i_action[i_action_anz-1]->index=i_action_anz-1;
    i_action[i]->index=i;
    i_action_anz--;
}


void MODEL3D_CLASS::clear_a_action(short o, char axis, signed char type)
{
    int i;
    for(i=0;i<a_action_anz;i++)
    if((a_action[i]->model3d==this) &&
       (a_action[i]->o==o) &&
       (a_action[i]->axis==axis) &&
       (a_action[i]->type==type))
    kill_a_action(i);
}

ACTION* get_action(MODEL3D_CLASS *model3d)
{
    int i, j;
    ACTION *link;
    for(i=0; i<a_action_anz; i++)
    if(((a_action[i]->type == ACT_WAIT) || (a_action[i]->type == ACT_WAITFOR)) && (a_action[i]->model3d == model3d))  //es gibt immer nur höchstens eine AKTIVE WAIT(_FOR)-Action. Ist eine aktiv?
    {
          link=a_action[i];
	      while(link->link != ACT_NONE) link=link->link;//stoße zur letzten Funktion der Verknüpfunsreihe vor
	      link->link=i_action[i_action_anz];//verknüpfe die neue Aktion
          i_action_anz++;
          return(i_action[i_action_anz-1]);             //gib Pointer auf die neue Aktion zurück
    }
    a_action_anz++;
    return(a_action[a_action_anz-1]);
}


void activate_action(ACTION *act)
{
   int i;
   ACTION link;
   act->model3d->clear_a_action(act->o, act->axis, act->type);
 
   *a_action[a_action_anz]=*act;
   if(act->whowaitsforme!=ACT_NONE)act->whowaitsforme->waitforaction=a_action[a_action_anz];
   a_action_anz++;
   
   if(act->link != ACT_NONE)
   if((act->type != ACT_WAIT) && (act->type != ACT_WAITFOR))
   activate_action(act->link);
   kill_i_action(act->index);
}

void stop_a_action(ACTION *act)
{
    if(act->active)
    {
       a_action_anz--;
       act->model3d->clear_a_action(act->o, act->axis, act->type);
       a_action_anz++;
    }
}

void MODEL3D_CLASS::start_blinking(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_BLINKING;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::hide_fill(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_HIDE_FILL;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::show_fill(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_SHOW_FILL;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::hide_grid(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_HIDE_GRID;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::hide_grid(int o)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=o;
    act->type=ACT_HIDE_GRID;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::show_grid(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_SHOW_GRID;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::hide(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_HIDE;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}
void MODEL3D_CLASS::hide(int o)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=o;
    act->type=ACT_HIDE;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::show(const char name[])
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->type=ACT_SHOW;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::start_function(ANIM_FUNCT anim_funct)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->type=ACT_FUNCTION;
    act->anim_funct=anim_funct;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

void MODEL3D_CLASS::move(const char name[], short axis, float amount, float speed)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->axis=axis;
    act->amount=amount;
    act->speed=speed;
    act->type=ACT_MOVE;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
    stop_a_action(act);
}

void MODEL3D_CLASS::turn(const char name[], short axis, float amount, float speed)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->axis=axis;
    act->amount=amount;
    act->speed=speed;
    act->type=ACT_TURN;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
    stop_a_action(act);
}

void MODEL3D_CLASS::spin(const char name[], char axis, float speed)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->o=this->get_object_by_name(name);
    act->axis=axis;
    act->speed=speed;
    act->type=ACT_SPIN;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
    stop_a_action(act);
}

void MODEL3D_CLASS::wait(int t)
{
    ACTION *act=get_action(this);
    act->model3d=this;
    act->t=t;
    act->type=ACT_WAIT;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

ACTION* find_action(MODEL3D_CLASS *model3d, short o, char axis, signed char type)
{
    int i;
    ACTION *ret, *link;
    for(i=0; i<i_action_anz; i++)
    {
       if ((i_action[i]->model3d==model3d) && (i_action[i]->o==o) && (i_action[i]->axis==axis) && (i_action[i]->type==type))
       {
          ret=i_action[i];
          link=ret->link;
          while(link!=ACT_NONE)
          {
             if ((link->model3d==model3d) && (link->o==o) && (link->axis==axis)&& (link->type==type))
                ret=link;
             link=link->link;
          }
          return(ret);
       }
    }
    for(i=0; i<a_action_anz; i++)
    if ((a_action[i]->model3d==model3d) && (a_action[i]->o==o) && (a_action[i]->axis==axis)&& (a_action[i]->type==type))
    return(a_action[i]);
    return(ACT_NONE);
}

bool MODEL3D_CLASS::wait_for_move(const char name[], char axis)
{
    ACTION *act, *link=find_action(this, this->get_object_by_name(name), axis, ACT_MOVE);
    if(link==ACT_NONE) return(FALSE);

    act=get_action(this);
    link->whowaitsforme=act;
    act->model3d=this;
    act->type=ACT_WAITFOR;
    act->link=ACT_NONE;
    act->waitforaction=link;	
    return(TRUE);
}


bool MODEL3D_CLASS::wait_for_turn(const char name[], char axis)
{
    ACTION *act, 
    *link=find_action(this, this->get_object_by_name(name), axis, ACT_TURN);
    if(link==ACT_NONE) return(FALSE);
    act=get_action(this);
    link->whowaitsforme=act;
    act->model3d=this;
    act->type=ACT_WAITFOR;
    act->link=ACT_NONE;
    act->waitforaction=link;	
    return(TRUE);
}

void MODEL3D_CLASS::reset_object_matrix(short o)
{
    this->object[o].m=identity_matrix_f;
    this->object[o].winkel[0]=0;
    this->object[o].winkel[1]=0;
    this->object[o].winkel[2]=0;
    this->object[o].moved=FALSE;
}

void MODEL3D_CLASS::kill_all_actions()
{

    int i;
    for(i=0;i<a_action_anz;i++)
    if(a_action[i]->model3d==this)
    {
       kill_a_action(i);
       i--;
    }
    
    
    for(i=0;i<i_action_anz;i++)
    if(i_action[i]->model3d==this)
    {
       kill_i_action(i);
       i--;
    }
}
    

void proceed_actions()
{
    int i;
    MATRIX_f m;
    bool ready;
    VECTOR pos;
    int k;
    for(i=0;i<a_action_anz; i++)
    {
       switch(a_action[i]->type)
       {
          case ACT_FINISHED:
               kill_a_action(i);
             
                 i--;
             //  a_action[i]->type=ACT_KILL_IT;
          break;     
          case ACT_MOVE:
               a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
              if(a_action[i]->speed==NOW)
               {
                   a_action[i]->model3d->object[a_action[i]->o].m.t[a_action[i]->axis]=a_action[i]->amount;
                  a_action[i]->type=ACT_FINISHED;
               }
               else if(accelerate(a_action[i]->amount, &a_action[i]->model3d->object[a_action[i]->o].m.t[a_action[i]->axis], a_action[i]->speed))
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_TURN:
               a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
               if(a_action[i]->speed==NOW)
               {
                   ready=1;
                   a_action[i]->model3d->object[a_action[i]->o].winkel[a_action[i]->axis]=a_action[i]->amount;
               }
               else ready=accelerate(a_action[i]->amount, &a_action[i]->model3d->object[a_action[i]->o].winkel[a_action[i]->axis], a_action[i]->speed);
               pos=trans_matrix_to_vector(&a_action[i]->model3d->object[a_action[i]->o].m);
               get_rotation_matrix_f(&a_action[i]->model3d->object[a_action[i]->o].m, a_action[i]->model3d->object[a_action[i]->o].winkel[0], a_action[i]->model3d->object[a_action[i]->o].winkel[1], a_action[i]->model3d->object[a_action[i]->o].winkel[2]);
               translate_matrix_v(&a_action[i]->model3d->object[a_action[i]->o].m, &pos);
               if(ready)a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_SPIN:
               a_action[i]->model3d->object[a_action[i]->o].moved=TRUE;
               a_action[i]->model3d->object[a_action[i]->o].winkel[a_action[i]->axis]+=a_action[i]->speed*fmult;
               pos=trans_matrix_to_vector(&a_action[i]->model3d->object[a_action[i]->o].m);
               get_rotation_matrix_f(&a_action[i]->model3d->object[a_action[i]->o].m, a_action[i]->model3d->object[a_action[i]->o].winkel[0], a_action[i]->model3d->object[a_action[i]->o].winkel[1], a_action[i]->model3d->object[a_action[i]->o].winkel[2]);
               translate_matrix_v(&a_action[i]->model3d->object[a_action[i]->o].m, &pos);
          break;
          case ACT_WAIT:
               a_action[i]->t-=fmult*1000;
               if(a_action[i]->t<=0)
               {
                  a_action[i]->type=ACT_FINISHED;
                  if(a_action[i]->link != ACT_NONE) activate_action(a_action[i]->link);
               }   
          break;
          case ACT_WAITFOR:
               if (a_action[i]->waitforaction->type == ACT_FINISHED)
               {
                  a_action[i]->type=ACT_FINISHED;
                  if(a_action[i]->link != ACT_NONE) activate_action(a_action[i]->link);
               }   
          break;
          case ACT_HIDE:
               a_action[i]->model3d->object[a_action[i]->o].visible=FALSE;
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_SHOW:
               a_action[i]->model3d->object[a_action[i]->o].visible=TRUE;
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_HIDE_GRID:
               a_action[i]->model3d->object[a_action[i]->o].grid_is_visible=FALSE;
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_SHOW_GRID:
               a_action[i]->model3d->object[a_action[i]->o].grid_is_visible=TRUE;
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_HIDE_FILL:
               a_action[i]->model3d->object[a_action[i]->o].fill_is_visible=FALSE;
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_SHOW_FILL:
               a_action[i]->model3d->object[a_action[i]->o].fill_is_visible=TRUE;
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_BLINKING:
               a_action[i]->model3d->object[a_action[i]->o].single_colored=TRUE;
               k=chrono;
               k/=50;
               k=k%16;
               a_action[i]->model3d->object[a_action[i]->o].color=k;
          break;
          case ACT_FUNCTION:
               a_action[i]->anim_funct(a_action[i]->model3d);
               a_action[i]->type=ACT_FINISHED;
          break;

          case ACT_INACTIVE:
               printf(" o nein!!!\n");
          break;
       }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////       STEREO       ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
             
/* Hier wird alles Stereoskopische erledigt.
 * - Es werden Berechnungen ausgeführt
 * - Es werden die 3D-Farben initiiert.
 */

const GLfloat light_ambient_l[] = {0.93, 1.0, 1.0, 1.0};
const GLfloat light_diffuse_l[] = {0.1 , 0.0, 0.0, 1.0};
const GLfloat light_ambient_r[] = {1.0 , 1.0, 0.93, 1.0};
const GLfloat light_diffuse_r[] = {0.0 , 0.0, 0.1, 1.0};
bool act_eye;

GLubyte col256[2][256][3];
GLubyte col16[2][24][3] =
{
{{ 0, 40, 40},
{ 11, 40, 40},
{ 21, 40, 40},
{ 32, 40, 40},
{ 43, 40, 39},
{ 53, 40, 39},
{ 64, 39, 39},
{ 75, 38, 39},
{ 85, 37, 38},
{ 96, 36, 37},
{107, 32, 34},
{117, 28, 34},
{128, 24, 33},
{139, 17, 29},
{149, 15, 29},
{160,  5, 20},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0}},

{{ 0,  0,  0},
{  0,  0, 16},
{  0,  0, 32},
{  0,  0, 48},
{  0,  0, 64},
{  0,  0, 80},
{  0,  0, 96},
{  0,  0,112},
{  0,  0,128},
{  0,  0,144},
{  0,  0,160},
{  0,  0,176},
{  0,  0,192},
{  0,  0,208},
{  0,  0,224},
{  0,  0,240},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0},
{  0,  0,  0}}
};


void init_color_table(char file[])
{
   int i;
   int buf[MAX_POINTS];
   ColorConversion colortable; 
   colortable.LoadConf(file);//"color_tables/editor.conf"
   for(i=0; i<255; i++)
   {
      //links
      col256[0][i][0]=colortable.GetValue(SIDE_LEFT, COLOR_RED, i);
      col256[0][i][1]=colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i);
      col256[0][i][2]=colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i);
      //rechts
      col256[1][i][0]=colortable.GetValue(SIDE_RIGHT, COLOR_RED, i);
      col256[1][i][1]=colortable.GetValue(SIDE_RIGHT, COLOR_GREEN, i);
      col256[1][i][2]=colortable.GetValue(SIDE_RIGHT, COLOR_BLUE, i);
   }
   for(i=0; i<16; i++)
   {
      //links
      col16[0][i][0]=colortable.GetValue(SIDE_LEFT, COLOR_RED, i*17);
      col16[0][i][1]=colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i*17);
      col16[0][i][2]=colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i*17);
      //rechts
      col16[1][i][0]=colortable.GetValue(SIDE_RIGHT, COLOR_RED, i*17);
      col16[1][i][1]=colortable.GetValue(SIDE_RIGHT, COLOR_GREEN, i*17);
      col16[1][i][2]=colortable.GetValue(SIDE_RIGHT, COLOR_BLUE, i*17);
   }
   GLubyte r,g,b;
   colortable.GetBackgnd(&r, &g, &b);
   float r2=r/255.0, g2=r/255.0, b2=r/255.0;
   glClearColor(r2, g2, b2, 1.0);
}

void update_color_fx()
{
    int a;
    a=(chrono/50)%16;
    col16[0][16][0]=col16[0][a][0];
    col16[0][16][1]=col16[0][a][1];
    col16[0][16][2]=col16[0][a][2];

    col16[1][16][0]=col16[1][a][0];
    col16[1][16][1]=col16[1][a][1];
    col16[1][16][2]=col16[1][a][2];
////////////////////////////////////
    a=250+rand() % 5;
    col16[0][17][0]=col256[0][a][0];
    col16[0][17][1]=col256[0][a][1];
    col16[0][17][2]=col256[0][a][2];

    col16[1][17][0]=col256[1][a][0];
    col16[1][17][1]=col256[1][a][1];
    col16[1][17][2]=col256[1][a][2];
////////////////////////////////////
    a=(chrono/70)%8;
    col16[0][18][0]=col16[0][8+a][0];
    col16[0][18][1]=col16[0][8+a][1];
    col16[0][18][2]=col16[0][8+a][2];

    col16[1][18][0]=col16[1][4+a/2][0];
    col16[1][18][1]=col16[1][4+a/2][1];
    col16[1][18][2]=col16[1][4+a/2][2];

}

void color16(char lumi)
{
    glColor3ubv(col16[act_eye][lumi]);
}
void color16(char lumi, char alpha)
{
    glColor4ub(col16[act_eye][lumi][0], col16[act_eye][lumi][1], col16[act_eye][lumi][2], alpha);
}
void color256(char lumi)
{
    glColor3ubv(col256[act_eye][lumi]);
}
void color256(char lumi, char alpha)
{
    glColor4ub(col256[act_eye][lumi][0], col256[act_eye][lumi][1], col256[act_eye][lumi][2], alpha);
}


bool get_eye() {return (act_eye);}

void STEREO_CLASS::set_to(bool eye)
{
    float width=half_monitor_width*clipscale;
    float height=half_monitor_height*clipscale;
    act_eye=eye;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(!act_eye)
    {
	    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //zeichne alle Farben
       glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_l);
       glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_l);
       glFrustum((-half_monitor_width+half_eye_seperation)*clipscale*scale,
                  (half_monitor_width+half_eye_seperation)*clipscale*scale,
                 (-half_monitor_height)*clipscale*scale,
                  (half_monitor_height)*clipscale*scale,
                  near_clip*scale,
                  far_clip*scale);
    }
    else
    {
       glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE); //zeichne nur blau!
       glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_r);
       glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_r);
       glFrustum((-half_monitor_width-half_eye_seperation)*clipscale*scale,
                  (half_monitor_width-half_eye_seperation)*clipscale*scale,
                 (-half_monitor_height)*clipscale*scale,
                  (half_monitor_height)*clipscale*scale,
                  near_clip*scale,
                  far_clip*scale);
    }

  	 glMatrixMode(GL_MODELVIEW);
  	 glLoadIdentity();
}

void STEREO_CLASS::position_camera(MATRIX_f *camera_matrix)
{
    if(!act_eye) glTranslatef( half_eye_seperation*scale, 0, 0);
    else         glTranslatef(-half_eye_seperation*scale, 0, 0);
    glMultMatrix_allegro_inv(camera_matrix);
    glPushMatrix();
}

void STEREO_CLASS::set_eye_seperation(float value)
{
    eye_seperation=value;
    half_eye_seperation=value/2;
}

float STEREO_CLASS::get_eye_seperation()
{
    return(eye_seperation);
}

void STEREO_CLASS::set_monitor_width(float value)
{
    monitor_width=value;
    half_monitor_width=value/2;
}

float STEREO_CLASS::get_monitor_width()
{
    return(monitor_width);
}

void STEREO_CLASS::set_monitor_height(float value)
{
    monitor_height=value;
    half_monitor_height=value/2;
}

float STEREO_CLASS::get_monitor_height()
{
    return(monitor_height);
}

void STEREO_CLASS::set_distance_to_monitor(float value)
{
     distance_to_monitor=value;
}

float STEREO_CLASS::get_distance_to_monitor()
{
    return(distance_to_monitor);
}

void STEREO_CLASS::set_far_clip(float value)
{
     far_clip=value;
}

float STEREO_CLASS::get_far_clip()
{
    return(far_clip);
}

void STEREO_CLASS::set_near_clip(float value)
{
    if(value==MONITOR_PLANE)
    {
       near_clip=distance_to_monitor;
       clipscale=1.0;
    }
    else if(value > 0)
    {
       near_clip=value;
       clipscale=near_clip/distance_to_monitor;
    }
}

STEREO_CLASS::STEREO_CLASS()
{
    act_eye=LEFT_EYE;
    monitor_height=0.24;
    half_monitor_height=0.12;
    monitor_width=0.32;
    half_monitor_width=0.16;
    eye_seperation=0.068;
    half_eye_seperation=0.034;
    distance_to_monitor=0.55;
    near_clip=distance_to_monitor;
    far_clip=3000.0;
    clipscale=1.0;
    scale=1.0;
}

void STEREO_CLASS::init_lighting()
{
    GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_position[] = {1, 1, 1, 0};
    GLfloat light_specular[] = {0, 0, 0, 0};
    GLfloat light_ambient[] = {0.9, 1, 1, 1};
    GLfloat light_diffuse[] = {0.1, 0, 0, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}     
////////////////////////////////////////////////////////////////////////////////
////////////////////////////       DRAWING       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void draw_object_color(O3D *o3d)
{
    int i, p, j=0;
    glVertexPointer(3, GL_FLOAT, 0, o3d->vert);
  	 for (p = 0; p < o3d->poly_anz; p++)
	{
	   glNormal3fv(&o3d->n[p].x);
	   glDrawElements(GL_POLYGON, o3d->ind_anz[p], GL_UNSIGNED_SHORT, &o3d->i[j]);
	   j+=o3d->ind_anz[p];
	}
}

void draw_object_alpha(O3D *o3d, char alpha)
{
    int i, p, j=0;
    color16(15, alpha);
    glVertexPointer(3, GL_FLOAT, 0, o3d->vert);
  	for (p = 0; p < o3d->poly_anz; p++)
	{ 
	   glNormal3fv(&o3d->n[p].x);
	   glDrawElements(GL_POLYGON, o3d->ind_anz[p], GL_UNSIGNED_SHORT, &o3d->i[j]);
	   j+=o3d->ind_anz[p];
	} 
}

void draw_object(O3D *o3d)
{
    int i, p, j=0;
    glVertexPointer(3, GL_FLOAT, 0, o3d->vert);
 	for (p = 0; p < o3d->poly_anz; p++)
	{ 
	   glNormal3fv(&o3d->n[p].x);
	   color16(o3d->color[p]);
	   glDrawElements(GL_POLYGON, o3d->ind_anz[p], GL_UNSIGNED_SHORT, &o3d->i[j]);
	   j+=o3d->ind_anz[p];
	} 
}

void MODEL3D_CLASS::draw_objects(short o)
{
    glPushMatrix();
    glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
    if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);
    if(this->object[o].visible && this->object[o].fill_is_visible)
    {
       if(this->object[o].single_colored)
       {
          color16(this->object[o].color);
          draw_object_color(&this->model->o3d[o]);
       }
       else draw_object(&this->model->o3d[o]);
    }
    if(this->model->o3d[o].child_o)
    {
       this->draw_objects(this->model->o3d[o].child_o);
    }
    glPopMatrix();    

    if(this->model->o3d[o].sibl_o)
    {
       this->draw_objects(this->model->o3d[o].sibl_o);
    }
}

void draw_object_lines(O3D *o3d)
{
    glVertexPointer(3, GL_FLOAT, 0, o3d->vert);
    glDrawElements(GL_LINES, o3d->line_anz*2, GL_UNSIGNED_SHORT, o3d->line);
}


void MODEL3D_CLASS::draw_objects_lines(short o)
{
    glPushMatrix();
    glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
    if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);

    if(this->object[o].visible && this->object[o].grid_is_visible)draw_object_lines(&this->model->o3d[o]);
    if(this->model->o3d[o].child_o)
    {
       this->draw_objects_lines(this->model->o3d[o].child_o);
    }
    glPopMatrix();    

    if(this->model->o3d[o].sibl_o)
    {
       this->draw_objects_lines(this->model->o3d[o].sibl_o);
    }
}

void MODEL3D_CLASS::draw_objects_alpha(short o, char alpha)
{
    glPushMatrix();
    glTranslatef(this->model->o3d[o].pos.x, this->model->o3d[o].pos.y, this->model->o3d[o].pos.z);
    if(this->object[o].moved)glMultMatrix_allegro(&this->object[o].m);

    if(this->object[o].visible && this->object[o].grid_is_visible)draw_object_alpha(&this->model->o3d[o], alpha);
    if(this->model->o3d[o].child_o)
    {
       this->draw_objects_alpha(this->model->o3d[o].child_o, alpha);
    }
    glPopMatrix();    

    if(this->model->o3d[o].sibl_o)
    {
       this->draw_objects_alpha(this->model->o3d[o].sibl_o, alpha);
    }
}
