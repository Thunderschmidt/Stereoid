/*
 * Eines der files, die irgendwann scriptbar sein sollen...
 *
 */
//#include <aldumb.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
//#include <GL/glext.h>
//#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "input3DO.h"

//#include "y1.h"
#include "actions.h"
//#include "mat.h"
//#include "drawprim.h"
//#include "stick.h"
//#include "cd.h"


void prepare_raptor(UNIT *u)
{
    int i, j;
    prepare_object_matrices(u);

    u->udata->engine_obj=get_object_by_name(&u->udata->model, "engemit");
    
    u->mount[0]=TRUE;
    u->mount[2]=TRUE;
    u->mount[1]=FALSE;
    u->mount[3]=FALSE;
    u->missile=AI_SEEKING;
    u->seekmissiles=2;
    u->dumbmissiles=15;
    
    hide(u, "windshield2");
    hide(u, "lever");
    hide(u, "frontstamp1");
    hide(u, "frontstamp2");
    hide(u, "frontskid");
    hide(u, "frontbox");
    hide(u, "backboxes");
    hide(u, "backstamps");
    hide(u, "backskids");
    hide(u, "lbackflap");
    hide(u, "rbackflap");
    hide(u, "frontflap");
    hide(u, "lemit");
    hide(u, "remit");
    hide(u, "camera");
    hide(u, "dumbemit");
    hide(u, "lmount2");
    hide(u, "rmount2");
    hide(u, "cockpit");
    hide(u, "guncalc");
    hide(u, "engemit");
    hide(u, "attach");
    hide(u, "missiles");
    
    
    move(u, "frontstamp2", Y_AXIS, 0.7, NOW);
    move(u, "backskids", Y_AXIS, 0.7, NOW);
    move(u, "backstamps", Y_AXIS, 0.4, NOW); 
    turn(u, "lever", X_AXIS, -22, NOW);
    turn(u, "frontstamp1", X_AXIS, -25, NOW);
    turn(u, "frontskid", X_AXIS, 47, NOW);
    
    u->object[get_object_by_name(&u->udata->model, "lmount1")].grid_is_visible=FALSE;
    u->object[get_object_by_name(&u->udata->model, "rmount1")].grid_is_visible=FALSE;
    u->object[get_object_by_name(&u->udata->model, "grate")].grid_is_visible=FALSE;
    u->object[get_object_by_name(&u->udata->model, "holes")].grid_is_visible=FALSE;
//    u->object[get_object_by_name(&u->udata->model, "backboxes")].grid_is_visible=FALSE;
//    u->object[get_object_by_name(&u->udata->model, "frontbox")].grid_is_visible=FALSE;
//    u->object[get_object_by_name(&u->udata->model, "")].grid_is_visible=FALSE;
//    u->object[get_object_by_name(&u->udata->model, "missiles")].grid_is_visible=FALSE;
    j=get_object_by_name(&u->udata->model, "miss1");
    for(i=0;i<15;i++)
    {
       u->object[j+i].grid_is_visible=FALSE;
    }
}

void raptor_extend_gear(UNIT *u)
{
    show(u, "lever");
    show(u, "frontstamp1");
    show(u, "frontstamp2");
    show(u, "frontskid");
    show(u, "frontbox");
    show(u, "backboxes");
    show(u, "backstamps");
    show(u, "backskids");
    show(u, "lbackflap");
    show(u, "rbackflap");
    show(u, "frontflap");
    hide(u, "frontflapdummy");
    hide(u, "rearflapsdummy");
    turn(u, "frontflap", Z_AXIS, -98, 60);
    turn(u, "lbackflap", Z_AXIS, -98, 50);
    turn(u, "rbackflap", Z_AXIS, 98, 50);
//    wait_for_turn(u, "lbackflap", Z_AXIS);
    wait(u, 700);
    turn(u, "lever", X_AXIS, 0, 20);
    turn(u, "frontskid", X_AXIS, 25, 20);
    move(u, "backstamps", Y_AXIS, 0, 0.3);
    wait_for_move(u, "backstamps", Y_AXIS);
    turn(u, "frontstamp1", X_AXIS, 0, 20);
    move(u, "backskids", Y_AXIS, 0, 0.56);
    move(u, "frontstamp2", Y_AXIS, 0, 0.56);
   turn(u, "frontskid", X_AXIS, 0, 20);
 
}

void raptor_retract_gear(UNIT *u)
{
    move(u, "frontstamp2", Y_AXIS, 0.7, 0.5);
    move(u, "backskids", Y_AXIS, 0.7, 0.5);
    turn(u, "frontstamp1", X_AXIS, -25, 20);
    turn(u, "frontskid", X_AXIS, 25, 20);

    wait_for_move(u, "backskids", Y_AXIS);
    move(u, "backstamps", Y_AXIS, 0.4, 0.3);
    turn(u, "lever", X_AXIS, -22, 20);
    turn(u, "frontskid", X_AXIS, 47, 20);

 
    wait(u, 700);
    turn(u, "frontflap", Z_AXIS, 0, 60);
    turn(u, "lbackflap", Z_AXIS, 0, 50);
    turn(u, "rbackflap", Z_AXIS, 0, 50);
    wait_for_turn(u, "lbackflap", Z_AXIS);

    hide(u, "lever");
    hide(u, "frontstamp1");
    hide(u, "frontstamp2");
    hide(u, "frontskid");
    hide(u, "frontbox");
    hide(u, "backboxes");
    hide(u, "backstamps");
    hide(u, "backskids");
    hide(u, "lbackflap");
    hide(u, "rbackflap");
    hide(u, "frontflap");
    show(u, "frontflapdummy");
    show(u, "rearflapsdummy");
}

