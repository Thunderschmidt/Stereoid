#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "input3DO.h"
#include "actions.h"
#include "mat.h"
#include "drawprim.h"
#include "stick.h"
#include "cd.h"
#include "ships.h"
#include "snd.h"
#include "fx.h"
#include "ster.h"
#include "highw.h"


float        fps_rate = 60.0,
             sinus10[36][2];					//Daten für Zielmonitor
int          frame_count = 0,
             frame_count_time = 0;
bool         buttn_pressed=0;
int          buttn_delay=0, input_delay;
volatile int chrono = 0;
float        fcount, fmult;
int unit_anz=0;


BITMAP *logo[4];

UNIT no_target;
UNIT *NO_TARGET=&no_target;
UNIT units[MAXUNITS];
UNIT *unit[MAXUNITS];
PLAYER player;
UDATA raptor;
UDATA seeker_missile;
UDATA dumb_missile;
UDATA asteroid00;
UDATA asteroid01;
UDATA asteroid02;
UDATA asteroid03;
UDATA asteroid04;
UDATA asteroid05;
UDATA speicher;
UDATA level;
UDATA flea;
STICK stick;

DEBRIS_CLASS debris;
EXPLOSION1_CLASS explosion1;
EXPLOSION2_CLASS explosion2;
BACKGROUND_CLASS background;
FLUSH_CLASS flush;
SHOT_CLASS shot;
SOUND_CLASS sound("sounds/", &player.zero_m, &player.camera.pos, 512.0);
STEREO_CLASS stereo;
MODEL cockp;
////////////////////////////////////////////////////////




void init_pointers()
{
   short i;
   for(i=0; i<MAXUNITS; i++)
   {
       unit[i]=&units[i];
       units[i].index=i;
   }
}
int engsound;
void load_samples()
{
    sound.load("shield_hit_01"); 
    sound.load("metal_hit"); 
    sound.load("laser_02"); 
    sound.load("fighter_explodes"); 
    sound.load("button_02"); 
    sound.load("button_01"); 
    sound.load("missile_starts_01"); 
    sound.load("missile_starts_02"); 
    sound.load("missile_explodes_01"); 
    engsound=sound.load("thrust");
    sound.load("shot_hit");
}

void load_bitmaps (void)
{
	logo[0]=create_video_bitmap(35, 30);
	logo[0] = load_bitmap ("bmp/logo1.bmp", NULL);
	logo[1]=create_video_bitmap(35, 30);
	logo[1] = load_bitmap ("bmp/logo2.bmp", NULL);
	logo[2]=create_video_bitmap(35, 30);
	logo[2] = load_bitmap ("bmp/logo3.bmp", NULL);
}

void destroy_bitmaps()
{
			destroy_bitmap(logo[0]);
			destroy_bitmap(logo[1]);
			destroy_bitmap(logo[2]);
}


void the_timer(void)
{
	chrono++;
} END_OF_FUNCTION(the_timer);

void init_sinus10()
{
	char a;
	for(a=0; a<36; a++)
	{
		sinus10[a][0]=sin( DEG_2_RAD(a*10));
		sinus10[a][1]=cos(  DEG_2_RAD(a*10));
	}
}

void calc_distances()
{
    int i;
    for(i=0; i<unit_anz; i++) unit[i]->dist=distance(&player.camera.pos, (VECTOR*)&unit[i]->m.t[0]);         
}

void render_selection()
{
    int i, j;
    for(i=0; i<unit_anz; i++)
    {
       if(unit[i]->udata->type==UNIT_LEVEL)
       if(unit[i]->udata->model.radius>unit[i]->dist)
       {
          unit[i]->rendered=RND_NORMAL;
          unit[i]->outlinecolor=14;
          unit[i]->presence=1.0;
          i++; if(i==unit_anz) break;
       }
       unit[i]->presence=unit[i]->udata->model.size/unit[i]->dist;
       if(unit[i]->presence>1.75)unit[i]->presence=1.75;
       if(spot_is_visible(&unit[i]->heading, unit[i]->presence))
       { 
          if(unit[i]->presence > 0.015)
          {
                unit[i]->rendered=RND_NORMAL;
                /*if(unit[i]->presence > 1)unit[i]->outlinecolor=15;
                else */if(unit[i]->presence > 0.4)unit[i]->outlinecolor=15;
                else if(unit[i]->presence > 0.2)unit[i]->outlinecolor=14;
                else if(unit[i]->presence > 0.1)unit[i]->outlinecolor=13;
                else if(unit[i]->presence > 0.05)unit[i]->outlinecolor=12;
                else if(unit[i]->presence > 0.025)unit[i]->outlinecolor=11;
                else if(unit[i]->presence > 0.0175)unit[i]->outlinecolor=10;
                else unit[i]->outlinecolor=9;
          }
          else if(unit[i]->presence > 0.0058)unit[i]->rendered=RND_NO_OUTLINES;
          else if(unit[i]->presence > 0.0012)
          {
             unit[i]->rendered=RND_ONLY_OUTLINES;
             unit[i]->outlinecolor=8;
          }  
          else if(unit[i]->presence > 0.0008)unit[i]->rendered=RND_BIG_POINT;
          else if(unit[i]->presence > 0.0004)unit[i]->rendered=RND_SMALL_POINT;
          else unit[i]->rendered=RND_NOT_RENDERED;
       }
       else unit[i]->rendered=RND_NOT_RENDERED;
    }
    for(i=0; i<shot.total; i++)
    {
       if (spot_is_visible(&shot.shot[i]->heading, 0))
       {
          shot.shot[i]->rendered=RND_NORMAL;
       }
       else shot.shot[i]->rendered=RND_NOT_RENDERED;
    }
}

void get_headings()
{
    int i;
    for (i=1; i<unit_anz; i++)
    {
       get_heading(&player.unit->m, &unit[i]->m, &unit[i]->heading);
    }
    for (i=0; i<shot.total; i++)
    {
       get_heading(&player.unit->m, &shot.shot[i]->m, &shot.shot[i]->heading);
    }
}

void kill_unit(short u)
{
    UNIT *temp;
    int i;
    free(unit[u]->object);               //befreie den Speicher für die Unterobjeckt-Matrizen
    kill_unit_actions(unit[u]);
    for(i=0;i<unit_anz;i++)if(unit[i]->target==unit[u])unit[i]->target=NO_TARGET;
    temp=unit[u];
    unit[u]=unit[unit_anz-1];
    unit[unit_anz-1]=temp;
    unit[unit_anz-1]->index=unit_anz-1;
    unit[u]->index=u;                //Indizes werden beibehalten!!
    unit_anz--;
    
}


void damage_unit(UNIT *u, float damage)
{

   u->justhit=chrono;
   if(u->vulnerable)
   {
   u->shield-=damage;
   if(u->shield<0)
   {
      u->hull+=u->shield;
      u->shield=0;
   }
   if (u->hull<1)
   {
      explosion2.start(&u->m, &u->speed, u->property);
      if (u != 0)sound.play("fighter_explodes", u->m.t);
      kill_unit(u->index);
   }
   }
}


void align_target(UNIT *u, float value)
{
    VECTOR heading;
    get_heading(&u->m, &u->target->m, &heading);
    u->wink_soll=vect(0,0,0);
    if (heading.x>0.01)
    { 
       if ((heading.z>0) && (heading.x<0.1)) u->wink_soll.y=4;
       else {u->wink_soll.y=value;u->wink_soll.z=5;}
    }
    else if (heading.x<-0.01)
    {
       if ((heading.z>0) && (heading.x>-0.1)) u->wink_soll.y=-4;
       else {u->wink_soll.y=-value;u->wink_soll.z=-5;}

    }
    if (heading.y>0.01)
    { 
       if ((heading.z>0) && (heading.y<0.1)) u->wink_soll.x=-4;
       else u->wink_soll.x=-value;
    }
    else if (heading.y<-0.01)
    {
       if ((heading.z>0) && (heading.y>-0.1)) u->wink_soll.x=4;
       else u->wink_soll.x=value;
    }
}


void fire_at_target(UNIT *u)
{
    VECTOR heading;
    MATRIX_f m; VECTOR emit;

    get_heading(&u->m, &u->target->m, &heading);
    if((heading.x<0.15) && (heading.x>-0.15)&& (heading.y<0.15)&& (heading.y>-0.15)&& (heading.z>0))
    {   
       if ( chrono-u->shotdelay > u->udata->shootrate)
       {
          if(u->gun)
          {
             shot.start(u, "lemit", SHOT_LASER, -1);
             raptor_fire_left_gun(u);
          }
          else
          {
             shot.start(u, "remit", SHOT_LASER, 1);
             raptor_fire_right_gun(u);
          }
               
          u->shotdelay=chrono;
          u->gun=1-u->gun;
          sound.play("laser_02", u->m.t);
       }
    }
}



void get_next_target(UNIT *u)
{
    if(u->target==NO_TARGET)u->target=unit[0];
    else 
    {
       if(u->target->index < (unit_anz-1)) u->target=unit[u->target->index+1];
       else u->target=unit[0];
    }
    if((u->target == u) && (unit_anz>1)) get_next_target(u);
}

void get_last_target(UNIT *u)
{
    if(u->target->index >0) u->target=unit[u->target->index-1];
    else u->target=unit[unit_anz-1];
    if(u->target == u && (unit_anz>1)) get_last_target(u);
}

UNIT *get_nearest_enemy_target(UNIT *u)
{
    short i, ret=NONE; float dist0, dist1=0;
    for (i=0; i<unit_anz; i++)
    if ((u!=unit[i]) && (unit[i]->party!=u->party) && unit[i]->party!=0)
    {
        dist0=1/distance((VECTOR*)&u->m.t[0], (VECTOR*)&unit[i]->m.t[0]);
        if(dist0>dist1)
        {
           ret=i;
           dist1=dist0;
        }
    }
    if(ret==NONE)return(NO_TARGET);
    else return(unit[ret]);
}

UNIT *target_unit_near_reticule()
{
    int i, ret=0;
    VECTOR heading; float minheading=2;
    for (i=0; i<unit_anz; i++)
    {
       heading=unit[i]->heading;
       if (heading.z>0)
       {
          if (heading.x<0)heading.x*=-1;
          if (heading.y<0)heading.y*=-1;
          if ((heading.x+heading.y)<minheading)
          {
             minheading=heading.x+heading.y;
             ret=i;
          }
       }
    }
    return(unit[ret]);
}   

