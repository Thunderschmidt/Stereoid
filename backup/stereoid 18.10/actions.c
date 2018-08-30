/* actions.c
 * Dies ist die Bibliothek, die die ACTION - structs bearbeitet und
 * für die Animation der Unterobjekte der units zuständig ist.
 */


//#include <allegro.h>
//#include "input3do.h"


#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
//#include <GL/glext.h>
#include <GL/glu.h>
#include "global.h"
#include "actions.h"
#include "mathematics.h"

volatile int *Chrono;
float        *Fmult;


typedef struct ACTION
{
    short index;      //index
    bool active;      
    signed char status;  //ACT_ACTIVE, ACT_INACTIVE, ACT_FINISHED
    signed char type;    //ACT_TURN, ACT_MOVE, ACT_WAIT, ACT_WAITFORACT_TURN, ACT_WAITFORACT_MOVE
    UNIT *unit;
    short o;
    char axis;
    int t;
    float amount;
    float speed;
    
    ACTION *link;   //nächste Aktion
    ACTION *waitforaction; //für Aktionstyp ACT_WAITFOR: warte auf Beendigung dieser Aktion in dieser Verknüpfung
    ACTION *whowaitsforme; //gibt es eine Funktion, die auf mich wartet? Wenn ja, welche?
} ACTION; 


/*ActiveActions: Es können mehrere Bewegungsaktionen hier drin sein,
 *jedoch nur eine ACT_WAIT-Aktion. Diese verweist auf eine InactiveAction
 */

ACTION act_none;
ACTION *ACT_NONE=&act_none;

ACTION a_actions[MAXACTIONS];
ACTION *a_action[MAXACTIONS];
ACTION i_actions[MAXACTIONS];
ACTION *i_action[MAXACTIONS];

volatile int a_action_anz;
volatile int i_action_anz;

