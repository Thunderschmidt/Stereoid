/*
 * Eines der files, die irgendwann scriptbar sein sollen...
 * Hier befinden sich Daten und Skripten der 3D-Objekte
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
//#include "actions.h"
//#include "mat.h"
//#include "drawprim.h"
//#include "stick.h"
//#include "cd.h"


void prepare_raptor(UNIT *u)
{
    int i, j;

    u->udata->engine_obj=u->model3d.get_object_by_name("engemit");
    
    u->mount[0]=TRUE;
    u->mount[2]=TRUE;
    u->mount[1]=FALSE;
    u->mount[3]=FALSE;
    u->missile=AI_SEEKING;
    u->seekmissiles=2;
    u->dumbmissiles=15;
    
    u->model3d.hide("windshield2");
    u->model3d.hide("lever");
    u->model3d.hide("frontstamp1");
    u->model3d.hide("frontstamp2");
    u->model3d.hide("frontskid");
    u->model3d.hide("frontbox");
    u->model3d.hide("backboxes");
    u->model3d.hide("backstamps");
    u->model3d.hide("backskids");
    u->model3d.hide("lbackflap");
    u->model3d.hide("rbackflap");
    u->model3d.hide("frontflap");
    u->model3d.hide("lemit");
    u->model3d.hide("remit");
    u->model3d.hide("camera");
    u->model3d.hide("dumbemit");
    u->model3d.hide("lmount2");
    u->model3d.hide("rmount2");
    u->model3d.hide("cockpit");
    u->model3d.hide("guncalc");
    u->model3d.hide("engemit");
    u->model3d.hide("attach");
    u->model3d.hide("missiles");
    
    
    u->model3d.move("frontstamp2", Y_AXIS, 0.7, NOW);
    u->model3d.move("backskids", Y_AXIS, 0.7, NOW);
    u->model3d.move("backstamps", Y_AXIS, 0.4, NOW); 
    u->model3d.turn("lever", X_AXIS, -22, NOW);
    u->model3d.turn("frontstamp1", X_AXIS, -25, NOW);
    u->model3d.turn("frontskid", X_AXIS, 47, NOW);
    
    u->model3d.hide_grid("lmount1");
    u->model3d.hide_grid("rmount1");
    u->model3d.hide_grid("grate");
    u->model3d.hide_grid("holes");
//    u->model3d.hide_grid("");
//    u->model3d.hide_grid("");
    j=u->model3d.get_object_by_name("miss1");
    for(i=0;i<15;i++)
    {
       u->model3d.hide_grid(j+i);              
    }
}

void raptor_extend_gear(UNIT *u)
{
    u->model3d.show("lever");
    u->model3d.show("frontstamp1");
    u->model3d.show("frontstamp2");
    u->model3d.show("frontskid");
    u->model3d.show("frontbox");
    u->model3d.show("backboxes");
    u->model3d.show("backstamps");
    u->model3d.show("backskids");
    u->model3d.show("lbackflap");
    u->model3d.show("rbackflap");
    u->model3d.show("frontflap");
    u->model3d.hide("frontflapdummy");
    u->model3d.hide("rearflapsdummy");
    u->model3d.turn("frontflap", Z_AXIS, 98, 60);
    u->model3d.turn("lbackflap", Z_AXIS, 98, 50);
    u->model3d.turn("rbackflap", Z_AXIS, -98, 50);
    u->model3d.wait_for_turn("lbackflap", Z_AXIS);
    u->model3d.wait(700);
    u->model3d.turn("lever", X_AXIS, 0, 20);
    u->model3d.turn("frontskid", X_AXIS, 25, 20);
    u->model3d.move("backstamps", Y_AXIS, 0, 0.3);
    u->model3d.wait_for_move("backstamps", Y_AXIS);
    u->model3d.turn("frontstamp1", X_AXIS, 0, 20);
    u->model3d.move("backskids", Y_AXIS, 0, 0.56);
    u->model3d.move("frontstamp2", Y_AXIS, 0, 0.56);
    u->model3d.turn("frontskid", X_AXIS, 0, 20);
}

void raptor_retract_gear(UNIT *u)
{
    u->model3d.move("frontstamp2", Y_AXIS, 0.7, 0.5);
    u->model3d.move("backskids", Y_AXIS, 0.7, 0.5);
    u->model3d.turn("frontstamp1", X_AXIS, -25, 20);
    u->model3d.turn("frontskid", X_AXIS, 25, 20);

    u->model3d.wait_for_move("backskids", Y_AXIS);
    u->model3d.move("backstamps", Y_AXIS, 0.4, 0.3);
    u->model3d.turn("lever", X_AXIS, -22, 20);
    u->model3d.turn("frontskid", X_AXIS, 47, 20);

 
    u->model3d.wait(700);
    u->model3d.turn("frontflap", Z_AXIS, 0, 60);
    u->model3d.turn("lbackflap", Z_AXIS, 0, 50);
    u->model3d.turn("rbackflap", Z_AXIS, 0, 50);
    u->model3d.wait_for_turn("lbackflap", Z_AXIS);

    u->model3d.hide("lever");
    u->model3d.hide("frontstamp1");
    u->model3d.hide("frontstamp2");
    u->model3d.hide("frontskid");
    u->model3d.hide("frontbox");
    u->model3d.hide("backboxes");
    u->model3d.hide("backstamps");
    u->model3d.hide("backskids");
    u->model3d.hide("lbackflap");
    u->model3d.hide("rbackflap");
    u->model3d.hide("frontflap");

    u->model3d.show("frontflapdummy");
    u->model3d.show("rearflapsdummy");
}

void raptor_fire_left_gun(UNIT *u)
{
    u->model3d.move("lgun", Z_AXIS, 0.34, 60);
    u->model3d.wait(150);
    u->model3d.move("lgun", Z_AXIS, 0, 1);
}

void raptor_fire_right_gun(UNIT *u)
{
    u->model3d.move("rgun", Z_AXIS, 0.34, 60);
    u->model3d.wait(150);
    u->model3d.move("rgun", Z_AXIS, 0, 1);
}
void raptor_fire_guns(UNIT *u)
{
    u->model3d.move("lgun", Z_AXIS, 0.34, 60);
    u->model3d.move("rgun", Z_AXIS, 0.34, 60);
    u->model3d.wait(150); 
    u->model3d.move("lgun", Z_AXIS, 0, 1);
    u->model3d.move("rgun", Z_AXIS, 0, 1);
}
void raptor_open_cabin(UNIT *u)
{
    u->model3d.show("cockpit");
    u->model3d.show("windshield2");
    u->model3d.turn("windshield", X_AXIS, 60, 30);
    u->model3d.wait_for_turn("windshield", X_AXIS);
}
void raptor_close_cabin(UNIT *u)
{
    u->model3d.turn("windshield", X_AXIS, 0, 30);
    u->model3d.wait_for_turn("windshield", X_AXIS);
    u->model3d.hide("cockpit");
    u->model3d.hide("windshield2");

}

void the_raptor(UDATA *ud)
{
//	lese_3do(&ud->col_model,  "3do/raptor_col.3do");
//	init_collision_model(&ud->model, &ud->col_model.o3d[0]);

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
           i=u->model3d.get_object_by_name("miss1")+u->dumbmissiles-1;       
           u->model3d.hide(i);
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
          j=u->model3d.get_object_by_name(u->udata->mount_obj[i]);       
          u->model3d.hide(j);
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
    u->model3d.move("door2", Z_AXIS, 12.0, 4.0);
    u->model3d.move("door1", Z_AXIS, -12.0, 4.0);
    u->model3d.wait_for_move("door1", Z_AXIS);
    u->model3d.move("door2", Z_AXIS, 0.0, 4.0);
    u->model3d.move("door1", Z_AXIS, 0.0, 4.0);
    u->model3d.wait_for_move("door1", Z_AXIS);
//    u->model3d.start_function(animate_door);
}
void animate_light(void *pointr)//MODEL3D_CLASS *model3d
{
    MODEL3D_CLASS  *model3d=(MODEL3D_CLASS*)pointr;
    model3d->show("light1");
    model3d->wait(300);
    model3d->hide("light1");
    model3d->show("light2");
    model3d->wait(300);
    model3d->hide("light2");
    model3d->show("light3");
    model3d->wait(300);
    model3d->hide("light3");
    model3d->start_function(animate_light);
}

void the_seeker_missile(UDATA *ud)
{
	ud->model="3do/nrakete.3do";
	ud->col_model, "3do/missile_col.3do";
	//init_collision_model(&ud->model,&ud->col_model.o3d[0]);

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
    ud->col_model="3do/missile_col.3do";              //Name des Kollisions-3do
    ud->model="3do/dumbfire2.3do";                  //Name des 3do
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