void check_explosion_radius(VECTOR *exp_point, float radius, UNIT *u1)
{
    int u;
    MATRIX_f m;
    VECTOR pushdir;
    float dist;

    translate_matrix_v(&m, exp_point);
    for(u=0;u<unit_anz;u++)
    if(unit[u]!=u1)
    if (check_col(&m, &unit[u]->m, radius))
    {
       pushdir.x=exp_point->x-unit[u]->m.t[0];
       pushdir.y=exp_point->y-unit[u]->m.t[1];
       pushdir.z=exp_point->z-unit[u]->m.t[2];
       dist=vector_length_f(pushdir.x, pushdir.y, pushdir.z);
       if(dist<=radius)
       {
          if(dist<2)dist=2;
          damage_unit(unit[u], MISSILEDAMAGE/dist*5);
          normalize_vector_f(&pushdir.x, &pushdir.y, &pushdir.z);
          unit[u]->flightdir.x+=pushdir.x*290/unit[u]->udata->mass/dist*5;
          unit[u]->flightdir.y+=pushdir.y*290/unit[u]->udata->mass/dist*5;
          unit[u]->flightdir.z+=pushdir.z*290/unit[u]->udata->mass/dist*5;
       }
    }
}

void missile_hit_fighter(UNIT *u1, UNIT *u2)
{
//////////////////////////////////////

    VECTOR pushdir;
    VECTOR v1;
    VECTOR col_point;
    VECTOR n;
    VECTOR turn;
    MATRIX_f m;
    int t1, t2;
    
    u2->udata->model.colmodel->getCollidingTriangles(t1, t2);
    n=u2->udata->col_model.o3d[0].n[t1];
    u2->udata->model.colmodel->getCollisionPoint ( &col_point.x, 0);
    vector_sub(&col_point, (VECTOR*)&u2->m.t[0], &pushdir); 
    normalize_vector_v(&pushdir);
    u2->flightdir.x+=pushdir.x*200/u2->udata->mass;;
    u2->flightdir.y+=pushdir.y*200/u2->udata->mass;;
    u2->flightdir.z+=pushdir.z*200/u2->udata->mass;;
    
    //////////////////////////////////////////////////////////////////////////

    m=u2->m;
    zero_matrix(&m);
    apply_matrix_f(&m, n.x, n.y, n.z, &n.x, &n.y, &n.z); //drehe Normalvektor auf WorldSpace
    v1.x=col_point.x+n.x; //Erstelle Punkt: Einschlagspunkt plus Normalvektor        
    v1.y=col_point.y+n.y;
    v1.z=col_point.z+n.z;
    cross_product_v((VECTOR*)&u2->m.t[0], &col_point, &v1, &turn);  //turn steht senkrecht zu dieser Ebene
//    float hitvalue=-vector_length_f(turn.x, turn.y, turn.z);//Intensität des Drehimpulses

    apply_matrix_f(&m, turn.x, turn.y, turn.z, &turn.x, &turn.y, &turn.z);
    u2->wink_ist.x-=turn.x*50;
    u2->wink_ist.y-=turn.y*50;
    u2->wink_ist.z-=turn.z*50;
    ///////////////////////////////////////////////////////////////////////////
    
    m=u2->m;
    translate_matrix_v(&m, &col_point);
    explosion2.start(&m, &u2->speed, u1->property);
 
    damage_unit(u2, MISSILEDAMAGE);
    kill_unit(u1->index);
    flush.start(&col_point, FLUSH_CIRCLE);
    sound.play("missile_explodes_01", &col_point);

//    if(u2->shield > 0)play_3D_sound_v(&col_point, snd_shieldhit);
    if(u2->shield > 0)sound.play("shield_hit_01", &col_point);
    else sound.play("metal_hit", &col_point);
    check_explosion_radius(&col_point, 25, u2);
}

void fighter_hit_object(UNIT *u1, UNIT *u2)
{
/*     float t1[9], t2[9];
 //   u1->udata->model.colmodel->getCollidingTriangles(t1, t2, 1);
 //   allegro_message("%.2f %.2f %.2f", t1[0], t1[1], t1[2]);
    damage_unit(u1, 100*fmult);
    VECTOR push;
    push.x=u2->m.t[0]-u1->m.t[0];
    push.y=u2->m.t[1]-u1->m.t[1];
    push.z=u2->m.t[2]-u1->m.t[2];
    normalize_vector_f(&push.x, &push.y, &push.z);
    u1->flightdir.x+=push.x*(u1->speed/5+2);
    u1->flightdir.y+=push.y*(u1->speed/5+2);
    u1->flightdir.z+=push.z*(u1->speed/5+2);*/
    u1->flightdir=vect(0,0,0);
}

void missile_hit_object(UNIT *u1, UNIT *u2)
{

    VECTOR col_point;
    u2->udata->model.colmodel->getCollisionPoint ( &col_point.x, 0); 
    MATRIX_f m=u2->m;
    translate_matrix_v(&m, &col_point);
    explosion2.start(&m, &u2->speed, u1->property);
    damage_unit(u2, MISSILEDAMAGE);
    kill_unit(u1->index);
    sound.play("missile_explodes_01", &col_point);
    if(u2->shield > 0)sound.play("shield_hit_01", &col_point);
    else sound.play("metal_hit", &col_point);
    flush.start(&col_point, FLUSH_CIRCLE);
    check_explosion_radius(&col_point, 25, u2);

}

void shot_hit_fighter(SHOT *sht, UNIT *u)
{
    explosion1.start(&vect(0,0,0), &sht->m);
    VECTOR turn;
    VECTOR col_point;
    VECTOR push, n;
    MATRIX_f m;
 
    int t1, t2;
    u->udata->model.colmodel->getCollidingTriangles(t1, t2);
    n=u->udata->col_model.o3d[0].n[t1];
    u->udata->model.colmodel->getCollisionPoint ( &col_point.x, 0); //ermittle Kollisionspunkt
    m=u->m;                              //m=unit-matrix  
    translate_matrix_v(&m, &col_point);           //erstelle Matrix für Explosion
//    explosion2.start(&m, &u->speed, sht->property);
    flush.start(&col_point, FLUSH_CIRCLE);

    //spanne Ebene auf (Kollisionspunkt, Mittelpunkt, Schuß-Vektor)

    m.v[2][0]=col_point.x+sht->m.v[2][0];         //errechne den Punkt: Kollisionspunkt plus Einschlagsvektor         
    m.v[2][1]=col_point.y+sht->m.v[2][1];
    m.v[2][2]=col_point.z+sht->m.v[2][2];
    cross_product_v((VECTOR*)&u->m.t[0], &col_point, (VECTOR*) &m.v[2][0], &turn);  //turn steht senkrecht zu dieser Ebene
    float hitvalue=-vector_length_f(turn.x, turn.y, turn.z);//Intensität des Drehimpulses
    m=u->m;                //m=unit-matrix  
    zero_matrix(&m);       //erstelle reine Rotations-Matrix 
    apply_matrix_f(&m, turn.x, turn.y, turn.z, &turn.x, &turn.y, &turn.z);  //drehe Drehvektor auf ModelSpace
    u->wink_ist.x-=turn.x*20;
    u->wink_ist.y-=turn.y*20;
    u->wink_ist.z-=turn.z*20;
    
    /////////////////////////////
//    apply_matrix_f(&m, n.x, n.y, n.z, &push.x, &push.y, &push.z);  //drehe Stoßvektor auf ModelSpace
//	get_align_matrix_f(&m, n.x, n.y, n.z, 1, 0, 0); //Achtung auf den Up-Vektor
//	translate_matrix_v(&m, &col_point);
//    explosion1.start(&vect(0,0,0), &m);
//    push.x/=hitvalue/5;
//    push.y/=hitvalue/5;
//    push.z/=hitvalue/5;
//    vector_sub(&u->flightdir, &push, &u->flightdir);
    damage_unit(u, sht->type);
//    shot.kill(sht->index);
    sht->active=FALSE;
    if(u->shield > 0)sound.play("shield_hit_01", &col_point);
    else sound.play("metal_hit", &col_point);
}

void shot_hit_object(SHOT *sht, UNIT *u)
{
    MATRIX_f m=identity_matrix_f, m2;
    VECTOR n, col_point;
    int t1, t2;
    u->udata->model.colmodel->getCollisionPoint (&col_point.x, 0); 
    sound.play("shot_hit", &col_point);
    flush.start(&col_point, 0);
    damage_unit(u, sht->type);
//    shot.kill(sht->index);
    sht->active=FALSE;

    u->udata->model.colmodel->getCollidingTriangles(t1, t2);
    n=u->udata->col_model.o3d[0].n[t1];
    m=u->m;
    zero_matrix(&m);
//    allegro_message(" t1: %d\n %.2f\n%.2f\n%.2f", t1, n.x, n.y, n.z);
    invert_matrix(&u->m, &m);
    zero_matrix(&m);
    apply_matrix_f(&m, n.x, n.y, n.z, &n.x, &n.y, &n.z);  
    
//   	get_align_matrix_f(&m, n.x, n.y, n.z, player.camera.m.v[0][0], player.camera.m.v[0][1], player.camera.m.v[0][2]); //Achtung auf den Up-Vektor
//   	get_align_matrix_f(&m, n.x, n.y, n.z, 1, 0, 0); //Achtung auf den Up-Vektor
//   	invert_matrix(&m, &m2);
    m=align_matrix_z(n);
//    allegro_message(" t1: %d\n %.2f\n%.2f\n%.2f\n%.2f\n%.2f\n%.2f\n", t1, n.x, n.y, n.z, m.v[2][0],m.v[2][1],m.v[2][2]);
	translate_matrix_v(&m, &col_point);
    explosion1.start(&vect(0,0,0), &m);

}