void init_actions(volatile int *chrono1, float *fmult1)
{
    int i;
    Chrono=chrono1;
    Fmult=fmult1;
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


void clear_a_action(UNIT *unit, short o, char axis, signed char type)
{
    int i;
    for(i=0;i<a_action_anz;i++)
    if((a_action[i]->unit==unit) &&
       (a_action[i]->o==o) &&
       (a_action[i]->axis==axis) &&
       (a_action[i]->type==type))
    kill_a_action(i);
}

ACTION *get_action(UNIT *unit)
{
    int i, j;
    ACTION *link;
    for(i=0; i<a_action_anz; i++)
    if(((a_action[i]->type == ACT_WAIT) || (a_action[i]->type == ACT_WAITFOR)) && (a_action[i]->unit == unit))  //es gibt immer nur höchstens eine AKTIVE WAIT(_FOR)-Action. Ist eine aktiv?
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
   clear_a_action(act->unit, act->o, act->axis, act->type);
 
   *a_action[a_action_anz]=*act;
   if(act->whowaitsforme!=ACT_NONE)act->whowaitsforme->waitforaction=a_action[a_action_anz];
   a_action_anz++;
   
   if(act->link != ACT_NONE)
   if((act->type != ACT_WAIT) && (act->type != ACT_WAITFOR))
   activate_action(act->link);
   kill_i_action(act->index);
}


void stop_move (UNIT *unit, short o, short axis)
{
}

void stop_turn (UNIT *unit, short o, short axis)
{
}

void stop_spin (UNIT *unit, short o, short axis)
{
}

void stop_a_action(ACTION *act)
{
    if(act->active)
    {
       a_action_anz--;
       clear_a_action(act->unit, act->o, act->axis, act->type);
       a_action_anz++;
    }
}


void move(UNIT *unit, short o, short axis, float amount, float speed)
{
    ACTION *act=get_action(unit);
    act->unit=unit;
    act->o=o;
    act->axis=axis;
    act->amount=amount;
    act->speed=speed;
    act->type=ACT_MOVE;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
    stop_a_action(act);
}

void turn(UNIT *unit, short o, short axis, float amount, float speed)
{
    ACTION *act=get_action(unit);
    act->unit=unit;
    act->o=o;
    act->axis=axis;
    act->amount=amount;
    act->speed=speed;
    act->type=ACT_TURN;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
    stop_a_action(act);
}

void spin(UNIT *unit, short o, char axis, float speed)
{
    ACTION *act=get_action(unit);
    act->unit=unit;
    act->o=o;
    act->axis=axis;
    act->speed=speed;
    act->type=ACT_SPIN;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
    stop_a_action(act);
}

void wait(UNIT *unit, int t)
{
    ACTION *act=get_action(unit);
    act->unit=unit;
    act->t=t;
    act->type=ACT_WAIT;
    act->link=ACT_NONE;
    act->waitforaction=ACT_NONE;
    act->whowaitsforme=ACT_NONE;
}

ACTION *find_action(UNIT *unit, short o, char axis, signed char type)
{
    int i;
    ACTION *ret, *link;
    for(i=0; i<i_action_anz; i++)
    {
       if ((i_action[i]->unit==unit) && (i_action[i]->o==o) && (i_action[i]->axis==axis) && (i_action[i]->type==type))
       {
          ret=i_action[i];
          link=ret->link;
          while(link!=ACT_NONE)
          {
             if ((link->unit==unit) && (link->o==o) && (link->axis==axis)&& (link->type==type))
                ret=link;
             link=link->link;
          }
          return(ret);
       }
    }
    for(i=0; i<a_action_anz; i++)
    if ((a_action[i]->unit==unit) && (a_action[i]->o==o) && (a_action[i]->axis==axis)&& (a_action[i]->type==type))
    return(a_action[i]);
    return(ACT_NONE);
}

bool wait_for_move(UNIT *unit, short o, char axis)
{
    ACTION *act, 
           *link=find_action(unit, o, axis, ACT_MOVE);
    if(link==ACT_NONE) return(FALSE);

    act=get_action(unit);
    link->whowaitsforme=act;
    act->unit=unit;
    act->type=ACT_WAITFOR;
    act->link=ACT_NONE;
    act->waitforaction=link;	
    return(TRUE);
}

bool wait_for_turn(UNIT *unit, short o, char axis)
{
    ACTION *act, 
           *link=find_action(unit, o, axis, ACT_TURN);
    if(link==ACT_NONE) return(FALSE);

    act=get_action(unit);
    link->whowaitsforme=act;
    act->unit=unit;
    act->type=ACT_WAITFOR;
    act->link=ACT_NONE;
    act->waitforaction=link;	
    return(TRUE);
}

void reset_object_matrix(UNIT *unit, short o)
{
    unit->object[o].m=identity_matrix_f;
    unit->object[o].winkel[0]=0;
    unit->object[o].winkel[1]=0;
    unit->object[o].winkel[2]=0;
    unit->object[o].moved=FALSE;
}

void kill_unit_actions(UNIT *unit)
{
    int i;
    for(i=0;i<a_action_anz;i++)
    if(a_action[i]->unit==unit)
    {
       kill_a_action(i);
       i--;
    }
    
    
    for(i=0;i<i_action_anz;i++)
    if(i_action[i]->unit==unit)
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
               a_action[i]->unit->object[a_action[i]->o].moved=TRUE;
               if(accelerate(a_action[i]->amount, &a_action[i]->unit->object[a_action[i]->o].m.t[a_action[i]->axis], a_action[i]->speed))
               a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_TURN:
               a_action[i]->unit->object[a_action[i]->o].moved=TRUE;
               ready=accelerate(a_action[i]->amount, &a_action[i]->unit->object[a_action[i]->o].winkel[a_action[i]->axis], a_action[i]->speed);
               pos=trans_matrix_to_vector(&a_action[i]->unit->object[a_action[i]->o].m);
               get_rotation_matrix_f(&a_action[i]->unit->object[a_action[i]->o].m, a_action[i]->unit->object[a_action[i]->o].winkel[0], a_action[i]->unit->object[a_action[i]->o].winkel[1], a_action[i]->unit->object[a_action[i]->o].winkel[2]);
               translate_matrix_v(&a_action[i]->unit->object[a_action[i]->o].m, &pos);
               if(ready)a_action[i]->type=ACT_FINISHED;
          break;
          case ACT_SPIN:
               a_action[i]->unit->object[a_action[i]->o].moved=TRUE;
               a_action[i]->unit->object[a_action[i]->o].winkel[a_action[i]->axis]+=a_action[i]->speed**Fmult;
               pos=trans_matrix_to_vector(&a_action[i]->unit->object[a_action[i]->o].m);
               get_rotation_matrix_f(&a_action[i]->unit->object[a_action[i]->o].m, a_action[i]->unit->object[a_action[i]->o].winkel[0], a_action[i]->unit->object[a_action[i]->o].winkel[1], a_action[i]->unit->object[a_action[i]->o].winkel[2]);
               translate_matrix_v(&a_action[i]->unit->object[a_action[i]->o].m, &pos);
          break;
          case ACT_WAIT:
               a_action[i]->t-=*Fmult*1000;
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
          case ACT_INACTIVE:
               printf(" o nein!!!\n");
          break;
       }
    }
}
             
     