void raptor_fire_left_gun(UNIT *u)
{
    move(u, "lgun", Z_AXIS, 0.3, 15);
    wait(u, 150);
    move(u, "lgun", Z_AXIS, 0, 1);
}

void raptor_fire_right_gun(UNIT *u)
{
    move(u, "rgun", Z_AXIS, 0.3, 15);
    wait(u, 150);
    move(u, "rgun", Z_AXIS, 0, 1);
}
void raptor_fire_guns(UNIT *u)
{
    move(u, "lgun", Z_AXIS, 0.3, 15);
    move(u, "rgun", Z_AXIS, 0.3, 15);
    wait(u, 150); 
    move(u, "lgun", Z_AXIS, 0, 1);
    move(u, "rgun", Z_AXIS, 0, 1);
}
void raptor_open_cabin(UNIT *u)
{
    show(u, "cockpit");
    show(u, "windshield2");
    turn(u, "windshield", X_AXIS, 60, 30);
    wait_for_turn(u, "windshield", X_AXIS);
}
void raptor_close_cabin(UNIT *u)
{
    turn(u, "windshield", X_AXIS, 0, 30);
    wait_for_turn(u, "windshield", X_AXIS);
    hide(u, "cockpit");
    hide(u, "windshield2");

}

void the_raptor(UDATA *ud)
{
	lese_3do(&ud->model,  "3do/raptor.3do");
	lese_3do(&ud->col_model,  "3do/raptor_col.3do");
	init_collision_model(&ud->model, &ud->col_model.o3d[0]);

	ud->type                 = UNIT_FIGHTER;
    ud->label                = "AV-01 SC";
    ud->maxspeed             =   70;
    ud->acceleration         =   30;
    ud->rotacceleration      =   80;
    ud->hullpower            = 1250;
    ud->shieldpower          =  800;
    ud->mass                 =    6;
    ud->maxrotationspeed     =   60;
    ud->shootrate            =  150;
    ud->shottype             = SHOT_LASER;
    ud->gunpower             =  160;
    ud->gun_erergy_drain     =    7;
    ud->gun_x_offset         =    2.575;
    ud->gun_max_angel        =    1.1;
    ud->mount_obj[0]         = "lmount1";
    ud->mount_obj[1]         = "lmount2";
    ud->mount_obj[2]         = "rmount1";
    ud->mount_obj[3]         = "rmount2";
    ud->max_x_speed          =   6.0;
    ud->max_y_speed          =   6.0;
    ud->afterburner_speed    = 120.0;
}

char raptor_fire_missile(UNIT *u, char type)
{
    char i, j;
    if(type==AI_DUMBFIRE)
    {
        if(u->dumbmissiles>0)
        {
           i=get_object_by_name(&u->udata->model, "miss1")+u->dumbmissiles-1;       
           hide(u, i);
           u->dumbmissiles--;
           return(i);
        }
        return(FALSE);
    }
    else if(type==AI_SEEKING)
    {
       for(i=0;i<4;i++)
       if(u->mount[i])
       {
          j=get_object_by_name(&u->udata->model, u->udata->mount_obj[i]);       
          hide(u, j);
          u->mount[i]=FALSE;
          u->seekmissiles--;
          
          return(j);
       }             
       return(FALSE);
    }
}

void raptor_frame(UNIT *u)
{
}












void animate_door(UNIT *u)
{
    move(u, "door2", Z_AXIS, 12.0, 4.0);
    move(u, "door1", Z_AXIS, -12.0, 4.0);
    wait_for_move(u, "door1", Z_AXIS);
    move(u, "door2", Z_AXIS, 0.0, 4.0);
    move(u, "door1", Z_AXIS, 0.0, 4.0);
    wait_for_move(u, "door1", Z_AXIS);
    start_function(u, animate_door);
}
void animate_light(UNIT *u)
{
    show(u, "light1");
    wait(u, 300);
    hide(u, "light1");
    show(u, "light2");
    wait(u, 300);
    hide(u, "light2");
    show(u, "light3");
    wait(u, 300);
    hide(u, "light3");
    start_function(u, animate_light);
}

void the_seeker_missile(UDATA *ud)
{
	lese_3do(&ud->model,  "3do/nrakete.3do");
	lese_3do(&ud->col_model, "3do/missile_col.3do");
	init_collision_model(&ud->model,&ud->col_model.o3d[0]);

	ud->type                 = UNIT_MISSILE;
    ud->label                = "MISS-01-SKR";
    ud->maxspeed             =   130;
    ud->acceleration         =   195;
    ud->rotacceleration      =   360;
    ud->hullpower            =    25;
    ud->shieldpower          =     0;
    ud->mass                 =     0.15;
    ud->maxrotationspeed     =    360;
    ud->shootrate            =     0;
    ud->shottype             =     0;
}

void the_dumb_missile(UDATA *ud)
{
	lese_3do(&ud->model, "3do/dumbfire2.3do");
	lese_3do(&ud->col_model, "3do/missile_col.3do");
	init_collision_model(&ud->model,&ud->col_model.o3d[0]);

	ud->type                 = UNIT_MISSILE;
    ud->label                = "MISS-001-DMB";
    ud->maxspeed             =   250;
    ud->acceleration         =    60;
    ud->rotacceleration      =   100;
    ud->hullpower            =    15;
    ud->shieldpower          =     0;
    ud->mass                 =     0.05;
    ud->maxrotationspeed     =    20;
    ud->shootrate            =     0;
    ud->shottype             =     0;
}