bool cd_fighter_object(UNIT *u1, UNIT *u2)
{
    int t1, t2, iteration=0;
    VECTOR col_point;
    bool hit=0;
    float m_per_frame=u1->speed*fmult;
    static GLfloat glm1[16], glm2[16];
    allegro_gl_MATRIX_f_to_GLfloat(&u1->m, glm1);
    if(u2->object[0].moved)
    {
       MATRIX_f m=u2->object[0].m;
       matrix_mul_f(&m, &u2->m, &m);
       allegro_gl_MATRIX_f_to_GLfloat(&m, glm2);
    }
    else allegro_gl_MATRIX_f_to_GLfloat(&u2->m, glm2);
    u2->udata->model.colmodel->setTransform (glm2); 
    u1->udata->model.colmodel->setTransform (glm1); 
    if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
    {
       hit=TRUE;
       u1->m.t[0]+=u1->flightdir.x*fmult/2;
       u1->m.t[1]+=u1->flightdir.y*fmult/2;
       u1->m.t[2]+=u1->flightdir.z*fmult/2;
       u1->udata->model.colmodel->setTransform (glm1);
       if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
       {
          u1->m.t[0]+=u1->flightdir.x*fmult/4;
          u1->m.t[1]+=u1->flightdir.y*fmult/4;
          u1->m.t[2]+=u1->flightdir.z*fmult/4;
          u1->udata->model.colmodel->setTransform (glm1);
          if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
          {
             u1->m.t[0]+=u1->flightdir.x*fmult/4;
             u1->m.t[1]+=u1->flightdir.y*fmult/4;
             u1->m.t[2]+=u1->flightdir.z*fmult/4;
          }
       }
       else
       {
          u1->m.t[0]-=u1->flightdir.x*fmult/4;
          u1->m.t[1]-=u1->flightdir.y*fmult/4;
          u1->m.t[2]-=u1->flightdir.z*fmult/4;
          u1->udata->model.colmodel->setTransform (glm1);
          if (u1->udata->model.colmodel->collision(u2->udata->model.colmodel))
          {
             u1->m.t[0]+=u1->flightdir.x*fmult/4;
             u1->m.t[1]+=u1->flightdir.y*fmult/4;
             u1->m.t[2]+=u1->flightdir.z*fmult/4;
          }
       }
    }
    return(hit);
}

void cd_fighter_fighter(short u1, short u2)
{
}


void collision_detection()
{
    short u, i, u1, u2;
    bool breakitdown;
    for(i=0; i<shot.total; i++)
    for(u=0;u<unit_anz;u++)
    if (shot.shot[i]->property!=unit[u])
    if (shot.shot[i]->active)
    if (check_col(&shot.shot[i]->m, &unit[u]->m, unit[u]->udata->model.radius + (shot.shot[i]->speed + unit[u]->speed)*fmult))  //COLMODEL.RADIUS!!!
    {
    breakitdown=FALSE;
    switch(unit[u]->udata->type)
    {
       case UNIT_MISSILE:
            if(cd_shot_unit(shot.shot[i], unit[u]))
            {
               sound.play("missile_explodes_01", unit[u]->m.t);
               explosion2.start(&unit[u]->m, &unit[u]->speed, shot.shot[i]->property);
//               shot.kill(i);
               shot.shot[i]->active=FALSE;
               kill_unit(u);
      //         i--;
            }
       break;
       case UNIT_FIGHTER:
            if(cd_shot_unit(shot.shot[i], unit[u]))
            {
               shot_hit_fighter(shot.shot[i], unit[u]);
               breakitdown=TRUE;
          //     i--;
            }
       break;
       case UNIT_OBJECT:
            if(cd_shot_unit(shot.shot[i], unit[u]))
            {
               shot_hit_object(shot.shot[i], unit[u]);
               breakitdown=TRUE;
       //        i--;
            }
       break;
       case UNIT_LEVEL:
            if(cd_shot_unit(shot.shot[i], unit[u]))
            {
               shot_hit_object(shot.shot[i], unit[u]);
               breakitdown=TRUE;
      //        i--;
            }
       break;
    }
    if(breakitdown)
    { 
        if(shot.total==0) break;
        else
        {
       //  allegro_message("%d",shot.total);
      //   u=0;
         }
    }//break;}

    }

    for(u1=0; u1<unit_anz; u1++)
    for(u2=u1+1;u2<unit_anz;u2++)
    if (check_col(&unit[u1]->m, &unit[u2]->m, unit[u1]->udata->model.radius + unit[u2]->udata->model.radius + (unit[u1]->speed + unit[u2]->speed)*fmult))//COLMODEL.RADIUS!!!
    switch(unit[u1]->udata->type)
    {
       case UNIT_MISSILE:
       if(unit[u1]->property!=unit[u2])
       switch(unit[u2]->udata->type)
       {
          case UNIT_FIGHTER:
               if(cd_missile_unit(unit[u1], unit[u2]))
               missile_hit_fighter(unit[u1], unit[u2]);
          break;
          case UNIT_OBJECT:
                if(cd_missile_unit(unit[u1], unit[u2]))
               missile_hit_object(unit[u1], unit[u2]);
          break;
          case UNIT_MISSILE:
  //             cd_missile_unit(unit[u1], unit[u2]);
          break;
          case UNIT_LEVEL:
                if(cd_missile_unit(unit[u1], unit[u2]))
               missile_hit_object(unit[u1], unit[u2]);
          break;
       }
       break;
       case UNIT_FIGHTER:
       switch(unit[u2]->udata->type)
       {
          case UNIT_MISSILE:
               if(unit[u2]->property!=unit[u1])
               if(cd_missile_unit(unit[u2], unit[u1]))
               missile_hit_fighter(unit[u2], unit[u1]);
          break;
          case UNIT_OBJECT:
               if(cd_fighter_object(unit[u1], unit[u2]))
               fighter_hit_object(unit[u1], unit[u2]);
          break;
       }
       break;
       case UNIT_OBJECT:
       switch(unit[u2]->udata->type)
       {
          case UNIT_MISSILE:
               if(unit[u2]->property!=unit[u1])
               if(cd_missile_unit(unit[u2], unit[u1]))
               missile_hit_object(unit[u2], unit[u1]);
          break;
          case UNIT_FIGHTER:
               if(cd_fighter_object(unit[u2], unit[u1]))
               fighter_hit_object(unit[u2], unit[u1]);
          break;
       }
       break;
       case UNIT_LEVEL:
       switch(unit[u2]->udata->type)
       {
          case UNIT_MISSILE:
               if(unit[u2]->property!=unit[u1])
               if(cd_missile_unit(unit[u2], unit[u1]))
               missile_hit_object(unit[u2], unit[u1]);
          break;
       }
       break;
    }       
    
}



bool check_aiming(UNIT *u, const char name[])
{
    MATRIX_f om;
    static GLfloat glm[16];
    get_object_matrix_world(&om, u, get_object_by_name(&u->udata->model, name));
    if(u->target->object[0].moved)
    {
       MATRIX_f m=u->target->object[0].m;
       matrix_mul_f(&m, &u->target->m, &m);
       allegro_gl_MATRIX_f_to_GLfloat(&m, glm);
    }
    else allegro_gl_MATRIX_f_to_GLfloat(&u->target->m, glm);
    glm[12]=player.vorhalt.t[0];
    glm[13]=player.vorhalt.t[1];
    glm[14]=player.vorhalt.t[2];

    om.v[2][0]*=-1;
    om.v[2][1]*=-1;
    om.v[2][2]*=-1;

    u->target->udata->model.colmodel->setTransform (glm); 
    return(u->target->udata->model.colmodel->rayCollision (&om.t[0], &om.v[2][0], 1));
}

void draw_monitor()
{
	glDisable(GL_DEPTH_TEST);
    int i, o=0;
    float monwink, aspect;
    VECTOR heading;
  //  MATRIX_f m=player.unit->m;
  //  zero_matrix(&m);

    get_heading(&player.unit->m, &player.vorhalt, &heading);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
      glViewport(378, 12, 267, 200);
   	glColor4ub(0,0,0, 255);
	gluOrtho2D(-50, 50, -50, 50);
	      glBegin(GL_QUADS);
	      glVertex2f(-50,-50);
	      glVertex2f(50,-50);
	      glVertex2f(50, 50);
	      glVertex2f(-50,50);
	      glEnd(); 
	glLoadIdentity();

	gluOrtho2D(-65, 65, -40, 40);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	aspect=player.unit->target->udata->model.size/player.distance_to_target;
	monwink=RAD_2_DEG(atan(aspect))*1.2;
      glViewport(378, 10, 267, 200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(monwink, 1.333, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(player.camera.pos.x, player.camera.pos.y, player.camera.pos.z, player.unit->target->m.t[0], player.unit->target->m.t[1], player.unit->target->m.t[2], player.unit->m.v[1][0], player.unit->m.v[1][1], player.unit->m.v[1][2]);
	glColor4ub(100, 0, 200, 255);
	glDisable(GL_DEPTH_TEST);
	
	glPushMatrix();
	draw_unit_lines(player.unit->target);
	glPopMatrix();
	glColor4ub(180, 160, 10, 255);
	for (i=0; i<unit_anz; i++)
	if(unit[i]->property==player.unit && unit[i]->status==AI_SEEKING)
	{
	   glPushMatrix();
       glMultMatrix_allegro(&unit[i]->m);
	   glVertexPointer(3, GL_FLOAT, 0, unit[i]->udata->model.o3d[0].vert);
       glDrawElements(GL_QUADS, unit[i]->udata->model.o3d[0].poly_anz*4, GL_UNSIGNED_SHORT, &unit[i]->udata->model.o3d[0].i[0]);
	   glPopMatrix();
    } 
	
	
	glPushMatrix();

      glViewport(378, 10, 267, 200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(monwink, 1.333, 0, 200);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	gluLookAt(0, 0, 0, 0, heading.y, heading.z, 0, -1, 0);
	glTranslatef(0, 0, 0);
	glRotatef(0, 0,0, 0);
	glPushMatrix();
	glBegin(GL_LINES);
//	glColor4ub(100,128, 0,0);
//	glVertex3f( 10, sinus10[0][0]*10, sinus10[0][1]*10);
//	glVertex3f(-10, sinus10[0][0]*10, sinus10[0][1]*10);
	glColor4ub(40, 50, 0, 255);
	for (i=0; i<36; i++)
	{
		glVertex3f( 10,sinus10[i][0]*10, sinus10[i][1]*10);
		glVertex3f(-10, sinus10[i][0]*10, sinus10[i][1]*10);
	}
	glEnd();
	glPopMatrix();
      glViewport(378, 10, 267, 200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(monwink, 1.333, 0, 200);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
	gluLookAt(0, 0, 0, heading.x, 0, heading.z, 0, 1, 0);
	glTranslatef(0, 0, 0);
	glRotatef(0, 0,0, 0);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	for (i=0; i<36; i++)
	{
		glVertex3f(sinus10[i][0]*10, 10, sinus10[i][1]*10);
		glVertex3f(sinus10[i][0]*10,-10, sinus10[i][1]*10);
	}
	glEnd();
	glPopMatrix();
	
    glColor4ub(80, 100, 0, 255);
      glViewport(378, 10, 267, 200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(5, 1.333, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, heading.x, -heading.y, heading.z, 0, 1, 0);
	glPushMatrix();
	glTranslatef(0, 0, 8);
    draw_circle_outline(0.15);
   	glPopMatrix();

    glColor4ub(130,160, 0, 255);
      glViewport(378, 10, 267, 200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(monwink, 1.333, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, heading.x, -heading.y, heading.z, 0, 1, 0);
	glPushMatrix();
	glTranslatef(0, 0, 8);
    draw_circle_outline(aspect);
   	glPopMatrix();

}

void draw_monitor2()
{
    int i;
   glViewport(378, 12, 267, 200);
   glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glColor4ub(0,0,0, 255);
	gluOrtho2D(-50, 50, -50, 50);
	      glBegin(GL_QUADS);
	      glVertex2f(-50,-50);
	      glVertex2f(50,-50);
	      glVertex2f(50, 50);
	      glVertex2f(-50,50);
	      glEnd(); 
	glLoadIdentity();
	glFrustum(-stereo.get_monitor_width()/2,
              stereo.get_monitor_width()/2,
             -stereo.get_monitor_height()/2,
              stereo.get_monitor_height()/2,
              stereo.get_distance_to_monitor(),
              stereo.get_far_clip());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glMultMatrix_allegro(&player.camera.m);
	glTranslatef(	-player.camera.pos.x,
			        -player.camera.pos.y,
		            -player.camera.pos.z);
 	glColor4ub(100, 128, 0, 255);
	for (i=0;i<unit_anz;i++)
	if(unit[i]->rendered)
    {
       draw_unit_lines(unit[i]);
       if(unit[i]->status == AI_DUMBFIRE)
       {
          glBegin(GL_LINES);
          glVertex3fv(&unit[i]->m.t[0]);
          glVertex3fv(&unit[i]->start_pos.x);
          glEnd();
       }
    }   

	glColor4ub(180, 160, 100, 255);

    glPushMatrix();
    glTranslatef(player.norm_flightdir.x*10, player.norm_flightdir.y*10, player.norm_flightdir.z*10);
    draw_circle_outline(0.1);
    draw_circle_outline(0.01);
    draw_circle_outline(0.001);
	   glPopMatrix();

 	glColor4ub(200, 255, 0, 255);
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);
//    glVertexPointer(3, GL_INT, 0, zielkreuz[0]);
//    glDrawArrays(GL_LINES, 0, 8);
    glTranslatef(0,0,1);
    draw_circle_outline(2);
   glEnable(GL_DEPTH_TEST);

}

void draw_monitor3()
{
    int i;
    glEnable(GL_DEPTH_TEST);

   	glViewport(100, 10, 112, 80);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.12, 1.12, -0.8, 0.8, 2, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 //   glMultMatrix_allegro(&player.camera.m);
	glTranslatef(	0,    0,    -12);
	glMultMatrix_allegro(&player.zero_m);
    glColor4ub(128, 128, 20, 255);
 //   draw_objects_lines(0, 0);
    draw_objects_lines(player.unit, 0);

}



void draw_cockpit()
{
    allegro_gl_set_allegro_mode();
    if(player.unit->target!=NO_TARGET)blit(logo[player.unit->target->party], screen, 0, 0, 277, 400, 30, 35);
    blit(logo[0], screen, 0, 0, 463, 398, 30, 35);
//    if(player.target_monitor_interference>chrono)blit(rauschen, screen, rand()%84, rand()%164, 267, 380, 172, 90);
    allegro_gl_unset_allegro_mode(); 
}

void draw_cockpit01()
{
    short i;
    char buf[128];
    glDisable(GL_DEPTH_TEST);

//	draw_cockpit_bitmap();
	glColor4ub(200, 255, 0, 255);
	glPointSize(2.0);
	for (i=1;i<unit_anz; i++)
	{
	      draw_scanner_point(&unit[i]->heading);
    }
    glColor4ub(200, 255, 0, 255);
    glPointSize(1.0);
   	for (i=0;i<shot.total; i++)
	{
	      draw_scanner_point(&shot.shot[i]->heading);
    }
    if(player.unit->target!=NO_TARGET)
    {
       glColor4ub(255, 255, 255, 255);
       glPointSize(3.0);
       draw_scanner_point(&player.unit->target->heading);
    }
    if(player.target_monitor_interference<chrono)
    {
	   if (player.unit->target!=NO_TARGET && !buttn_pressed)draw_monitor();
	   else draw_monitor2();
	}
//	draw_monitor3();
	glViewport(0, 0, SCREEN_W, SCREEN_H);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0 , SCREEN_W, 0, SCREEN_H);
//    draw_cockpit();

  	glColor4ub(100, 128, 0, 255);
   glPushMatrix();
   glTranslatef(132, 52,0);  	
	draw_circle_outline(32);
   glPopMatrix();

   glPushMatrix();
   glTranslatef(202, 52,0);  	
	draw_circle_outline(32);
   glPopMatrix();
	
  	glColor4ub(100, 128, 0, 255);
   	      glPushMatrix();
   glTranslatef(260, 90,0);  	
	draw_circle_outline(40);
   	      glPopMatrix();
   	      glPushMatrix();
   glTranslatef(80, 90,0);  	
	draw_circle_outline(30);
   	      glPopMatrix();
   	      glPushMatrix();
   glTranslatef(80, 90,0);  	
	draw_circle_outline(40);
   	      glPopMatrix();
   	      
	if(player.unit->target!=NO_TARGET)
	{
	glColor4ub(128, 128, 128, 255);

	   if(!player.target_is_visible)
	   {
//	gluOrtho2D(0 , SCREEN_W, 0, SCREEN_H);
   	      glPushMatrix();
   	      glTranslatef(512,384,0);  	
   	      translate_target_pointer(player.unit->target->heading, 512, 384, 0.80);
   	      MATRIX_f m;
   	      get_align_matrix_f(&m,0,0,1, player.unit->target->heading.x, player.unit->target->heading.y, 0);
   	      glMultMatrix_allegro(&m);
   	      glColor4ub(64, 64, 100, 255);
   	      draw_arrow();
   	      glPopMatrix();
//	gluOrtho2D(0 , 512, 0, 384);
       } 


//       if(player.target_monitor_interference<chrono)
// 	   {
//  	      glPushMatrix();
//          glTranslatef(266,38,0);
//          sprintf(buf, "hull: %.1f\nshld: %.1f\nspd : %.1f\ndist: %.1f\nrender: %d\npres: %.6f", player.unit->target->hull, player.unit->target->shield, player.unit->target->speed, player.distance_to_target, player.unit->target->rendered, player.unit->target->presence);
//          draw_string(buf);
//          glPopMatrix();
// 	      glPushMatrix();
//          glTranslatef(230,98,0);
//	      draw_string(player.unit->target->name);
// 	      glPopMatrix();
//	   }
  	}
//
// 	glPushMatrix();
//    glTranslatef(116,15,0);
//    glRotatef(-90,0, 0, 1);
//    draw_energy_bar(5, player.unit->speed_soll.z*1.5);    
//    glTranslatef(0,1,0);
//    draw_energy_bar(5, player.unit->speed*1.5);    
// 	glPopMatrix();
//
   	      glColor4ub(100, 196, 100, 255);
 	
    glPushMatrix();
 	sprintf(buf, "%.1f", player.unit->speed);
    glTranslatef(280, 26,0);
   glScalef(2,2,2);
    draw_string(buf);
 	glPopMatrix();

//
//    glPushMatrix();
//	sprintf(buf, "%.1f", player.unit->speed_soll.z);
//    glTranslatef(223, 10,0);
//    draw_string(buf);
// 	glPopMatrix();
//
// 	glPushMatrix();
//    glTranslatef(268,10,0);
//    draw_energy_bar(6, player.unit->gunpower[1]*0.9);    
// 	glPopMatrix();
//	
// 	glPushMatrix();
//    glTranslatef(430,10,0);
//    draw_energy_bar(6, player.unit->gunpower[0]*0.9);    
// 	glPopMatrix();
//
// 	glPushMatrix();
//    glTranslatef(440,56,0);
//    sprintf(buf, "ia  : %d\naa  : %d\nfps : %.1f\nshts: %d\nunts: %d\njoy : %d\ngun : %.2f\ncoolx:%i\ncooly:%i", get_i_action_anz(), get_a_action_anz(), fps_rate, shot.total, unit_anz, stick.state, player.unit->gunangel, stick.head_x, stick.head_y);
//	draw_string(buf);
// 	glPopMatrix();
// 	
//	glPushMatrix();
//	sprintf(buf, "hull: %.1f\nshld: %.1f\ntrst: %.f\nspd : %.1f\ndumb: %d\nseek: %d", player.unit->hull, player.unit->shield, player.unit->speed_soll.z, player.unit->speed, player.unit->dumbmissiles, player.unit->seekmissiles);
//    glTranslatef(100,38,0);
//    draw_string(buf);
// 	glPopMatrix();
// 	
}


void stereo_render(bool eye)
/*  In dieser Funkion wird alles Stereoskopische gezeichnet.
 *  Sie wird deshalb auch zweimal pro Frame aufgerufen.
 *  Einmal mit eye=0 und einmal mit eye=1;
 */
{
    int i;
    stereo.set_to(eye);
    if(player.bg_is_drawn)
    {
       glDisable(GL_DEPTH_TEST);
       background.draw();
       glEnable(GL_DEPTH_TEST);
    }
//	 stereo.position_camera(&player.camera.m, &player.camera.pos);

    explosion1.draw();
    shot.draw();
	
    draw_highways();
    
	for (i=0;i<unit_anz;i++)
	if((unit[i]->rendered==RND_NORMAL) || (unit[i]->rendered==RND_NO_OUTLINES))
	{
	 /*  if   (unit[i]->justhit && (unit[i]->udata->type==UNIT_FIGHTER)) draw_unit(unit[i], eye+2);
	   else*/ draw_unit(unit[i]);
//       if(unit[i]->udata->type==UNIT_FIGHTER && (unit[i]->status != AI_PLAYER)) draw_engine_outline(unit[i], eye);
    }
    
//    glEnable(GL_DEPTH_TEST);
    glDisable(GL_POLYGON_OFFSET_FILL);
	for (i=0;i<unit_anz;i++)
    
	if(unit[i]->rendered)  //Zeichne Outlines
    {
       if((unit[i]->rendered==RND_NORMAL) || (unit[i]->rendered==RND_ONLY_OUTLINES))
       {
          glLineWidth(unit[i]->presence);
          color16(unit[i]->outlinecolor);
          draw_unit_lines(unit[i]);
       }
       else if(unit[i]->rendered==RND_BIG_POINT)
       {
           color16(15);
           glPointSize(2.0);
           glBegin(GL_POINTS);
           glVertex3fv(&unit[i]->m.t[0]);
           glEnd();
       }
       else if(unit[i]->rendered==RND_SMALL_POINT)
       {
           color16(15);
           glPointSize(1.0);
           glBegin(GL_POINTS);
           glVertex3fv(&unit[i]->m.t[0]);
           glEnd();
       }
    }
    glLineWidth(1.0);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    

   for(i=0;i<unit_anz;i++)
   {
      if(unit[i]->status == AI_DUMBFIRE)
      {
         color16(10 + rand()%6);
         glBegin(GL_LINES);
         glVertex3fv(&unit[i]->m.t[0]);
         glVertex3fv(&unit[i]->start_pos.x);
         glEnd();
      }
   }
    if (player.unit->target!=NO_TARGET)
    {
       glDisable(GL_DEPTH_TEST);
       
	   glPushMatrix();
	   glTranslatef(player.unit->target->m.t[0], player.unit->target->m.t[1], player.unit->target->m.t[2]);
	   glMultMatrix_allegro(&player.zero_m);
	   glDisable (GL_LINE_SMOOTH);
	   color16(15);
//	   draw_marker(player.unit->target->udata->model.size/2);  //male Rechteck um Ziel
	   draw_marker(player.unit->target->dist*0.05);                                //male Rechteck um Vorhalt
	   glPopMatrix();

	   glPushMatrix();
	   glTranslatef(player.vorhalt.t[0], player.vorhalt.t[1], player.vorhalt.t[2]);
	   glMultMatrix_allegro(&player.zero_m);
	   draw_marker(player.distance_to_target*0.05);                                //male Rechteck um Vorhalt
	   glPopMatrix();
	   
	   if(player.gunhits)
	   {
	      glPushMatrix();
	      glTranslatef(player.hitpoint.x, player.hitpoint.y, player.hitpoint.z);
	      glMultMatrix_allegro(&player.zero_m);

	      color16(15);
	      draw_circle_outline(player.distance_to_target*0.005);
	      glPopMatrix();
	   }

	   glEnable (GL_LINE_SMOOTH);
       glPopMatrix();
    }


     flush.draw();
	explosion2.draw();

    stereo.set_to(eye);
    stereo.position_camera(&identity_matrix_f, (VECTOR*)&player.camera.offset.t[0]);
    color16(13);
    draw_object_lines(&cockp.o3d[0]);
    if(player.debris_is_drawn)
    {
       glPushMatrix();
       debris.draw();
       glPopMatrix();
	}    
}

void render()
{
    int j, u, i;
  //  glEnable(GL_SCISSOR_TEST);
  // glIsEnabled(GL_SCISSOR_TEST);
 //  glScissor(0, 150, 640, 330);
//   glViewport(0,  85, SCREEN_W, SCREEN_H-85); //113
   glViewport(0,  0, SCREEN_W, SCREEN_H); //113
    ////////////////////////////////////////////////////////////////////////////
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	stereo_render(LEFT_EYE);
	////////////////////////////////////////////////////////////////////////////
	glClear(GL_DEPTH_BUFFER_BIT );
	stereo_render(RIGHT_EYE);
	////////////////////////////////////////////////////////////////////////////
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(player.camera.perspective==CAMERA_COCKPIT)
	{
	   glViewport(0,  0, SCREEN_W, SCREEN_H);
	   if(player.unit->justhit)
	   {
         glViewport(0, 0, SCREEN_W, SCREEN_H);
	      glMatrixMode(GL_PROJECTION);
	      glLoadIdentity();
	      glMatrixMode(GL_MODELVIEW);
	      glLoadIdentity();
	      gluOrtho2D(0 , SCREEN_W, 0, SCREEN_H);//male auffleckernden Schild auf den Schirm
	      glColor4ub(160, 0, 240, 128);
	      glBegin(GL_QUADS);
	      glVertex2f(0,0);
	      glVertex2f(SCREEN_W,0);
	      glVertex2f(SCREEN_W, SCREEN_H);
	      glVertex2f(0,SCREEN_H);
	      glEnd();
	   }
	   draw_cockpit01();
	}   
	glFlush();
	allegro_gl_flip();
}

void turn_unit(UNIT *u)
{
    MATRIX_f m; VECTOR pos;
    pos=trans_matrix_to_vector(&u->m);
    zero_matrix(&u->m);
    accelerate(u->wink_soll.x, &u->wink_ist.x, ROTACC);
    accelerate(u->wink_soll.y, &u->wink_ist.y, ROTACC);
    accelerate(u->wink_soll.z, &u->wink_ist.z, ROTACC);
    get_rotation_matrix_f(&m, u->wink_ist.x*fmult, u->wink_ist.y*fmult, u->wink_ist.z*fmult);
    matrix_mul_f(&u->m, &m,  &u->m);
    translate_matrix_v(&u->m, &pos);
}

void move_ship(UNIT *u)
{
    float speed;
    VECTOR dir_soll, dir_delta;
    dir_soll=v_mul((VECTOR*)&u->m.v[2][0], u->speed_soll.z);
    if(u->speed_soll.x)
    {
       vector_add(&dir_soll, &v_mul((VECTOR*)&u->m.v[0][0], u->speed_soll.x), &dir_soll);
    }
    if(u->speed_soll.y)
    {
       vector_add(&dir_soll, &v_mul((VECTOR*)&u->m.v[1][0], u->speed_soll.y), &dir_soll);
    }
    if((u->flightdir.x != dir_soll.x) ||
       (u->flightdir.y != dir_soll.y) ||
       (u->flightdir.z != dir_soll.z))
    {
       vector_sub(&u->flightdir, &dir_soll, &u->dir_delta);
       dir_delta=u->dir_delta;
       if(dir_delta.x<0)dir_delta.x*=-1;
       if(dir_delta.y<0)dir_delta.y*=-1;
       if(dir_delta.z<0)dir_delta.z*=-1;
       
       normalize_vector_v(&dir_delta);
       accelerate(dir_soll.x, &u->flightdir.x, dir_delta.x*u->udata->acceleration);
       accelerate(dir_soll.y, &u->flightdir.y, dir_delta.y*u->udata->acceleration);
       accelerate(dir_soll.z, &u->flightdir.z, dir_delta.z*u->udata->acceleration);      
    }
    u->speed=vector_length_v(&u->flightdir);
    vector_sub((VECTOR*)&u->m.t[0], &v_mul(&u->flightdir, fmult), (VECTOR*)&u->m.t[0]);

}

void start_missile(UNIT *u, short o, char status)
{
   unit[unit_anz]->name="Missile";
   unit[unit_anz]->party=u->party;
   unit[unit_anz]->property=u;
   unit[unit_anz]->status=status;
   unit[unit_anz]->shield=0;
   unit[unit_anz]->hull=100;
   get_object_matrix_world(&unit[unit_anz]->m, u, o);
   unit[unit_anz]->flightdir=u->flightdir;
   unit[unit_anz]->speed=u->speed;
   unit[unit_anz]->speed_soll.z=MISSILEMAXSPEED;
   unit[unit_anz]->t=chrono;
   unit[unit_anz]->vulnerable=TRUE;

   if(status ==AI_SEEKING)
   {
      unit[unit_anz]->target=u->target;
      unit[unit_anz]->udata=&seeker_missile;
      unit[unit_anz]->flightdir.x+=unit[unit_anz]->m.v[2][0]*MISSILESTARTSPEED;
      unit[unit_anz]->flightdir.y+=unit[unit_anz]->m.v[2][1]*MISSILESTARTSPEED;
      unit[unit_anz]->flightdir.z+=unit[unit_anz]->m.v[2][2]*MISSILESTARTSPEED;
   }
   else
   {
       unit[unit_anz]->udata=&dumb_missile;
      unit[unit_anz]->start_pos=trans_matrix_to_vector(&unit[unit_anz]->m);
      unit[unit_anz]->flightdir.x+=unit[unit_anz]->m.v[2][0]*DUMBMISSILESTARTSPEED;
      unit[unit_anz]->flightdir.y+=unit[unit_anz]->m.v[2][1]*DUMBMISSILESTARTSPEED;
      unit[unit_anz]->flightdir.z+=unit[unit_anz]->m.v[2][2]*DUMBMISSILESTARTSPEED;
   }
   prepare_object_matrices(unit[unit_anz]); 
   spin(unit[unit_anz], "base", 2, 200);
   sound.play("missile_starts_02", unit[unit_anz]->m.t);
   unit_anz++;
}

void raptor_start_shot(UNIT *u)
{
    if(chrono-u->shotdelay > 0)
    {
	      if ((u->gun==1) && (u->gunpower[0]>=u->udata->gun_erergy_drain))
	      {
	          shot.start(u, "lemit", u->udata->shottype, -1);
             sound.play("laser_02", 48, 32, 1000, 0);
	         u->gun=0;
	         u->gunpower[0]-=u->udata->gun_erergy_drain;
             u->shotdelay = chrono+u->udata->shootrate;
             raptor_fire_left_gun(u);
          }
          else if ((!u->gun) && (u->gunpower[1]>=u->udata->gun_erergy_drain))
          {
             shot.start(u, "remit", u->udata->shottype, 1);
   	         u->gun=1;
             sound.play("laser_02", 48, 228, 1000, 0);
   	         u->gunpower[1]-=u->udata->gun_erergy_drain;
             u->shotdelay = chrono+u->udata->shootrate;
             raptor_fire_right_gun(u);

          }
          else if((u->gunpower[0]>=u->udata->gun_erergy_drain) && (u->gunpower[1]>=u->udata->gun_erergy_drain))
          {
	         shot.start(u, "lemit", u->udata->shottype, -1);
	         shot.start(u, "remit", u->udata->shottype,  1);
             sound.play("laser_02", 48, 128, 1000, 0);
             u->shotdelay = chrono+2*u->udata->shootrate;
             u->gunpower[0]-=u->udata->gun_erergy_drain;
             u->gunpower[1]-=u->udata->gun_erergy_drain;
             raptor_fire_guns(u);
          }
    }
}


void buttn(char status)
{
    switch(status)
    {
       case BTTN_DEFAULT:
       sound.play("button_01", 24, 128, 1000, 0);
       input_delay=chrono-1;
       player.wait_for_release=TRUE;
       break;
       case BTTN_FAST:
       input_delay=chrono+50;
       break;
    }
}

void user_input()
{
    int i, input;
    static float old_speed;
    poll_keyboard();
    if(keypressed() &&(chrono>input_delay))
    {//player.wait_for_release=FALSE;
    if((player.wait_for_release && !player.keyboard_was_pressed) || !player.wait_for_release)
    {
       player.wait_for_release=FALSE;
       player.keyboard_was_pressed=TRUE;
       input=readkey();
       switch(input >> 8)
       {
          case KEY_M:
          buttn(BTTN_DEFAULT);
          if(player.unit->missile==AI_DUMBFIRE)player.unit->missile=AI_SEEKING;
          else player.unit->missile=AI_DUMBFIRE;
          break;
          
          case KEY_R:
          buttn(BTTN_DEFAULT);
	      player.unit->target=get_nearest_enemy_target(player.unit);
          break;
          
 	      case KEY_T:
          buttn(BTTN_DEFAULT);
	      get_next_target(player.unit);
	      break;
	      
          case KEY_Y:
          buttn(BTTN_DEFAULT);
	      get_last_target(player.unit);
	      break;

	      case KEY_O:
          buttn(BTTN_DEFAULT);
	      player.unit->target=NO_TARGET;
          break;
          
          case KEY_X:
          buttn(BTTN_DEFAULT);
	      if (player.unit->gun<2)player.unit->gun=2;
	      else player.unit->gun=0;
          break;

	      case KEY_ENTER:
          buttn(BTTN_DEFAULT);
          player.unit->speed_soll.z = player.unit->target->speed_soll.z;
          break;
          
  	      case KEY_BACKSPACE:
          buttn(BTTN_DEFAULT);
          player.unit->speed_soll.z = player.unit->udata->maxspeed;
          break;
          
  	      case KEY_0:              
          buttn(BTTN_DEFAULT);
          player.unit->speed_soll.z=0;
          break;

	      case KEY_PLUS_PAD:
          buttn(BTTN_FAST);
          player.unit->speed_soll.z++;
          if(player.unit->speed_soll.z> player.unit->udata->maxspeed)player.unit->speed_soll.z=player.unit->udata->maxspeed;
          break;

	      case KEY_5_PAD:
          buttn(BTTN_DEFAULT);
          player.camera.perspective=CAMERA_COCKPIT;
          break;

	      case KEY_2_PAD:
          buttn(BTTN_DEFAULT);
          player.camera.perspective=CAMERA_BACK;
          break;
          
	      case KEY_MINUS_PAD:
          buttn(BTTN_FAST);
          player.unit->speed_soll.z--;
          if(player.unit->speed_soll.z<0)player.unit->speed_soll.z=0;
          break;

	      case KEY_J:
          buttn(BTTN_DEFAULT);
	      stick.state++;
	      if(stick.state>JOY_5)stick.state=JOY_1;
          break;
          
	      case KEY_E:
         buttn(BTTN_FAST);
	      if(key_shifts & KB_SHIFT_FLAG) stereo.set_eye_seperation(stereo.get_eye_seperation()-0.001);
	      else stereo.set_eye_seperation(stereo.get_eye_seperation()+0.001);
         break;
          
          case  KEY_TAB:
          buttn(BTTN_FAST);
          player.unit->afterburner=TRUE;
          old_speed=player.unit->speed_soll.z;
          player.unit->speed_soll.z=player.unit->udata->afterburner_speed;
          break;
          
          case  KEY_B:
          buttn(BTTN_DEFAULT);
          player.bg_is_drawn=!player.bg_is_drawn;
          break;
          
          case  KEY_D:
          buttn(BTTN_DEFAULT);
          player.debris_is_drawn=!player.debris_is_drawn;
          break;
          
  /*        case  KEY_CAPSLOCK:
          buttn(BTTN_FAST);
          player.unit->afterburner=TRUE;
          old_speed=player.unit->speed_soll.z;
          player.unit->speed_soll.z=player.unit->udata->afterburner_speed;
          break;
    */      
   	      case KEY_F1:
          buttn(BTTN_DEFAULT);
          raptor_extend_gear(unit[1]);
          break;
   	      case KEY_F2:
          buttn(BTTN_DEFAULT);
          raptor_retract_gear(unit[1]);
          break;
  	       case KEY_F3:
          buttn(BTTN_DEFAULT);
          raptor_fire_guns(unit[1]);
          shot.start(unit[1], "lemit", unit[1]->udata->shottype, -1);
          shot.start(unit[1], "remit", unit[1]->udata->shottype, 1);
          sound.play("laser_02", unit[1]->m.t);
          break;
          
  	       case KEY_F4:
          buttn(BTTN_DEFAULT);
          if (i=raptor_fire_missile(unit[1], AI_DUMBFIRE))
          {       
             start_missile(unit[1], i, AI_DUMBFIRE);
          }
          break;
  	      case KEY_F5:
          buttn(BTTN_DEFAULT);
          raptor_open_cabin(unit[1]);
          break;
  	      case KEY_F6:
          buttn(BTTN_DEFAULT);
          raptor_close_cabin(unit[1]);
          break;
  	      case KEY_F7:
          buttn(BTTN_FAST);
          unit[1]->wink_soll.y=60;
          break;
  	      case KEY_F8:
          buttn(BTTN_FAST);
          unit[1]->wink_soll.y=0;
          break;
  	      case KEY_F9:
          buttn(BTTN_DEFAULT);
          if (i=raptor_fire_missile(unit[1], AI_SEEKING))
          {       
             start_missile(unit[1], i, AI_SEEKING);
          }
          break;
  	      case KEY_SPACE:
	      raptor_start_shot(player.unit);
          break;
          case KEY_LCONTROL:
          if (i=raptor_fire_missile(player.unit, player.unit->missile))
          {       
             start_missile(player.unit, i, player.unit->missile);
             player.unit->shotdelay = chrono;
          }
          break;
       }
    }
    }
    else
    {
        player.keyboard_was_pressed=FALSE;
    }
            
	if (stick.present)        //wenn ein Joystick angeschlossen ist
	{	 
	   refresh_stick(&stick);  //frage den Stick ab
       player.unit->speed_soll.x = -stick.head_x*player.unit->udata->max_x_speed; //seitliche Drift
       player.unit->speed_soll.y =  stick.head_y*player.unit->udata->max_y_speed; //vertikale Drift

       player.unit->wink_soll.x = stick.axis[0]*player.unit->udata->maxrotationspeed;  //X-Achsen-Rotation
       if(stick.b[3])
       {
          player.unit->wink_soll.z = stick.axis[1]*player.unit->udata->maxrotationspeed; //Z-Achsen-Rotation
          player.unit->wink_soll.y = 0;
       }
       else
       {
          player.unit->wink_soll.y = stick.axis[1]*player.unit->udata->maxrotationspeed;  //Y-Achsen-Rotation
          player.unit->wink_soll.z = 0;
       }
	   if (stick.b[0])
	   {
	       raptor_start_shot(player.unit);
       }
       if (stick.b[1] && ((chrono-player.unit->shotdelay) > MISSILESHOOTRATE))
       {
          if (i=raptor_fire_missile(player.unit, player.unit->missile))
          {       
             start_missile(player.unit, i, player.unit->missile);
             player.unit->shotdelay = chrono;
          }
       }
       if (stick.b[3])
       {
          if (!buttn_pressed)
          {
              buttn_pressed=TRUE;
              buttn_delay=chrono;
          }
       }
       else if(buttn_pressed)
       {
          buttn_pressed=FALSE;   
          if ((chrono-buttn_delay)<300)       
          {
             player.unit->target=target_unit_near_reticule();
             player.target_monitor_interference=chrono+120;
             sound.play("button_02", 24, 128, 1000, 0);
             buttn_delay=chrono-300;
          }
       }
    }
    else
    {
    if(key[KEY_UP])
    {
        player.unit->wink_soll.x=-50;
    }
    else if(key[KEY_DOWN])
    {
        player.unit->wink_soll.x=50;
    }
    else player.unit->wink_soll.x=0;
            	 
    if(key[KEY_LEFT])
    {
        player.unit->wink_soll.y=50;
        player.unit->wink_soll.z=15;
    }
    else if(key[KEY_RIGHT])
    {
        player.unit->wink_soll.y=-50;
        player.unit->wink_soll.z=-15;
    }
    else
    {
        player.unit->wink_soll.y=0;     	 
        player.unit->wink_soll.z=0;     	 
    }
    if (key[KEY_DEL])
    {
       player.unit->speed_soll.x =  player.unit->udata->max_x_speed; //seitliche Drift
    }
    else if(key[KEY_PGDN])
    {
       player.unit->speed_soll.x = -player.unit->udata->max_x_speed; //seitliche Drift
    }
    else
    {
       player.unit->speed_soll.x = 0;
    }
    if (key[KEY_HOME])
    {
       player.unit->speed_soll.y =  -player.unit->udata->max_y_speed; //vertikale Drift
    }
    else if (key[KEY_END])
    {
       player.unit->speed_soll.y =  player.unit->udata->max_y_speed; //vertikale Drift
    }
    else
    {
       player.unit->speed_soll.y = 0;
    }
    }

    clear_keybuf();
}



void proceed_player_ship()
{
    VECTOR pos_delta;

    player.zero_m=player.unit->m;  //erstelle unit-Matrix ohne Translation
    zero_matrix(&player.zero_m);
      
//    invert_matrix(&player.unit->m, &player.camera.m);   //erstelle  Kamera-Matrix
//    zero_matrix(&player.camera.m);
 z   
//    apply_matrix_v(&player.camera.m, (VECTOR*)&player.camera.offset.t[0], &player.camera.pos); //hole die neue Nasenwurzel-Position
//    vector_add(&player.camera.pos, (VECTOR*)&player.unit->m.t[0], &player.camera.pos);
//    player.camera.pos+=(VECTOR)player.unit->m.t[0];
      get_matrix_delta(&player.camera.m, &player.camera.m_old, &player.camera.m_delta);  //hole Differenz zwischen alter Kamera-Position und neuer (wichtig für Debris)
    apply_matrix_v(&player.zero_m, &player.unit->flightdir, (VECTOR*)&player.camera.m_delta.t[0]);
    vector_mul((VECTOR*)&player.camera.m_delta.t[0], fmult);
    
    player.camera.m_old=player.camera.m; //die aktuelle Kamera wird zur alten Kamera

    sound.adjust(engsound, player.unit->speed/2+7, 128, player.unit->speed*12+700, TRUE);    
        if(player.unit->target!=NO_TARGET)  //errechne Vorhalt und Zielkreuz
        {
           player.distance_to_target=distance(&player.camera.pos, (VECTOR*)&player.unit->target->m.t[0]);
           player.time_to_impact=time_to_impact(player.unit, player.unit->target);
           calc_vorhalt((VECTOR*)&player.vorhalt.t[0], player.unit->target, player.time_to_impact);
           if(player.target_is_visible=spot_is_visible(&player.unit->target->heading, 0))
           {
             if(player.gunhits=check_aiming(player.unit, "guncalc"))
              {
                 player.unit->target->udata->model.colmodel->getCollisionPoint (&player.hitpoint.x, 0);
                 player.distance_to_target=distance(&player.camera.pos, &player.hitpoint);
              }
           }
        }
    if(player.unit->speed!=0)
    {  //errechne die Flugrichtung relativ zum Spieler-Schiff
	   apply_matrix_v(&player.zero_m, &player.unit->flightdir, &player.norm_flightdir);
	   normalize_vector_v(&player.norm_flightdir);
    }
    else
    {  //bei speed=0 ist die Flugrichtung "vorwärts"
       player.norm_flightdir=trans_matrix_to_vector(&player.unit->m);
    }
}


void refresh_unit_energy(UNIT *u)
{
    accelerate(100, &u->gunpower[0], 5);
    accelerate(100, &u->gunpower[1], 5);
    accelerate(100, &u->shield, 1);
}

void proceed_units()
{
    int u;
    MATRIX_f m;
	for (u=0; u<unit_anz; u++)
	{
	   if (unit[u]->justhit) if(chrono-unit[u]->justhit > 100)unit[u]->justhit=0;
       switch(unit[u]->udata->type)
       {
          case UNIT_FIGHTER:
          if(unit[u]->target!=NO_TARGET)
          {
             unit[u]->gunangel=RAD_2_256(atan(unit[u]->udata->gun_x_offset/unit[u]->target->dist));
             if(unit[u]->gunangel>unit[u]->udata->gun_max_angel)unit[u]->gunangel=unit[u]->udata->gun_max_angel;
          }
          else unit[u]->gunangel=0;
//          proceed_engine_outline(unit[u]);
/*          if(unit[u]->speed_soll.z > 10)
          {
             get_object_matrix_world(&m, u, unit[u]->udata->engine_obj);
                 flush.start((VECTOR*)&m.t[0], 0);
 //            start_engine_outline(unit[u]);
             unit[u]->enginetimer=chrono;
          } */
          break;
       } 	   
       switch(unit[u]->status)
       {
          case AI_PLAYER:
          turn_unit(unit[u]);
          move_ship(unit[u]);
          proceed_player_ship();
          refresh_unit_energy(unit[u]);
          break;
          
          
          case AI_ATTACKING:
          align_target(unit[u], 30);
          turn_unit(unit[u]);
          fire_at_target(unit[u]);
          move_ship(unit[u]);
          refresh_unit_energy(unit[u]);
          break;
          
          case AI_SEEKING:
          align_target(unit[u], MISSILETURN);
          turn_unit(unit[u]);
          move_ship(unit[u]);
    /*      if(unit[u]->udata->model.engine)
          {
             proceed_engine_outline(u);
             if((chrono-unit[u]->enginetimer) > 100)
             {
                start_engine_outline(u);
                unit[u]->enginetimer=chrono;
             }
          }
     */
//         flush.start((VECTOR*)&unit[u]->m.t[0], 0);

          if((chrono-unit[u]->t)>MISSILETIMER)
          {
             explosion2.start(&unit[u]->m, &unit[u]->speed, unit[u]->property);
             kill_unit(u);
  //           u--;
          }
          break;
          
          case AI_DUMBFIRE:
          move_ship(unit[u]);
//          flush.start((VECTOR*)&unit[u]->m.t[0], 0);
          if((chrono-unit[u]->t)>MISSILETIMER)
          {
             explosion2.start(&unit[u]->m, &unit[u]->speed, unit[u]->property);
             kill_unit(u);
  //           u--;
          }
          break;
          case AI_JUSTFLYING:
          turn_unit(unit[u]);
          move_ship(unit[u]);
          if((chrono-unit[u]->t)>MISSILETIMER)
          break;
       }
    }
}



void init_3d_world()
{
	unit[unit_anz]->name="Martin";
	unit[unit_anz]->status=AI_PLAYER;
	unit[unit_anz]->m=matr(800, 11, -17, 127, 60, 127);
	unit[unit_anz]->target = unit[1];
	unit[unit_anz]->shield=100;
	unit[unit_anz]->hull=100;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=1;
	unit[unit_anz]->speed_soll.z=0;
	unit[unit_anz]->speed=1;
	unit[unit_anz]->udata=&raptor;
	unit[unit_anz]->gunpower[0]=90;
	unit[unit_anz]->gunpower[1]=90;
   prepare_raptor(unit[unit_anz]);
   unit_anz++;

 	unit[unit_anz]->name="testeinheit 1";
	unit[unit_anz]->status=AI_JUSTFLYING;
	unit[unit_anz]->m=matr(-80, 5, 50, 0, 10, 10);
	unit[unit_anz]->speed_soll.z=0;
	unit[unit_anz]->wink_soll.y=10;
	unit[unit_anz]->target = unit[0];
	unit[unit_anz]->shield=80;
   unit[unit_anz]->hull=100;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
	unit[unit_anz]->udata=&raptor;
   prepare_raptor(unit[unit_anz]);
   unit_anz++;

    
 	unit[unit_anz]->name="testeinheit 2";
	unit[unit_anz]->status=AI_JUSTFLYING;
	unit[unit_anz]->m=matr(-750, 15, 75, 0, -64, 0);
	unit[unit_anz]->speed_soll.z=26;
	unit[unit_anz]->target = unit[1];
	unit[unit_anz]->shield=80;
    unit[unit_anz]->hull=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=2;
	unit[unit_anz]->udata=&raptor;
	unit[unit_anz]->gunpower[0]=90;
	unit[unit_anz]->gunpower[1]=90;
    prepare_raptor(unit[unit_anz]);
    unit_anz++; 
 
   	unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->udata = &asteroid00;
	unit[unit_anz]->m=matr(600, -15, -15, 0, 0, 127);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	prepare_object_matrices(unit[unit_anz]);
//    spin(unit[unit_anz], "base", Z_AXIS, 10);
	unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->udata = &asteroid01;
	unit[unit_anz]->m=matr(1200, -10, 0, 0, 0, 0);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	prepare_object_matrices(unit[unit_anz]);
//	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->udata = &asteroid01;
	unit[unit_anz]->m=matr(-1000, 30, 40, 0, 0, 0);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	prepare_object_matrices(unit[unit_anz]);
	spin(unit[unit_anz], "base", Z_AXIS, 15);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->udata = &asteroid02;
	unit[unit_anz]->m=matr(-900, 20, 200, 0, 0, 64);
 	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	prepare_object_matrices(unit[unit_anz]);
//	spin(unit[unit_anz], "base", Y_AXIS, 10);
    unit_anz++;
//
//    unit[unit_anz]->name="Asteroid";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &asteroid00;
//	unit[unit_anz]->m=matr(0, 10, 800, 0, 0, 20);
//	unit[unit_anz]->vulnerable=0;
//	unit[unit_anz]->party=0;
//	prepare_object_matrices(unit[unit_anz]);
////	spin(unit[unit_anz], "base", Z_AXIS, 10);
//    unit_anz++;
//
//    unit[unit_anz]->name="Asteroid";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &asteroid01;
//	unit[unit_anz]->m=matr(280, 300, -1600, 0, 0, 20);
//	unit[unit_anz]->vulnerable=0;
//	unit[unit_anz]->party=0;
//	prepare_object_matrices(unit[unit_anz]);
////	spin(unit[unit_anz], "base", Z_AXIS, 10);
//    unit_anz++;

    unit[unit_anz]->name="level";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->udata = &level;
	unit[unit_anz]->m=matr(-1000, 0, 63, 0, 0, 0);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	prepare_object_matrices(unit[unit_anz]);
	hide_grid(unit[unit_anz], "roof");
	hide_grid(unit[unit_anz], "floor");
	hide_grid(unit[unit_anz], "nooutlines");
	hide_grid(unit[unit_anz], "light1");
	hide_grid(unit[unit_anz], "light2");
	hide_grid(unit[unit_anz], "light3");
//	hide_grid(unit[unit_anz], "");
//	hide_grid(unit[unit_anz], "");
   hide_fill(unit[unit_anz], "art");
   hide(unit[unit_anz], "light1");
   hide(unit[unit_anz], "light2");
   hide(unit[unit_anz], "light3");
	animate_light(unit[unit_anz]);
   unit_anz++;
//    
//    unit[unit_anz]->name="speicher";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &speicher;
//	unit[unit_anz]->m=matr(-7.5, 0, 15, 0, 64, 128);
//	unit[unit_anz]->vulnerable=1;
//	unit[unit_anz]->party=0;
//    unit[unit_anz]->hull=100;
//	prepare_object_matrices(unit[unit_anz]);
//    unit_anz++;
//    unit[unit_anz]->name="speicher";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &speicher;
//	unit[unit_anz]->m=matr(-7.5, -2.3, 15, 0, 64, 128);
//	unit[unit_anz]->vulnerable=1;
//	unit[unit_anz]->party=0;
//    unit[unit_anz]->hull=100;
//	prepare_object_matrices(unit[unit_anz]);
//    unit_anz++;
//


//    unit[unit_anz]->name="flea";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &flea;
//	unit[unit_anz]->m=matr(-7.5, 100, 25, 0, 64, 128);
//	unit[unit_anz]->vulnerable=1;
//	unit[unit_anz]->party=0;
//    unit[unit_anz]->hull=100;
//	prepare_object_matrices(unit[unit_anz]);
//    spin(unit[unit_anz], "turret", Y_AXIS, 10);
//    unit_anz++;

//    unit[unit_anz]->name="speicher";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &speicher;
//	unit[unit_anz]->m=matr(-7.5, -2.3, 25, 0, 64, 128);
//	unit[unit_anz]->vulnerable=1;
//	unit[unit_anz]->party=0;
//    unit[unit_anz]->hull=100;
//	prepare_object_matrices(unit[unit_anz]);
//    unit_anz++;
    
//    unit[unit_anz]->name="test-scheibe";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &asteroid03;
//	unit[unit_anz]->m=matr( -40, -15, 45, 64, 0, 0);
//	unit[unit_anz]->vulnerable=0;
//	unit[unit_anz]->party=0;
//   unit[unit_anz]->hull=100;
//	prepare_object_matrices(unit[unit_anz]);
//   unit_anz++;
//   unit[unit_anz]->name="test-scheibe";
//	unit[unit_anz]->status=AI_NOTHING;
//	unit[unit_anz]->udata = &asteroid03;
//	unit[unit_anz]->m=matr( -60, -15, 45, 0, 32, 0);
//	unit[unit_anz]->vulnerable=0;
//	unit[unit_anz]->party=0;
//   unit[unit_anz]->hull=100;
//	prepare_object_matrices(unit[unit_anz]);
//   unit_anz++;

}

void init_player_data()
{
	 player.far_clip             =30000;
    player.aufl_w               =  1024;
    player.aufl_h               =  768;
    player.farbtiefe            =   32;
    player.z_puffer             =   32;
    
//    stereo.set_monitor_height(0.1975);
//    stereo.set_monitor_width(0.32);

    player.wait_for_release     = FALSE;
    player.keyboard_was_pressed = FALSE;
    player.unit=unit[0];
    
    player.bg_is_drawn          = TRUE;
    player.debris_is_drawn      = TRUE;

	player.camera.m             = identity_matrix_f;
	player.camera.m_delta       = identity_matrix_f;
	player.camera.m_old         = identity_matrix_f;
	player.unit->rendered       = RND_NOT_RENDERED;
	player.camera.perspective   = CAMERA_COCKPIT;
}

void init_unit_data()
{
    the_raptor(&raptor);
    the_seeker_missile(&seeker_missile);
    the_dumb_missile(&dumb_missile);
    
   lese_3do(&cockp,  "3do/coc3d.3do");
   
   lese_3do(&asteroid00.model,  "3do/asteroid01.3do");
	lese_3do(&asteroid01.model,  "3do/asteroid02.3do");
	lese_3do(&asteroid02.model, "3do/asteroid03.3do");
	lese_3do(&asteroid03.model, "3do/testscheibe.3do");
	asteroid00.col_model=asteroid00.model;
	asteroid01.col_model=asteroid01.model;
	asteroid02.col_model=asteroid02.model;
	asteroid03.col_model=asteroid03.model;
	
	init_collision_model(&asteroid00.model, &asteroid00.model.o3d[0]);
	init_collision_model(&asteroid01.model, &asteroid01.model.o3d[0]);
	init_collision_model(&asteroid02.model, &asteroid02.model.o3d[0]);

	init_collision_model(&asteroid03.model, &asteroid03.model.o3d[0]);
	asteroid00.type=UNIT_OBJECT;
	asteroid01.type=UNIT_OBJECT;
	asteroid02.type=UNIT_OBJECT;
	asteroid03.type=UNIT_OBJECT;
	
	flea.type=UNIT_OBJECT;

   	lese_3do(&speicher.model,  "3do/speicher.3do");
   	lese_3do(&speicher.col_model,  "3do/speicher_col.3do");
	speicher.type=UNIT_OBJECT;
	init_collision_model(&speicher.model, &speicher.col_model.o3d[0]);
	
	lese_3do(&level.model,  "3do/level5.3do");
   	lese_3do(&level.col_model,  "3do/level5_col.3do");
	level.type=UNIT_LEVEL;
	init_collision_model(&level.model, &level.col_model.o3d[0]);


}

void init_gfx()
{
	allegro_gl_clear_settings();
	set_color_depth(player.farbtiefe);
	allegro_gl_set (AGL_COLOR_DEPTH, player.farbtiefe);
	allegro_gl_set (AGL_Z_DEPTH, player.z_puffer);
	allegro_gl_set (AGL_DOUBLEBUFFER, 1);
	allegro_gl_set (AGL_RENDERMETHOD, 1);
	allegro_gl_set (GFX_OPENGL_FULLSCREEN, TRUE);
	allegro_gl_use_mipmapping(FALSE);
 	if (set_gfx_mode(GFX_OPENGL, player.aufl_w, player.aufl_h, 0, 0) != 0)
	{
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message ("Error setting OpenGL graphics mode:\n%s\nAllegro GL error : %s\n", allegro_error, allegro_gl_error);
		exit(1);
	}
	glPolygonMode(GL_FRONT, GL_FILL);            //Fülle die sichtbaren Polygone aus
    glPolygonMode(GL_BACK, GL_POINT);            //errechne für die unsichtbaren Polys nur die Punkte
	glCullFace(GL_BACK);                         //zeichne nur die Vorderseite der Polygone
	glEnable(GL_CULL_FACE);                      //aktiviere BackFaceCulling
	glFrontFace(GL_CCW);                         //die sichtbaren Polygone sind die linksgedrehten
	glShadeModel(GL_FLAT);                       //heißt: kein Gouraud-Shading
	glAlphaFunc(GL_GREATER, 0.5);                //Pixel mit einem Alpha unter 0.5 werden ignoriert
//	glClearColor(0.156, 0.156, 0.156, 1.0);      //Hintergrundfarbe

//	glClearColor4ub(0.156, 0.156, 0.156, 255);

    glEnableClientState (GL_VERTEX_ARRAY);       //wird noch nicht gebraucht
    glBlendFunc(GL_ONE, GL_ONE);                 //Blending-Methode. RGB-Werte werden zusammenaddiert 

    glEnable (GL_LINE_SMOOTH);                           //Schaltet Linien-Antialiasing ein
    glEnable (GL_POINT_SMOOTH);                          //Schaltet Punkt-Antialiasing ein  
    glEnable(GL_BLEND);                                  //Schaltet Blenden-Effekte ein
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  //aktiviert den Alpha-Kanal
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);          //?
    glLineWidth (0.2);                                   //Linienstärke = 0.2 Ppixel...
    
}

void game_loop()
{

    while (!key[KEY_ESC])				
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
		proceed_music();
        proceed_actions();
		shot.proceed();
		explosion2.proceed();
		explosion1.proceed();
 		flush.proceed();
 		
        user_input();
		collision_detection();
		proceed_units();

		if(player.debris_is_drawn)debris.proceed(&player.camera.m_delta);
		get_headings();
		calc_distances();
		render_selection();
		render();

	}
}

int main(void)
{
    NO_TARGET->index=-1;
	allegro_init();
	install_allegro_gl();
	install_keyboard();
	set_keyboard_rate(1, 10);
	init_pointers();
    init_actions(&chrono, &fmult);


	init_player_data();
	init_unit_data();

	init_gfx();
//	init_color_table();
    init_mathematics(&fmult);
    init_drawprim();
    init_fx(&fmult, &chrono);
//	lese_3do(&mod_explosions, "3do/explosions.3do");
	init_3d_world();

    player.camera.camera_object=get_object_by_name(&player.unit->udata->model, "camera");
	player.camera.offset=get_object_matrix(player.unit, player.camera.camera_object);

	install_int(the_timer, 1);
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);
	init_stick(&stick);
	load_bitmaps();
	init_sinus10();

	stereo.init_lighting();
	debris.init("3do/debris01.3do", 48, 4, 150);
	explosion1.init("3do/flare2.3do", 12);
	explosion2.init(32, &player.zero_m);
	background.init("3do/box8.3do", &player.camera.m);
	flush.init(&player.zero_m);
	shot.init("3do/laser.3do");
	init_cd(&fmult);
   init_sound(&player);
	load_samples();
   init_highways();
   sound.play("thrust", 24, 128, 1000, 1);
   fcount=chrono;
//   if(allegro_gl_is_extension_supported ("GL_ARB_vertex_buffer_object"))
//    allegro_message("vertex buffer object is supported");
//    allegro_message("%d", allegro_gl_get_proc_address("GL_ARB_vertex_buffer_object"));

    ///////////
    game_loop();
    ///////////
//    delete sound();
    terminate_sound();
    destroy_bitmaps();
	allegro_exit();
	return 0;
}
END_OF_MAIN();

