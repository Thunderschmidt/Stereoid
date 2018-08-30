/* Action Commander Glossar
 * u: unit
 * o: object
 * p: polygon
 * v: vertex
 * i: index
 * n: normal vector
 * r: rechts
 * l: links
 * m: allegro-matrix
 * pos: position vector
 * vo: Vektor nach vorne
 * re: Vektor nach rechts
 * a: acceleration
 * t: time
 * c: color
 * RL: render list
 */


//#define MUSIK

#include <aldumb.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <allegro.h>
#include <alleggl.h>
//#include <GL/glext.h>
#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "input3DO.h"
//#include "inputINI.h"

#include "y1.h"
#include "actions.h"
#include "mat.h"
#include "drawprim.h"
#include "stick.h"
#include "cd.h"
#include "ships.h"

//#include "Trans_3D.h"
/*
void init_color_table()
{
   int i;
   int buf[MAX_POINTS];
   ColorConversion colortable; 
   colortable.LoadConf("editor.conf");
   for(i=0; i<16; i++)
   {
      //links
      col[0][i][0]=colortable.GetValue(SIDE_LEFT, COLOR_RED, i*16);
      col[0][i][1]=colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i*16);
      col[0][i][2]=colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i*16);
      colortable.GetLum(SIDE_LEFT, COLOR_BLUE, buf);
      printf("links: %d %d %d \n",colortable.GetValue(SIDE_LEFT, COLOR_RED, i*16),colortable.GetValue(SIDE_LEFT, COLOR_GREEN, i*16),colortable.GetValue(SIDE_LEFT, COLOR_BLUE, i*16));
      printf("links: %d %d %d \n",col[0][i][0],col[0][i][1],col[0][i][2]);
//      printf("links: %d %d %d \n",col[0][i][0],col[0][i][1],col[0][i][2]);
      //rechts
      col[1][i][0]=colortable.GetValue(SIDE_RIGHT, COLOR_RED, i*16);
      col[1][i][1]=colortable.GetValue(SIDE_RIGHT, COLOR_GREEN, i*16);
      col[1][i][2]=colortable.GetValue(SIDE_RIGHT, COLOR_BLUE, i*16);
   }
}
*/  

int prepare_object_matrices(short u)
{
    short o;
    unit[u]->object=(OBJECT*)malloc(unit[u]->model->obj_anz*sizeof(OBJECT));
    for(o=0;o<unit[u]->model->obj_anz;o++)
    {
       unit[u]->object[o].m=identity_matrix_f;
       unit[u]->object[o].moved=FALSE;
       unit[u]->object[o].visible=unit[u]->model->o3d[o].visible;
       unit[u]->object[o].winkel[0]=0;
       unit[u]->object[o].winkel[1]=0;
       unit[u]->object[o].winkel[2]=0;
    }
}


/*
void init_background()
{
    bool b;
    int i, p;
    int  t1, t2;
    CollisionModel3D* icosahedron_col;
    icosahedron_col = newCollisionModel3D();
    for(p=0;p<20;p++)
    icosahedron_col->addTriangle(&icosahedron_v[icosahedron_i[p][0]][0],
                                 &icosahedron_v[icosahedron_i[p][1]][0],
                                 &icosahedron_v[icosahedron_i[p][2]][0]);
    icosahedron_col->finalize();
    for (i=0;i<20;i++)
    {
       for (p=0; p<10;p++)
       {
             sterne[i][p].x=rand()%10000-5000;
             sterne[i][p].y=rand()%10000-5000;
             sterne[i][p].z=rand()%10000-5000;
             normalize_vector_f(&sterne[i][p].x, &sterne[i][p].y, &sterne[i][p].z);
             printf("%d \n", icosahedron_col->rayCollision (&null.x, &sterne[i][p].x, 0));

             b=icosahedron_col->getCollidingTriangles(t1, t2); 
             printf("%d %d %d %d\n", t1, t2, p, b);
        }
    }
}

*/

void init_pointers()
{
   short i;
   for(i=0; i<MAXUNITS; i++)
   {
       unit[i]=&units[i];
       units[i].index=i;
   }
   for(i=0; i<MAXMODELS; i++)
   {
       model[i]=&models[i];
       models[i].index=i;
   }
   for(i=0; i<MAXSHOTS; i++)
   {
       shot[i]=&shots[i];
       shots[i].index=i;
   }
   for(i=0; i<MAXEXPLOSIONS; i++)
   {
       explosion[i]=&explosions[i];
   }
   for(i=0; i<MAXJETS; i++)
   {
       jet[i]=&jets[i];
   }
}

void kill_shot(short i)
{
   SHOT *temp;
   temp=shot[i];
   shot[i]=shot[shot_anz-1];
   shot[shot_anz-1]=temp;
   shot[shot_anz-1]->index=shot_anz-1;
   shot[i]->index=i;
   shot_anz--;
}

void kill_explosion(short i)
{
   EXPLOSION *temp;
   temp=explosion[i];
   explosion[i]=explosion[explosion_anz-1];
   explosion[explosion_anz-1]=temp;
   explosion_anz--;
}

void init_stars()
{
    int i;
    for(i=0; i<MAXSTARS; i++)
    {
       do
       {
          stars[i].x=rand()%10000-5000;
          stars[i].y=rand()%10000-5000;
          stars[i].z=rand()%10000-5000;
       }
       while((stars[i].x*stars[i].x+stars[i].y*stars[i].y+stars[i].z*stars[i].z)>25000000);
       normalize_vector_f(&stars[i].x, &stars[i].y, &stars[i].z);    
    }
}

void draw_stars(bool c)
{
    glColor3ubv(col[c][9]);
    glVertexPointer(3, GL_FLOAT, 0, &stars[0].x);
    glDrawArrays(GL_POINTS, 0, MAXSTARS);
}
    
bool get_object_matrix_recursive(short u, short o, MATRIX_f *m, short o_soll, VECTOR *pos)
{
    pos->x+=unit[u]->model->o3d[o].pos.x;       
    pos->y+=unit[u]->model->o3d[o].pos.y;       
    pos->z+=unit[u]->model->o3d[o].pos.z;
    if(o==o_soll)
    {
       m->t[0]=pos->x;       
       m->t[1]=pos->y;       
       m->t[2]=pos->z;
    }
    else
    {
    if(unit[u]->model->o3d[o].child_o) get_object_matrix_recursive(u, unit[u]->model->o3d[o].child_o, m, o_soll, pos);
    pos->x-=unit[u]->model->o3d[o].pos.x;       
    pos->y-=unit[u]->model->o3d[o].pos.y;       
    pos->z-=unit[u]->model->o3d[o].pos.z;
    if(unit[u]->model->o3d[o].sibl_o) get_object_matrix_recursive(u, unit[u]->model->o3d[o].sibl_o,  m, o_soll, pos);
    }
}

MATRIX_f get_object_matrix(short u, short o_soll)
{
    MATRIX_f m=identity_matrix_f;
    VECTOR pos=null;
    get_object_matrix_recursive(u, 0, &m, o_soll, &pos);
    return(m);
}

void init_sound()
{
    if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
       printf ("Error initializing sound card");
    }
    snd_shieldhit = load_wav ("sounds/snd_shieldhit.wav");
    snd_hullhit   = load_wav ("sounds/snd_hullhit.wav");
    	
    sound[0] = load_wav ("sounds/hum.wav");
    sound[1] = load_wav ("sounds/lasfirerb.wav");
    sound[4] = load_wav ("sounds/tirantarionmissile.wav");
    sound[5] = load_wav ("sounds/TTaDth00.wav");
    sound[6] = load_wav ("sounds/vdu_a.wav");
    sound[7] = load_wav ("sounds/wolf_fire_hit.wav");
    sound[8] = load_wav ("sounds/katakis12.wav");
    sound[9] = load_wav ("sounds/Smash_TV_rakete.wav");
    sound[10] = load_wav ("sounds/c64explode.wav");
   
}

void destroy_samples()
{
    int i;
    for (i=0; i<12; i++) destroy_sample(sound[i]);
}

void load_bitmaps (void)
{
	logo[0]=create_video_bitmap(35, 30);
	logo[0] = load_bitmap ("bmp/logo1.bmp", NULL);
	logo[1]=create_video_bitmap(35, 30);
	logo[1] = load_bitmap ("bmp/logo2.bmp", NULL);
	logo[2]=create_video_bitmap(35, 30);
	logo[2] = load_bitmap ("bmp/logo3.bmp", NULL);
	rauschen=create_video_bitmap(256, 256);
	rauschen=load_bitmap ("bmp/rauschen.bmp", NULL);
}

void destroy_bitmaps()
{
			destroy_bitmap(logo[0]);
			destroy_bitmap(logo[1]);
			destroy_bitmap(logo[2]);
			destroy_bitmap(rauschen);
			destroy_bitmap(bmp);
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
int check_col(MATRIX_f *m1, MATRIX_f *m2, float bounding)
{
    if (((m1->t[0]-m2->t[0])<bounding) && ((m1->t[0]-m2->t[0])> -bounding))
    if (((m1->t[1]-m2->t[1])<bounding) && ((m1->t[1]-m2->t[1])> -bounding))
    if (((m1->t[2]-m2->t[2])<bounding) && ((m1->t[2]-m2->t[2])> -bounding))
    return(1);
    return(0);
}

void get_next_target(short u)
{
    if(unit[u]->target->index < (unit_anz-1)) unit[u]->target=unit[unit[u]->target->index+1];
    else unit[u]->target=player.unit;
    if(unit[u]->target == unit[u] && (unit_anz>1)) get_next_target(u);
}

void get_last_target(short u)
{
    if(unit[u]->target->index >0) unit[u]->target=unit[unit[u]->target->index-1];
    else unit[u]->target=unit[unit_anz-1];
    if(unit[u]->target == unit[u] && (unit_anz>1)) get_last_target(u);
}

UNIT *get_nearest_enemy_target(short u)
{
    short i, ret=NONE; float dist0, dist1=0;
    for (i=0; i<unit_anz; i++)
    if ((u!=i) && (unit[i]->party!=unit[u]->party) && unit[i]->party!=0)
    {
        dist0=1/vector_length_f(unit[u]->m.t[0]-unit[i]->m.t[0],
                                unit[u]->m.t[1]-unit[i]->m.t[1],
                                unit[u]->m.t[2]-unit[i]->m.t[2]);
        if(dist0>dist1)
        {
           ret=i;
           dist1=dist0;
        }
    }
    if(ret==NONE)return(NO_TARGET);
    else return(unit[ret]);
}


void init_new_unit()
{
   
   unit[unit_anz]->model = model[0];
   unit[unit_anz]->m =identity_matrix_f;
   get_x_rotate_matrix_f(&unit[unit_anz]->m, 128);
   unit[unit_anz]->m.t[0] = 300;
   unit[unit_anz]->m.t[1] = 300;
   unit[unit_anz]->speed_soll.z=50;
   unit[unit_anz]->speed=0;
   unit[unit_anz]->target = 0;
   unit[unit_anz]->shield=100;
   unit[unit_anz]->status=AI_ATTACKING;
   unit[unit_anz]->type=UNIT_FIGHTER;
   unit[unit_anz]->vulnerable=1;
   unit[unit_anz]->party = 1;
   unit_anz++;
}

float time_to_impact(short u)
{
    float dist, t, appspeed;  
    VECTOR heading;
    get_heading(&unit[u]->m, &player.unit->m, &heading);
    appspeed=heading.z*unit[u]->speed;

    dist=vector_length_f(unit[u]->m.t[0]-player.unit->m.t[0],
                         unit[u]->m.t[1]-player.unit->m.t[1],
                         unit[u]->m.t[2]-player.unit->m.t[2]);
    t=dist/(player.unit->speed+SHOTSPEED+appspeed);
    return(t);
}

void calc_vorhalt(short u, float t)
{
    player.vorhalt.t[0]=unit[u]->m.t[0]-unit[u]->m.v[2][0]*unit[u]->speed*t;
    player.vorhalt.t[1]=unit[u]->m.t[1]-unit[u]->m.v[2][1]*unit[u]->speed*t;
    player.vorhalt.t[2]=unit[u]->m.t[2]-unit[u]->m.v[2][2]*unit[u]->speed*t;
}


//Render-Selektion
#define XHEADING 0.333
#define YHEADING 0.183

bool unit_is_visible(UNIT *u)
{
   float dist=0;
   if (u->heading.z > 0)
   {
      dist=distance(&player.camera.pos, (VECTOR*)&u->m.t[0]);         
      dist=u->model->size /dist;
      if (u->heading.x < (XHEADING+dist))
      if (u->heading.x >-(XHEADING+dist))
      if (u->heading.y < (YHEADING+dist))
      if (u->heading.y >-(YHEADING+dist))
      {    
         return(TRUE);
      }
   }
   return(FALSE);
}

bool spot_is_visible(VECTOR *heading)
{
   float dist=0;
   if (heading->z > 0)
   {
      if (heading->x < (XHEADING+dist))
      if (heading->x >-(XHEADING+dist))
      if (heading->y < (YHEADING+dist))
      if (heading->y >-(YHEADING+dist))
      {    
         return(TRUE);
      }
   }
   return(FALSE);
}

void make_RL()
{
    short i=0, i2=0; float dist;
    for(i=1; i<unit_anz; i++)
    if(unit_is_visible(unit[i]))
    {
       RL.u.i[i2]=i;
       i2++;
    }
    RL.u.anz=i2; i2=0;
    for(i=0; i<shot_anz; i++)
    {
       if (spot_is_visible(&shot[i]->heading))
       {
          RL.shot.i[i2]=i;
          i2++;
       }
    }
    
    RL.shot.anz=i2; i2=0;
    /*
    for(i=0; i<explosion_anz; i++)
    {
       if (explosion[i]->heading.z > 0)
       {
             if (explosion[i]->heading.x < XHEADING)
             if (explosion[i]->heading.x >-XHEADING)
             if (explosion[i]->heading.y < YHEADING)
             if (explosion[i]->heading.y >-YHEADING)
             {    
                RL.explosion.i[i2]=i;
                i2++;
             }
       }
    }
    RL.explosion.anz=i2; i2=0;
    */
}

void get_headings()
{
    int i;
    for (i=1; i<unit_anz; i++)
    {
       get_heading(&player.unit->m, &unit[i]->m, &unit[i]->heading);
    }
    for (i=0; i<shot_anz; i++)
    {
       get_heading(&player.unit->m, &shot[i]->m, &shot[i]->heading);
    }
}

UNIT *target_unit_near_reticule()
{
    short u, nu=0; VECTOR heading; float minheading=2;
    for (u=0; u<RL.u.anz; u++)
    {
           heading=unit[RL.u.i[u]]->heading;
           if (heading.z>0)
           {
              if (heading.x<0)heading.x*=-1;
              if (heading.y<0)heading.y*=-1;
              if ((heading.x+heading.y)<minheading)
              {
                 minheading=heading.x+heading.y;
                 nu=RL.u.i[u];
              }
           }
    }
    return(unit[nu]);
}   
    
void play_3D_sound(float x, float y, float z, SAMPLE *sample)
{
    VECTOR heading;
    MATRIX_f m;
    float dist;
    get_translation_matrix_f(&m, x, y, z);
    get_heading(&player.unit->m, &m, &heading);
    dist=vector_length_f(player.camera.pos.x-x, player.camera.pos.y-y, player.camera.pos.z-z);            
    if (dist < 400)play_sample (sample, 2048/(dist+32), 128+heading.x*-128, 1000, 0);
}

void play_3D_sound_v(VECTOR *pos, SAMPLE *sample)
{
    VECTOR heading;
    MATRIX_f m;
    float dist;
    get_translation_matrix_f(&m, pos->x, pos->y, pos->z);
    get_heading(&player.unit->m, &m, &heading);
    dist=distance(&player.camera.pos, pos);
    if (dist < 400)play_sample (sample, 2048/(dist+32), 128+heading.x*-128, 1000, 0);
}



void init_explosion()
{
    int i;
   	for (i=0; i<64; i++)
	{
	   partikel[i].x=0; 
	   while (((partikel[i].x*partikel[i].x + 
                partikel[i].y*partikel[i].y +
                partikel[i].z*partikel[i].z  ) > 262144) || !partikel[i].x)
	   {
		  partikel[i].x = rand() % 1024 - 512;
		  partikel[i].y = rand() % 1024 - 512;
		  partikel[i].z = rand() % 1024 - 512;
	   }
	   normalize_vector_f(&partikel[i].x, &partikel[i].y, &partikel[i].z);
	}
}

void start_explosion(MATRIX_f *m, float *speed, UNIT *property)
{
    int  j;
    explosion[explosion_anz]->pos.x=m->t[0];
    explosion[explosion_anz]->pos.y=m->t[1];
    explosion[explosion_anz]->pos.z=m->t[2];
    explosion[explosion_anz]->t=chrono;
    explosion[explosion_anz]->mult=0.1;
    explosion[explosion_anz]->anz=64;
    explosion[explosion_anz]->m=*m;
    explosion[explosion_anz]->speed=*speed;
    explosion[explosion_anz]->property=property;
    for(j=0;j<explosion[explosion_anz]->anz;j++)
    {
       explosion[explosion_anz]->partikel[j]=rand() % 1024;
       explosion[explosion_anz]->partikel[j]/=2048;
    }
    explosion_anz++;
}

void draw_explosion(int j, char c)
{
    int i;
    float mult;
   	glColor3ubv(col[c][14]);
   	for (i=0; i<explosion[j]->anz;i++)
   	{
   	   mult=explosion[j]->partikel[i]*explosion[j]->mult;
   	   glTranslatef(partikel[i].x*mult, partikel[i].y*mult, partikel[i].z*mult);
   	   glBegin(GL_QUADS);
   	   glVertex3f(-0.1, 0.1,0);
   	   glVertex3f(-0.1,-0.1,0);
   	   glVertex3f( 0.1,-0.1,0);
   	   glVertex3f( 0.1, 0.1,0);
   	   glEnd();
    }
}

void proceed_explosion()
{
    int i, j;
    float add;
  	for (i=0; i<explosion_anz; i++)
	{
	   add=(chrono-explosion[i]->t+400);
	   add=2/add;
	   accelerate(0, &explosion[i]->speed, 30);
 	   explosion[i]->pos.x-=explosion[i]->m.v[2][0]*explosion[i]->speed*fmult;
  	   explosion[i]->pos.y-=explosion[i]->m.v[2][1]*explosion[i]->speed*fmult;
   	   explosion[i]->pos.z-=explosion[i]->m.v[2][2]*explosion[i]->speed*fmult;
   	   if (rand() % 2 ==1) explosion[i]->anz--;
   	   for(j=0;j<explosion[i]->anz;j++) explosion[i]->mult+=add;
   	   if ((chrono-explosion[i]->t)>1800)
       {
          kill_explosion(i);
          i--;
       }
    }
}

void draw_pointer(float x, float y, float aspect)
{
    VECTOR heading=player.unit->target->heading;
    heading.z=0;
    normalize_vector_f(&heading.x, &heading.y, &heading.z);    

    if(heading.x<-aspect)
      glTranslatef(x,heading.y/heading.x*x,0);    
    else if(heading.x>aspect)
      glTranslatef(-x,-heading.y/heading.x*x,0);    
    else if(heading.y>=0)
      glTranslatef(-heading.x/heading.y*y, -y,0);    
    else if(heading.y<0)
      glTranslatef(heading.x/heading.y*y, y,0);   
}


void kill_unit(short u)
{
    UNIT *temp;
    int i;

//    free(unit[u]->name);
    free(unit[u]->object);
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
      start_explosion(&u->m, &u->speed, u->property);
      if (u != 0)play_3D_sound_v((VECTOR*)&u->m.t, sound[5]);
      kill_unit(u->index);
   }
   }
}

void fighter_hit_object(UNIT *u1, UNIT *u2)
{
    damage_unit(u1, 100*fmult);
    VECTOR push;
    push.x=u2->m.t[0]-u1->m.t[0];
    push.y=u2->m.t[1]-u1->m.t[1];
    push.z=u2->m.t[2]-u1->m.t[2];
    normalize_vector_f(&push.x, &push.y, &push.z);
    u1->flightdir.x+=push.x*(u1->speed/5+2);
    u1->flightdir.y+=push.y*(u1->speed/5+2);
    u1->flightdir.z+=push.z*(u1->speed/5+2);
}

void missile_hit_object(UNIT *u1, UNIT *u2)
{
    VECTOR col_point;
    u2->model->colmodel->getCollisionPoint ( &col_point.x, 0); 
    MATRIX_f m=u2->m;
    translate_matrix_v(&m, &col_point);
    start_explosion(&m, &u2->speed, u1->property);
    damage_unit(u2, MISSILEDAMAGE);
    kill_unit(u1->index);
    testval++;
    play_3D_sound_v(&col_point, sound[10]);
    if(u2->shield > 0)play_3D_sound_v(&col_point, snd_shieldhit);
    else play_3D_sound_v(&col_point, snd_hullhit);
}

void shot_hit_fighter(SHOT *sht, UNIT *u)
{
    VECTOR col_point;
    u->model->colmodel->getCollisionPoint ( &col_point.x, 0); 
    MATRIX_f m=u->m;
    translate_matrix_v(&m, &col_point);
    start_explosion(&m, &u->speed, sht->property);
    m.v[2][0]=col_point.x+sht->m.v[2][0];
    m.v[2][1]=col_point.y+sht->m.v[2][1];
    m.v[2][2]=col_point.z+sht->m.v[2][2];
    VECTOR turn;
    cross_product_v((VECTOR*)&u->m.t[0], &col_point, (VECTOR*) &m.v[2][0], &turn);
    float hitvalue=-vector_length_f(turn.x, turn.y, turn.z);
    get_vector_rotation_matrix_f(&m, turn.x, turn.y, turn.z, hitvalue);
    matrix_mul_f(&m, &u->m, &u->m);
    u->flightdir.x+=sht->m.v[2][0];
    u->flightdir.y+=sht->m.v[2][1];
    u->flightdir.z+=sht->m.v[2][2];
    damage_unit(u, sht->type);
    kill_shot(sht->index);
    if(u->shield > 0)play_3D_sound_v(&col_point, snd_shieldhit);
    else play_3D_sound_v(&col_point, snd_hullhit);
}

void shot_hit_object(SHOT *sht, UNIT *u)
{
   MATRIX_f m=identity_matrix_f;
   u->model->colmodel->getCollisionPoint (&m.t[0], 0); 
   play_3D_sound_v((VECTOR*)&m.t[0], snd_hullhit);
   start_explosion(&m, &u->speed, sht->property);
   damage_unit(u, sht->type);
   kill_shot(sht->index);
}

void collision_detection()
{
    short u, i, u1, u2;
    for(i=0; i<shot_anz; i++)
    for(u=0;u<unit_anz;u++)
    if (shot[i]->property!=unit[u])
    if (check_col(&shot[i]->m, &unit[u]->m, unit[u]->model->radius + (shot[i]->speed + unit[u]->speed)*fmult))  //COLMODEL.RADIUS!!!
    switch(unit[u]->type)
    {
       case UNIT_MISSILE:
            if(cd_shot_unit(shot[i], unit[u]))
            {
               play_3D_sound_v((VECTOR*)&unit[u]->m.t[0], sound[10]);
               start_explosion(&unit[u]->m, &unit[u]->speed, shot[i]->property);
               kill_shot(i);
               kill_unit(u);
      //         i--;
            }
       break;
       case UNIT_FIGHTER:
            if(cd_shot_unit(shot[i], unit[u]))
            {
               shot_hit_fighter(shot[i], unit[u]);
          //     i--;
            }
       break;
       case UNIT_OBJECT:
            if(cd_shot_unit(shot[i], unit[u]))
            {
               shot_hit_object(shot[i], unit[u]);
       //        i--;
            }
       break;
    }

    for(u1=0; u1<unit_anz; u1++)
    for(u2=u1+1;u2<unit_anz;u2++)
    if (check_col(&unit[u1]->m, &unit[u2]->m, unit[u1]->model->radius + unit[u2]->model->radius + (unit[u1]->speed + unit[u2]->speed)*fmult))//COLMODEL.RADIUS!!!
    switch(unit[u1]->type)
    {
       case UNIT_MISSILE:
       if(unit[u1]->property!=unit[u2])
       switch(unit[u2]->type)
       {
          case UNIT_FIGHTER:
               if(cd_missile_unit(unit[u1], unit[u2]))
               missile_hit_object(unit[u1], unit[u2]);
          break;
          case UNIT_OBJECT:
                if(cd_missile_unit(unit[u1], unit[u2]))
               missile_hit_object(unit[u1], unit[u2]);
          break;
          case UNIT_MISSILE:
  //             cd_missile_unit(unit[u1], unit[u2]);
          break;
       }
       break;
       case UNIT_FIGHTER:
       switch(unit[u2]->type)
       {
          case UNIT_MISSILE:
               if(unit[u2]->property!=unit[u1])
               if(cd_missile_unit(unit[u2], unit[u1]))
               missile_hit_object(unit[u2], unit[u1]);
          break;
          case UNIT_OBJECT:
               if(cd_fighter_object(unit[u1], unit[u2]))
               fighter_hit_object(unit[u1], unit[u2]);
          break;
       }
       break;
       case UNIT_OBJECT:
       switch(unit[u2]->type)
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
    }       
    
}

void get_object_matrix_world(MATRIX_f *m, short u, short o)
{
    MATRIX_f m2=get_object_matrix(u, o);
    VECTOR pos;
    invert_matrix(&unit[u]->m, m);
    zero_matrix(m);
    apply_matrix_f(m, m2.t[0], m2.t[1], m2.t[2], &pos.x, &pos.y, &pos.z);
    *m=unit[u]->m;
    m->t[0]=unit[u]->m.t[0]+pos.x;
    m->t[1]=unit[u]->m.t[1]+pos.y;
    m->t[2]=unit[u]->m.t[2]+pos.z;
}


void start_shot(short u, const char name[], char type)
{
    VECTOR pos=null;
    shot[shot_anz]->type=type;
    shot[shot_anz]->property=unit[u];
    get_object_matrix_world(&shot[shot_anz]->m, u, get_object_by_name(unit[u]->model, name));
    shot[shot_anz]->speed=SHOTSPEED+unit[u]->speed;
    shot[shot_anz]->t=chrono;
    shot_anz++;
}


void proceed_shots()
{
   int i;
   for (i=0; i<shot_anz; i++)
   {
	  shot[i]->m.t[0]-=shot[i]->m.v[2][0]*shot[i]->speed*fmult;
	  shot[i]->m.t[1]-=shot[i]->m.v[2][1]*shot[i]->speed*fmult;
	  shot[i]->m.t[2]-=shot[i]->m.v[2][2]*shot[i]->speed*fmult;
	  if (chrono-shot[i]->t>SHOTTIME)
      {
         kill_shot(i);
         i--;
      }
    }
}

bool check_aiming(short u, const char name[])
{
    MATRIX_f om;
    static GLfloat glm[16];
    get_object_matrix_world(&om, u, get_object_by_name(unit[u]->model, name));
    if(unit[u]->target->object[0].moved)
    {
       MATRIX_f m=unit[u]->target->object[0].m;
       matrix_mul_f(&m, &unit[u]->target->m, &m);
       allegro_gl_MATRIX_f_to_GLfloat(&m, glm);
    }
    else allegro_gl_MATRIX_f_to_GLfloat(&unit[u]->target->m, glm);
    glm[12]=player.vorhalt.t[0];
    glm[13]=player.vorhalt.t[1];
    glm[14]=player.vorhalt.t[2];

    om.v[2][0]*=-1;
    om.v[2][1]*=-1;
    om.v[2][2]*=-1;

    unit[u]->target->model->colmodel->setTransform (glm); 
    return(unit[u]->target->model->colmodel->rayCollision (&om.t[0], &om.v[2][0], 1));
}

void start_jet(short u, short o)
{
    jet[jet_anz]->t=chrono;
 /*   apply_matrix_f(&unit[u]->m, unit[u]->model->o3d[o]->vert[0]->x, unit[u]->model->o3d[o]->vert[0].y, unit[u]->model->o3d[o]->vert[0].z, &jet[jet_anz]->v[0].x, &jet[jet_anz]->v[0].y, &jet[jet_anz]->v[0].z);
    apply_matrix_f(&unit[u]->m, unit[u]->model->o3d[o]->vert[1].x, unit[u]->model->o3d[o]->vert[1].y, unit[u]->model->o3d[o]->vert[1].z, &jet[jet_anz]->v[1].x, &jet[jet_anz]->v[1].y, &jet[jet_anz]->v[1].z);
    apply_matrix_f(&unit[u]->m, unit[u]->model->o3d[o]->vert[2].x, unit[u]->model->o3d[o]->vert[2].y, unit[u]->model->o3d[o]->vert[2].z, &jet[jet_anz]->v[2].x, &jet[jet_anz]->v[2].y, &jet[jet_anz]->v[2].z);
    apply_matrix_f(&unit[u]->m, unit[u]->model->o3d[o]->vert[3].x, unit[u]->model->o3d[o]->vert[3].y, unit[u]->model->o3d[o]->vert[3].z, &jet[jet_anz]->v[3].x, &jet[jet_anz]->v[3].y, &jet[jet_anz]->v[3].z);
*/    jet[jet_anz]->nextjet=unit[u]->jet;
    jet[jet_anz]->lastjet=NO_JET;
    unit[u]->jet->lastjet=jet[jet_anz];
    unit[u]->jet=jet[jet_anz];
    jet_anz++;
}

void kill_jet(JET *j)
{
    JET *temp;
    j->lastjet->nextjet=NO_JET;
    temp=j;
    j=jet[jet_anz];
    jet[jet_anz]=temp;
    jet_anz--;
}

void draw_jet(JET *j)
{
   
}

void set_view_l()
{
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_l);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_l);
	glViewport(0, 85, SCREEN_W, SCREEN_H-85);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-player.mb/2+player.an, player.mb/2+player.an, -player.half_viewport_heigth, player.half_viewport_heigth, player.db, player.far_clip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void move_camera_l()
{
    glMultMatrix_allegro(&player.camera.m);
	glTranslatef(	-player.camera.pos.x+player.camera.stereo.x,
			        -player.camera.pos.y+player.camera.stereo.y,
		            -player.camera.pos.z+player.camera.stereo.z);
    glPushMatrix();
}

void set_view_r()
{
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_r);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_r);
	glViewport(0,  85, SCREEN_W, SCREEN_H-85); //113
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-player.mb/2-player.an, player.mb/2-player.an, -player.half_viewport_heigth, player.half_viewport_heigth, player.db, player.far_clip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void move_camera_r()
{
    glMultMatrix_allegro(&player.camera.m);
	glTranslatef(	-player.camera.pos.x-player.camera.stereo.x,
			        -player.camera.pos.y-player.camera.stereo.y,
		            -player.camera.pos.z-player.camera.stereo.z);
    glPushMatrix();
}

void draw_object(O3D *o3d,  char c)
{
    int i, p, j=0;
 	for (p = 0; p < o3d->poly_anz; p++)
	{ 
	   glNormal3fv(&o3d->n[p].x);
	   glColor3ubv(col[c][o3d->color[p]]);
	   glVertexPointer(3, GL_FLOAT, 0, o3d->vert);
	   glDrawElements(GL_POLYGON, o3d->ind_anz[p], GL_UNSIGNED_SHORT, &o3d->i[j]);
	   j+=o3d->ind_anz[p];
	} 
}

void draw_objects(short u, short o, char c)
{
    glPushMatrix();
    glTranslatef(unit[u]->model->o3d[o].pos.x, unit[u]->model->o3d[o].pos.y, unit[u]->model->o3d[o].pos.z);
    if(unit[u]->object[o].moved)glMultMatrix_allegro(&unit[u]->object[o].m);

    if(unit[u]->object[o].visible)draw_object(&unit[u]->model->o3d[o], c);
    if(unit[u]->model->o3d[o].child_o)
    {
       draw_objects(u, unit[u]->model->o3d[o].child_o, c);
    }
    glPopMatrix();    

    if(unit[u]->model->o3d[o].sibl_o)
    {
       draw_objects(u, unit[u]->model->o3d[o].sibl_o, c);
    }
}

void draw_unit(short u, char c)
{
       glPushMatrix();
       glMultMatrix_allegro(&unit[u]->m);
       draw_objects(u, 0, c);
       glPopMatrix();
}

void draw_object_lines(O3D *o3d)
{
    glVertexPointer(3, GL_FLOAT, 0, o3d->vert);
    glDrawElements(GL_LINES, o3d->line_anz*2, GL_UNSIGNED_SHORT, o3d->line);
}

void draw_objects_lines(short u, short o)
{
    glPushMatrix();
    glTranslatef(unit[u]->model->o3d[o].pos.x, unit[u]->model->o3d[o].pos.y, unit[u]->model->o3d[o].pos.z);
    if(unit[u]->object[o].moved)glMultMatrix_allegro(&unit[u]->object[o].m);

    if(unit[u]->object[o].visible)draw_object_lines(&unit[u]->model->o3d[o]);
    if(unit[u]->model->o3d[o].child_o)
    {
       draw_objects_lines(u, unit[u]->model->o3d[o].child_o);
    }
    glPopMatrix();    

    if(unit[u]->model->o3d[o].sibl_o)
    {
       draw_objects_lines(u, unit[u]->model->o3d[o].sibl_o);
    }
}

void draw_unit_lines(short u)
{
       glPushMatrix();
       glMultMatrix_allegro(&unit[u]->m);
       draw_objects_lines(u, 0);
       glPopMatrix();
}
/*
void draw_engine_outline(short u, bool c)
{
    int i;
//    glBlendFunc(GL_BLEND);

//    glEnable(GL_BLEND);
    for (i=0; i<MAXENGINE; i++)
    if (unit[u]->engine[i].active)
    {
   	   glColor3ubv(col[c][15-((chrono-unit[u]->engine[i].t)/25)]);
  // 	 glColor3ubv(col[c][15]);
       glPushMatrix();
 //      glTranslatef(unit[u]->engine[i].m.t[0], unit[u]->engine[i].pos.y, unit[u]->engine[i].pos.z);
       glMultMatrix_allegro(&unit[u]->engine[i].m);
  //     draw_object_lines(&unit[u]->model->o3d[unit[u]->model->engine]);
       draw_object(&unit[u]->model->o3d[unit[u]->model->engine], c);
       glPopMatrix();
    }
//    glDisable(GL_BLEND);
}
*/
void start_engine_outline(short u)
{
    int i;
    for (i=0; i<MAXENGINE; i++)
    if (!unit[u]->engine[i].active)
    {
  //     unit[u]->engine[i].pos.x=unit[u]->m.t[0];
  //     unit[u]->engine[i].pos.y=unit[u]->m.t[1];
  //     unit[u]->engine[i].pos.z=unit[u]->m.t[2];
       unit[u]->engine[i].m=unit[u]->m;
       unit[u]->engine[i].t=chrono;
       unit[u]->engine[i].speed=(unit[u]->speed-unit[u]->speed_soll.z);
       unit[u]->engine[i].active=1;
       break;
    }
}

void proceed_engine_outline(short u)
{
    int i; float speed;
    for (i=0; i<MAXENGINE; i++)
    if (unit[u]->engine[i].active)
    {
       speed=unit[u]->engine[i].speed*fmult;
       unit[u]->engine[i].m.t[0]-=unit[u]->engine[i].m.v[2][0]*speed;
       unit[u]->engine[i].m.t[1]-=unit[u]->engine[i].m.v[2][1]*speed;
       unit[u]->engine[i].m.t[2]-=unit[u]->engine[i].m.v[2][2]*speed;
       if ((chrono-unit[u]->engine[i].t)>300)unit[u]->engine[i].active=0;
    }
}

void draw_integer(int index)
{
   int a;
   int i=0;

   char str[16], b[2], c;
   b[1]='\n';
   sprintf(str, "%d", index);
   b[0]=str[0];
    while(b[0]!='\0')
    {
       a=atoi(b);
	   glVertexPointer(3, GL_FLOAT, 0, ziffern.o3d[a].vert);
	   glDrawElements(GL_TRIANGLES, 3*ziffern.o3d[a].poly_anz, GL_UNSIGNED_SHORT, &ziffern.o3d[a].i[0]);
	   glTranslatef(8,0,0);
	   i++;
       b[0]=str[i];
    }
}

void draw_char(unsigned char a)
{
   if((a >= 33) && (a <=96)) a-= 33;
   else if((a >= 97) && (a <=122)) a-= 65;
   else if((a >= 123) && (a <=128)) a-= 63;
   else if (a=='ß') a=71;
   else if (a=='ä' ||a=='Ä') a=72;
   else if (a=='ö' ||a=='Ö') a=73;
   else if (a=='ü' ||a=='Ü') a=74;
   else if (a==223) a=73;
   glVertexPointer(3, GL_FLOAT, 0, font3d.o3d[a].vert);
   glDrawElements(GL_QUADS, 4*font3d.o3d[a].poly_anz, GL_UNSIGNED_SHORT, &font3d.o3d[a].i[0]);
}

void draw_string(char string[])
{
    int i=0, column=0;
    while(string[i]!='\0')
    {
       if(string[i]=='\n')
       {
          glTranslatef(-column*4,-6, 0);
          column=0;
       }
       else if(string[i]==' ')
       {
          glTranslatef(4,0,0);
          column++;
       }
       else
       {
          draw_char(string[i]);
          glTranslatef(4,0,0);
          column++;
       }
       i++;       
    }
}


void init_debris()
{
	float aufl=AUFL;
	int aufl2=AUFL, i;
	for (i=0; i<MAXDEBRIS; i++)
	{
        get_rotation_matrix_f(&debris[i], rand()%256, rand()%256, rand()%256);
		debris[i].t[0] = (rand() % aufl2)*(DBOX_X/aufl) - DBOX_X/2;
		debris[i].t[1] = (rand() % aufl2)*(DBOX_Y/aufl) - DBOX_Y/2;
		debris[i].t[2] =-(rand() % aufl2)*(DBOX_Z/aufl);

	}
	aufl=156;
	for(i=0;i<8;i++)
	{
       get_rotation_matrix_f(&debris_rot[i], (rand()%256-128)/aufl, (rand()%256-128)/aufl, (rand()%256-128)/aufl);
    }
}

void proceed_debris()
{
	int i;
	float aufl=AUFL;
	int aufl2=AUFL;

	for (i=0; i<MAXDEBRIS; i++)
	{
       matrix_mul_f(&debris_rot[i%8], &debris[i], &debris[i]);
       matrix_mul_f(&debris[i], &player.camera.m_delta, &debris[i]);
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

void draw_debris(char c)
{
    int i;
   	for (i = 0; i < MAXDEBRIS; i++)
	{
	   glPushMatrix();
       glMultMatrix_allegro(&debris[i]);
       draw_object(&model[13]->o3d[0], c);
	   glPopMatrix();	
	}
}

void draw_monitor()
{
    int i, o=0;
    float monwink, aspect;
    VECTOR heading;
  //  MATRIX_f m=player.unit->m;
  //  zero_matrix(&m);

    get_heading(&player.unit->m, &player.vorhalt, &heading);
	glViewport(265, 8, 162, 100);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-65, 65, -40, 40);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	aspect=player.unit->target->model->size/player.distance_to_target;
	monwink=RAD_2_DEG(atan(aspect))*1.2;
	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(monwink, 1.902, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(player.camera.pos.x, player.camera.pos.y, player.camera.pos.z, player.unit->target->m.t[0], player.unit->target->m.t[1], player.unit->target->m.t[2], player.unit->m.v[1][0], player.unit->m.v[1][1], player.unit->m.v[1][2]);
	glColor4ub(100, 128, 0,0);
	glDisable(GL_DEPTH_TEST);
	
	glPushMatrix();
	draw_unit_lines(player.unit->target->index);
	glPopMatrix();
	
	for (i=0; i<shot_anz; i++)
	if(shot[i]->property==player.unit)
    {
	   glPushMatrix();
	   switch(shot[i]->type)
	   {
	      case SHOT_PLASMA:
	      	   glTranslatef(shot[i]->m.t[0], shot[i]->m.t[1], shot[i]->m.t[2]);
	      	   glMultMatrix_allegro(&player.zero_m);
	           draw_circle(0.2);
	      break;
	      case SHOT_LASER:
   	           glMultMatrix_allegro(&shot[i]->m);
     		   glVertexPointer(3, GL_FLOAT, 0, laser.o3d[0].vert);
		       glDrawElements(GL_QUADS, laser.o3d[0].poly_anz*4, GL_UNSIGNED_SHORT, &laser.o3d[0].i[0]);
	      break;
       }
	   glPopMatrix();
	}

	glColor4ub(180, 160, 10,0);

	for (i=0; i<explosion_anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(explosion[i]->pos.x, explosion[i]->pos.y, explosion[i]->pos.z);
	   glMultMatrix_allegro(&player.zero_m);
       glScalef(explosion[i]->mult, explosion[i]->mult, explosion[i]->mult);
       draw_object_lines(&mod_explosions.o3d[0]);
	   glPopMatrix();
    }
     
	for (i=0; i<unit_anz; i++)
	if(unit[i]->property==player.unit && unit[i]->status==AI_SEEKING)
	{
	   glPushMatrix();
       glMultMatrix_allegro(&unit[i]->m);
	   glVertexPointer(3, GL_FLOAT, 0, unit[i]->model->o3d[0].vert);
       glDrawElements(GL_QUADS, unit[i]->model->o3d[0].poly_anz*4, GL_UNSIGNED_SHORT, &unit[i]->model->o3d[0].i[0]);
	   glPopMatrix();
    } 
	
	
	glPushMatrix();

	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(monwink, 1.902, 0, 200);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

	gluLookAt(0, 0, 0, 0, heading.y, heading.z, 0, -1, 0);
	glTranslatef(0, 0, 0);
	glRotatef(0, 0,0, 0);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
//	glColor4ub(100,128, 0,0);
//	glVertex3f( 10, sinus10[0][0]*10, sinus10[0][1]*10);
//	glVertex3f(-10, sinus10[0][0]*10, sinus10[0][1]*10);
	glColor4ub(40, 50, 0,0);
	for (i=0; i<36; i++)
	{
		glVertex3f( 10,sinus10[i][0]*10, sinus10[i][1]*10);
		glVertex3f(-10, sinus10[i][0]*10, sinus10[i][1]*10);
	}
	glEnd();
	glPopMatrix();
	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(monwink, 1.902, 0, 200);
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
	gluLookAt(0, 0, 0, heading.x, 0, heading.z, 0, 1, 0);
	glTranslatef(0, 0, 0);
	glRotatef(0, 0,0, 0);
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
//	glColor4ub(100,128, 0,0);
//	glVertex3f(sinus10[0][0]*10, 10, sinus10[0][1]*10);
//	glVertex3f(sinus10[0][0]*10,-10, sinus10[0][1]*10);
//	glColor4ub(50, 64, 0,0);
	for (i=0; i<36; i++)
	{
		glVertex3f(sinus10[i][0]*10, 10, sinus10[i][1]*10);
		glVertex3f(sinus10[i][0]*10,-10, sinus10[i][1]*10);
	}
	glEnd();
	glPopMatrix();
	
    glColor4ub(80, 100, 0,0);
	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(5, 1.902, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 0, heading.x, -heading.y, heading.z, 0, 1, 0);
	glPushMatrix();
	glTranslatef(0, 0, 8);
    draw_circle_outline(0.15);
   	glPopMatrix();

    glColor4ub(130,160, 0,0);
	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(monwink, 1.902, 0, 200);
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
//    MATRIX_f m = player.unit->m;
  //  zero_matrix(&m);
   	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-player.mb/2, player.mb/2, -player.half_viewport_heigth, player.half_viewport_heigth, player.db, player.far_clip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glMultMatrix_allegro(&player.camera.m);
	glTranslatef(	-player.camera.pos.x,
			        -player.camera.pos.y,
		            -player.camera.pos.z);
 	glColor4ub(100, 128, 0,0);
	for (i=0;i<RL.u.anz;i++)
    {
       draw_unit_lines(RL.u.i[i]);
       if(unit[RL.u.i[i]]->status == AI_DUMBFIRE)
       {
//       	   glColor3ubv(col[c][10 + rand()%6]);

          glBegin(GL_LINES);
          glVertex3fv(&unit[RL.u.i[i]]->m.t[0]);
          glVertex3fv(&unit[RL.u.i[i]]->start_pos.x);
          glEnd();
       }
    }   
	for (i=0; i<RL.shot.anz; i++)
    {
	   glPushMatrix();
	   switch(shot[RL.shot.i[i]]->type)
	   {
	      case SHOT_PLASMA:
	      	   glTranslatef(shot[RL.shot.i[i]]->m.t[0], shot[RL.shot.i[i]]->m.t[1], shot[RL.shot.i[i]]->m.t[2]);
	      	   glMultMatrix_allegro(&player.zero_m);
	           draw_circle(0.2);
	      break;
	      case SHOT_LASER:
   	           glMultMatrix_allegro(&shot[RL.shot.i[i]]->m);
     		   glVertexPointer(3, GL_FLOAT, 0, laser.o3d[0].vert);
		       glDrawElements(GL_QUADS, laser.o3d[0].poly_anz*4, GL_UNSIGNED_SHORT, &laser.o3d[0].i[0]);
	      break;
       }
	   glPopMatrix();
	}

	glColor4ub(180, 160, 100,0);

	for (i=0; i<explosion_anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(explosion[i]->pos.x, explosion[i]->pos.y, explosion[i]->pos.z);
	   glMultMatrix_allegro(&player.zero_m);
	   draw_circle_outline(explosion[i]->mult);
	   glPopMatrix();
    }
    glEnable(GL_BLEND);
 	glColor4ub(100, 128, 0,0);
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);
 //   glVertexPointer(3, GL_INT, 0, zielkreuz[0]);
 //   glDrawArrays(GL_LINES, 0, 8);
    glTranslatef(0,0,1);
    draw_circle_outline(2);

    glDisable(GL_BLEND);
	glLoadIdentity();
    glPointSize(1.0);
	glColor4ub(100, 130, 20,0);

	glBegin(GL_POINTS);
   	for (i = 0; i < MAXDEBRIS; i++)
	{
	   glVertex3f(debris[i].t[0], debris[i].t[1], debris[i].t[2]);
	   
	}
	glEnd();
//	   glPopMatrix();	


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
    glColor4ub(100, 128, 0,0);
 //   draw_objects_lines(0, 0);
    draw_objects(0, 0, 1);

}


/*
void draw_cockpit()
{
    allegro_gl_set_allegro_mode();
//    blit(bmp, screen, 0, 0, 0, 367, 640, 113);
    if(player.unit->target!=NO_TARGET)blit(logo[player.unit->target->party], screen, 0, 0, 267, 380, 30, 35);
    blit(logo[0], screen, 0, 0, 463, 398, 30, 35);
    if(player.target_monitor_interference>chrono)blit(rauschen, screen, rand()%84, rand()%164, 267, 380, 172, 90);
    allegro_gl_unset_allegro_mode(); 
}
*/

void load_texture()
{
	PALETTE pal;
	BITMAP *bmp;//, *bmp2;
	int w, h;
	bmp = load_bitmap ("bmp/cockpit4.bmp", pal);
	if (!bmp) {
		allegro_message ("Error loading `cockpit4.bmp'");
		exit (1);
	}
	tex = allegro_gl_make_masked_texture (bmp);
	destroy_bitmap (bmp);
}

void draw_cockpit_bitmap()
{
	glViewport(0, -15, 1024, 128);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable (GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f( 1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1,  1);
 	glEnd(); 
	glDisable (GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
}

void init_lighting()
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

void draw_cockpit01()
{
    short u;
    char buf[128];

	draw_cockpit_bitmap();
	glColor4ub(200, 255, 0,0);
	glPointSize(2.0);
	for (u=1;u<unit_anz; u++)
	{
	      draw_scanner_point(&unit[u]->heading);
    }
    glColor4ub(200, 255, 0,0);
    glPointSize(1.0);
   	for (u=0;u<shot_anz; u++)
	{
	      draw_scanner_point(&shot[u]->heading);
    }
    if(player.unit->target!=NO_TARGET)
    {
       glColor4ub(255, 255, 255,0);
       glPointSize(3.0);
       draw_scanner_point(&player.unit->target->heading);
    }
    
    if(player.target_monitor_interference<chrono)
    {
	   if (player.unit->target!=NO_TARGET && !buttn_pressed)draw_monitor();
	   else draw_monitor2();
	}
	draw_monitor3();
	glViewport(0, 0, player.aufl_w, player.aufl_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0 , player.aufl_w, 0, player.aufl_h);
 	glColor4ub(100, 128, 0,0);

	if((player.unit->target)!=NO_TARGET)
	{
	   if(!player.target_is_visible)
	   {
   	      glPushMatrix();
   	      glTranslatef(320,300,0);  	
   	      draw_pointer(320, 180, 0.8715);
   	      MATRIX_f m;
   	      get_align_matrix_f(&m,0,0,1, player.unit->target->heading.x, player.unit->target->heading.y, 0);
   	      glMultMatrix_allegro(&m);
   	      glColor4ub(60, 67, 90,0);
   	      glEnable(GL_BLEND);

   	      draw_arrow();
   	      glDisable(GL_BLEND);
   	      glColor4ub(160, 128, 0,0);
   	      glPopMatrix();
       }
  	   else
  	   {
          if(player.gunhits[0])
 	      {
 	         glPushMatrix();
 	         glTranslatef(380,50,0);
 	         draw_string("XXXX\nXXXX");
 	         glPopMatrix();
          }
          if(player.gunhits[1])
          {
           	 glPushMatrix();
           	 glTranslatef(300,50,0);
           	 draw_string("XXXX\nXXXX");
           	 glPopMatrix();
          } 
  	   } 
       if(player.target_monitor_interference<chrono)
 	   {
  	      glPushMatrix();
          glTranslatef(360,90,0);
          sprintf(buf, "hull: %.1f\nshld: %.1f\nspd : %.1f\ndist: %.1f", player.unit->target->hull, player.unit->target->shield, player.unit->target->speed, player.distance_to_target);
          draw_string(buf);
          glPopMatrix();
 	      glPushMatrix();
          glTranslatef(298,95,0);
	      draw_string(player.unit->target->name);
 	      glPopMatrix();
	   }
  	}


 	glPushMatrix();
    glTranslatef(116,15,0);
    glRotatef(-90,0, 0, 1);
    draw_energy_bar(5, player.unit->speed_soll.z*1.5);    
    glTranslatef(0,1,0);
    draw_energy_bar(5, player.unit->speed*1.5);    
 	glPopMatrix();
 	
    glPushMatrix();
	sprintf(buf, "%.1f", player.unit->speed);
    glTranslatef(100, 10,0);
    draw_string(buf);
 	glPopMatrix();

    glPushMatrix();
	sprintf(buf, "%.1f", player.unit->speed_soll.z);
    glTranslatef(223, 10,0);
    draw_string(buf);
 	glPopMatrix();
 	
    glEnable(GL_BLEND);

 	glPushMatrix();
    glTranslatef(268,10,0);
    draw_energy_bar(5, player.unit->gunpower[1]*0.9);    
 	glPopMatrix();
	
 	glPushMatrix();
    glTranslatef(430,10,0);
    draw_energy_bar(5, player.unit->gunpower[0]*0.9);    
 	glPopMatrix();
 	
    glDisable(GL_BLEND);

	
 	glPushMatrix();
    glTranslatef(500,78,0);
    sprintf(buf, "ia  : %d\naa  : %d\nfps : %.1f\nshts: %d\nunts: %d\njoy : %d\neye : %.1f", get_i_action_anz(), get_a_action_anz(), fps_rate, shot_anz, unit_anz, stick.state, player.aa*100);
	draw_string(buf);
 	glPopMatrix();
 	
	glPushMatrix();
	sprintf(buf, "hull: %.1f\nshld: %.1f\ntrst: %.f\nspd : %.1f", player.unit->hull, player.unit->shield, player.unit->speed_soll.z, player.unit->speed);
    glTranslatef(100,70,0);
    draw_string(buf);
 	glPopMatrix();
 	
}


void render_pipeline(char c)
{
    int i;
	for (i=0; i<explosion_anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(explosion[i]->pos.x, explosion[i]->pos.y, explosion[i]->pos.z);
	   glMultMatrix_allegro(&player.zero_m);
	   draw_explosion(i, c);
	   glPopMatrix();
    } 
	for (i=0; i<RL.shot.anz; i++)
	{
	   glPushMatrix();
	   switch(shot[RL.shot.i[i]]->type)
	   {
	      case SHOT_PLASMA:
	      	   glTranslatef(shot[RL.shot.i[i]]->m.t[0], shot[RL.shot.i[i]]->m.t[1], shot[RL.shot.i[i]]->m.t[2]);
	      	   glMultMatrix_allegro(&player.zero_m);
	           glColor3ubv(col[c][15]);
	           draw_circle(0.2);
	      break;
	      case SHOT_LASER:
   	           glMultMatrix_allegro(&shot[RL.shot.i[i]]->m);
               draw_object(&laser.o3d[0], c);	      
	      break;
       }
	   glPopMatrix();
	}

	for (i=0;i<RL.u.anz;i++)
	{
	   if   (unit[RL.u.i[i]]->justhit &&!unit[RL.u.i[i]]->big) draw_unit(RL.u.i[i], c+2);
	   else                          draw_unit(RL.u.i[i], c);
     /* if(unit[RL.u.i[i]]->model->engine && ((unit[RL.u.i[i]]->status == AI_ATTACKING)||(unit[RL.u.i[i]]->status == AI_SEEKING)))
       draw_engine_outline(RL.u.i[i], c);
       else*/ 
   }
   for(i=0;i<unit_anz;i++)
   {
      if(unit[i]->status == AI_DUMBFIRE)
      {
    	 glColor3ubv(col[c][10 + rand()%6]);
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
	   glColor3ubv(col[c][13]);
	   draw_marker(player.unit->target->model->size/2);
	   glColor3ubv(col[c][15]);
	   glPopMatrix();

	   glPushMatrix();
	   glTranslatef(player.vorhalt.t[0], player.vorhalt.t[1], player.vorhalt.t[2]);
	   glMultMatrix_allegro(&player.zero_m);
	   glColor3ubv(col[c][13]);
	   draw_marker(5);
	   glPopMatrix();
	   
	   if(player.gunhits[0])
	   {
	      glPushMatrix();
	      glTranslatef(player.hitpoint[0].x, player.hitpoint[0].y, player.hitpoint[0].z);
	      glMultMatrix_allegro(&player.zero_m);

	      glColor3ubv(col[c][15]);
	      draw_circle_outline(player.distance_to_target*0.005);
	      glPopMatrix();
	   }
	   if(player.gunhits[1])
	   {
	      glPushMatrix();
	      glTranslatef(player.hitpoint[1].x, player.hitpoint[1].y, player.hitpoint[1].z);
	      glMultMatrix_allegro(&player.zero_m);
	      glColor3ubv(col[c][15]);
	      draw_circle_outline(player.distance_to_target*0.005);
	      glPopMatrix();
	   }
//     draw_unit_lines(player.unit->target->index);
//     glEnable(GL_DEPTH_TEST);
       glPopMatrix();
    }
}
 
void render()
{
    int j, u, i;
  //  glEnable(GL_SCISSOR_TEST);
  // glIsEnabled(GL_SCISSOR_TEST);
 //  glScissor(0, 150, 640, 330);
////////////////////////////////////////////////////////////////////////////
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
//lCallList(l_kam_list);
    set_view_l();

    glPointSize(1.0);
    glPushMatrix();
    glMultMatrix_allegro(&player.camera.m);
    draw_stars(0);
    glPopMatrix();


	glEnable(GL_DEPTH_TEST);
	move_camera_l();

	render_pipeline(0);
	glCallList(l_kam_list);
	glTranslatef(player.an, 0, 0);
 //   glMultMatrix_allegro(&player.camera.m);
 //	draw_crosses(0);
    set_view_l();
    glTranslatef(-player.camera.offset.t[0]+player.an, -player.camera.offset.t[1], -player.camera.offset.t[2]);

    glPushMatrix();
    draw_debris(0);
    glPopMatrix();

	////////////////////////////////////////////////////////////////////////////
	glClear(GL_DEPTH_BUFFER_BIT );
	glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
//lCallList(r_kam_list);
    set_view_r();

   glDisable(GL_DEPTH_TEST); 	

    glPointSize(1.0);
    glPushMatrix();
    glMultMatrix_allegro(&player.camera.m);
    draw_stars(1);
    glPopMatrix();
   	glEnable(GL_DEPTH_TEST);

/*	
	          glPushMatrix();
		      glMultMatrix_allegro(&player.camera.m);
		      draw_icosahedron(1);
		      glPopMatrix();
*/
	move_camera_r();
	render_pipeline(1);
	
//	glCallList(r_kam_list);
//	glTranslatef(-player.an, 0, 0);
//	glMultMatrix_allegro(&player.camera.m);
    set_view_r();
    glTranslatef(-player.camera.offset.t[0]-player.an, -player.camera.offset.t[1], -player.camera.offset.t[2]);
    glPushMatrix();
    draw_debris(1);
    glPopMatrix();
 //	draw_crosses(1);
	////////////////////////////////////////////////////////////////////////////
	glViewport(0,  0, SCREEN_W, SCREEN_H);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	printf("TEST a\n");
	if(player.unit->justhit)
	{
	   glViewport(0, 0, player.aufl_w, player.aufl_h);
	   glMatrixMode(GL_PROJECTION);
	   glLoadIdentity();
	   glMatrixMode(GL_MODELVIEW);
	   glLoadIdentity();
	   gluOrtho2D(0 , player.aufl_w, 0, player.aufl_h);
	   glColor4ub(120, 0, 200,0);
	   glEnable(GL_BLEND);
	   glBegin(GL_QUADS);
	   glVertex2f(0,0);
	   glVertex2f(SCREEN_W,0);
	   glVertex2f(SCREEN_W, SCREEN_H);
	   glVertex2f(0,SCREEN_H);
	   glEnd();
	   glDisable(GL_BLEND);
	}
	draw_cockpit01();
	glFlush();
	allegro_gl_flip();
}


void align_target(short u, float value)
{
    VECTOR heading;
    get_heading(&unit[u]->m, &unit[u]->target->m, &heading);
    unit[u]->wink_soll=null;
    if (heading.x>0.01)
    { 
       if ((heading.z>0) && (heading.x<0.1)) unit[u]->wink_soll.y=4;
       else {unit[u]->wink_soll.y=value;unit[u]->wink_soll.z=5;}
    }
    else if (heading.x<-0.01)
    {
       if ((heading.z>0) && (heading.x>-0.1)) unit[u]->wink_soll.y=-4;
       else {unit[u]->wink_soll.y=-value;unit[u]->wink_soll.z=-5;}

    }
    if (heading.y>0.01)
    { 
       if ((heading.z>0) && (heading.y<0.1)) unit[u]->wink_soll.x=-4;
       else unit[u]->wink_soll.x=-value;
    }
    else if (heading.y<-0.01)
    {
       if ((heading.z>0) && (heading.y>-0.1)) unit[u]->wink_soll.x=4;
       else unit[u]->wink_soll.x=value;
    }
}

void turn_unit(short u)
{
    MATRIX_f m; VECTOR pos;
    pos.x=unit[u]->m.t[0];
    pos.y=unit[u]->m.t[1];
    pos.z=unit[u]->m.t[2];
    zero_matrix(&unit[u]->m);
    accelerate(unit[u]->wink_soll.x, &unit[u]->wink_ist.x, ROTACC);
    accelerate(unit[u]->wink_soll.y, &unit[u]->wink_ist.y, ROTACC);
    accelerate(unit[u]->wink_soll.z, &unit[u]->wink_ist.z, ROTACC);
    unit[u]->speed-=unit[u]->speed*vector_length_f(unit[u]->wink_ist.x, unit[u]->wink_ist.y, 0)*ROTDEC*fmult;
    get_rotation_matrix_f(&m, unit[u]->wink_ist.x*fmult, unit[u]->wink_ist.y*fmult, unit[u]->wink_ist.z*fmult);
    matrix_mul_f(&unit[u]->m, &m,  &unit[u]->m);
    unit[u]->m.t[0]=pos.x;
    unit[u]->m.t[1]=pos.y;
    unit[u]->m.t[2]=pos.z;
}

void fire_at_target(short u)
{
    VECTOR heading;
    MATRIX_f m; VECTOR emit;

    get_heading(&unit[u]->m, &unit[u]->target->m, &heading);
    if((heading.x<0.15) && (heading.x>-0.15)&& (heading.y<0.15)&& (heading.y>-0.15)&& (heading.z>0))
    {   
       if ( chrono-unit[u]->shotdelay > unit[u]->udata->shootrate)
       {
          if(unit[u]->gun)
          {
             start_shot(u, "lemit", SHOT_LASER);
             move(unit[u], "lgun", Z_AXIS, 1, 60);
             wait(unit[u], 200);
             move(unit[u], "lgun", Z_AXIS, 0, 2);
 
          }
          else
          {
             start_shot(u, "remit", SHOT_LASER);
             move(unit[u], "rgun", Z_AXIS, 1, 60);
             wait(unit[u], 200);
             move(unit[u], "rgun", Z_AXIS, 0, 2);          }
              
          unit[u]->shotdelay=chrono;
          unit[u]->gun=1-unit[u]->gun;
          play_3D_sound_v((VECTOR*)&unit[u]->m.t, sound[1]);
       }
    }
}
/*
void move_ship(short u)
{
    float speed;
	if (unit[u]->speed_soll.z != unit[u]->speed)
	{
       accelerate(unit[u]->speed_soll.z, &unit[u]->speed, TRANSACC);
    }
    accelerate(unit[u]->speed_soll.z, &unit[u]->speed, TRANSACC);
    speed=unit[u]->speed*fmult;
    unit[u]->m.t[0]-=unit[u]->m.v[2][0]*speed;
    unit[u]->m.t[1]-=unit[u]->m.v[2][1]*speed;
    unit[u]->m.t[2]-=unit[u]->m.v[2][2]*speed;
}
*/

void move_ship(short u)
{
    float speed;
    VECTOR dir_soll, dir_delta;
    dir_soll.x=unit[u]->m.v[2][0]*unit[u]->speed_soll.z;
    dir_soll.y=unit[u]->m.v[2][1]*unit[u]->speed_soll.z;
    dir_soll.z=unit[u]->m.v[2][2]*unit[u]->speed_soll.z;
    if(unit[u]->speed_soll.x)
    {
       dir_soll.x+=unit[u]->m.v[0][0]*unit[u]->speed_soll.x;
       dir_soll.y+=unit[u]->m.v[0][1]*unit[u]->speed_soll.x;
       dir_soll.z+=unit[u]->m.v[0][2]*unit[u]->speed_soll.x;
    }
    if(unit[u]->speed_soll.y)
    {
       dir_soll.x+=unit[u]->m.v[1][0]*unit[u]->speed_soll.y;
       dir_soll.y+=unit[u]->m.v[1][1]*unit[u]->speed_soll.y;
       dir_soll.z+=unit[u]->m.v[1][2]*unit[u]->speed_soll.y;
    }
    if((unit[u]->flightdir.x != dir_soll.x) ||
       (unit[u]->flightdir.y != dir_soll.y) ||
       (unit[u]->flightdir.z != dir_soll.z))
    {
    
       unit[u]->dir_delta.x=(unit[u]->flightdir.x-dir_soll.x);
       unit[u]->dir_delta.y=(unit[u]->flightdir.y-dir_soll.y);
       unit[u]->dir_delta.z=(unit[u]->flightdir.z-dir_soll.z);
       dir_delta.x=unit[u]->dir_delta.x;
       dir_delta.y=unit[u]->dir_delta.y;
       dir_delta.z=unit[u]->dir_delta.z;
       if(dir_delta.x<0)dir_delta.x*=-1;
       if(dir_delta.y<0)dir_delta.y*=-1;
       if(dir_delta.z<0)dir_delta.z*=-1;
       
       normalize_vector_f(&dir_delta.x, &dir_delta.y, &dir_delta.z);
       accelerate(dir_soll.x, &unit[u]->flightdir.x, dir_delta.x*unit[u]->udata->acceleration);
       accelerate(dir_soll.y, &unit[u]->flightdir.y, dir_delta.y*unit[u]->udata->acceleration);
       accelerate(dir_soll.z, &unit[u]->flightdir.z, dir_delta.z*unit[u]->udata->acceleration);      
    }
    unit[u]->speed=vector_length_f(unit[u]->flightdir.x, unit[u]->flightdir.y, unit[u]->flightdir.z);
    unit[u]->m.t[0]-=unit[u]->flightdir.x*fmult;
    unit[u]->m.t[1]-=unit[u]->flightdir.y*fmult;
    unit[u]->m.t[2]-=unit[u]->flightdir.z*fmult;
}

void start_missile(short u, const char name[], char status)
{
   unit[unit_anz]->name="Missile";
   unit[unit_anz]->party=unit[u]->party;
   unit[unit_anz]->property=unit[u];
   unit[unit_anz]->type=UNIT_MISSILE;
   unit[unit_anz]->status=status;
   unit[unit_anz]->shield=0;
   unit[unit_anz]->hull=100;
   get_object_matrix_world(&unit[unit_anz]->m, u, get_object_by_name(unit[u]->model, name));
   unit[unit_anz]->flightdir=unit[u]->flightdir;
   unit[unit_anz]->speed=unit[u]->speed;
   unit[unit_anz]->speed_soll.z=MISSILEMAXSPEED;
   unit[unit_anz]->t=chrono;
   unit[unit_anz]->vulnerable=TRUE;

   if(status ==AI_SEEKING)
   {
      unit[unit_anz]->target=unit[u]->target;
      unit[unit_anz]->model=&mod_seeker_missile;
      spin(unit[unit_anz], "base", 2, 200);
      unit[unit_anz]->udata=&seeker_missile;
      unit[unit_anz]->flightdir.x+=unit[unit_anz]->m.v[2][0]*MISSILESTARTSPEED;
      unit[unit_anz]->flightdir.y+=unit[unit_anz]->m.v[2][1]*MISSILESTARTSPEED;
      unit[unit_anz]->flightdir.z+=unit[unit_anz]->m.v[2][2]*MISSILESTARTSPEED;
   }
   else
   {
      unit[unit_anz]->model=&mod_dumbfire;
      unit[unit_anz]->udata=&dumb_missile;
      unit[unit_anz]->start_pos=trans_matrix_to_vector(&unit[unit_anz]->m);
      unit[unit_anz]->flightdir.x+=unit[unit_anz]->m.v[2][0]*DUMBMISSILESTARTSPEED;
      unit[unit_anz]->flightdir.y+=unit[unit_anz]->m.v[2][1]*DUMBMISSILESTARTSPEED;
      unit[unit_anz]->flightdir.z+=unit[unit_anz]->m.v[2][2]*DUMBMISSILESTARTSPEED;

      
   }
   prepare_object_matrices(unit_anz); 
   unit_anz++;
}

void buttn(char status)
{
    switch(status)
    {
       case BTTN_DEFAULT:
       play_sample (sound[8], 24, 128, 1000, 0);
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
          case KEY_R:
          buttn(BTTN_DEFAULT);
	      player.unit->target=get_nearest_enemy_target(0);
          break;
          
 	      case KEY_T:
          buttn(BTTN_DEFAULT);
	      get_next_target(0);
	      break;
	      
          case KEY_Y:
          buttn(BTTN_DEFAULT);
	      get_last_target(0);
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
	      if(key_shifts & KB_SHIFT_FLAG) player.aa-=0.001;
	      else player.aa+=0.001;
	      player.an = player.aa/2;
          break;
          
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
          start_shot(1, "lemit", unit[1]->udata->shottype);
          start_shot(1, "remit", unit[1]->udata->shottype);
          play_3D_sound_v((VECTOR*)&unit[1]->m.t, sound[1]);

          break;
          
  	      case KEY_F4:
          buttn(BTTN_DEFAULT);
          start_missile(1, "lmount", AI_DUMBFIRE);
          play_3D_sound_v((VECTOR*)&unit[1]->m.t, sound[9]);
          break;
  	      case KEY_F5:
          buttn(BTTN_DEFAULT);
          raptor_open_cabin(unit[1]);
          break;
  	      case KEY_F6:
          buttn(BTTN_DEFAULT);
          raptor_close_cabin(unit[1]);
          break;

       }
    }
    }
        else player.keyboard_was_pressed=FALSE;
            
	if (stick.present)
	{	 
	   refresh_stick(&stick);
       player.unit->wink_soll.x = stick.axis[0]*player.unit->udata->maxrotationspeed;
       if(stick.b[3])
       {
          player.unit->wink_soll.z = stick.axis[1]*player.unit->udata->maxrotationspeed;
          player.unit->wink_soll.y = 0;
       }
       else
       {
          player.unit->wink_soll.y = stick.axis[1]*player.unit->udata->maxrotationspeed;
          player.unit->wink_soll.z = 0;
       }
	   if (stick.b[0] && ((chrono-player.unit->shotdelay) > 0))
	   {
	      if ((player.unit->gun==1) && (player.unit->gunpower[0]>=player.unit->udata->gun_erergy_drain))
	      {
	         start_shot(0, "lemit", player.unit->udata->shottype);
             play_sample (sound[1], 48, 228, 1000, 0);
	         player.unit->gun=0;
	         player.unit->gunpower[0]-=player.unit->udata->gun_erergy_drain;
             player.unit->shotdelay = chrono+player.unit->udata->shootrate;
          }
          else if ((!player.unit->gun) && (player.unit->gunpower[1]>=player.unit->udata->gun_erergy_drain))
          {
	         start_shot(0, "remit", player.unit->udata->shottype);
   	         player.unit->gun=1;
	         play_sample (sound[1], 48, 32, 1000, 0);
   	         player.unit->gunpower[1]-=player.unit->udata->gun_erergy_drain;
             player.unit->shotdelay = chrono+player.unit->udata->shootrate;
          }
          else if((player.unit->gunpower[0]>=player.unit->udata->gun_erergy_drain) && (player.unit->gunpower[1]>=player.unit->udata->gun_erergy_drain))
          {
	         start_shot(0, "lemit", player.unit->udata->shottype);
	         start_shot(0, "remit", player.unit->udata->shottype);
             play_sample (sound[1], 48, 128, 1000, 0);
             player.unit->shotdelay = chrono+2*player.unit->udata->shootrate;
             player.unit->gunpower[0]-=player.unit->udata->gun_erergy_drain;
             player.unit->gunpower[1]-=player.unit->udata->gun_erergy_drain;

          }
       }
       if (stick.b[1] && ((chrono-player.unit->shotdelay) > MISSILESHOOTRATE))
       {
          if(player.unit->target!=NO_TARGET && !buttn_pressed)start_missile(0, "lmount", AI_SEEKING);
          else start_missile(0, "rmount", AI_DUMBFIRE);
          play_sample (sound[9], 32, 128, 1000, 0);
          player.unit->shotdelay = chrono;
       }
       if (stick.b[3])
       {
          if (!buttn_pressed)
          {
              buttn_pressed=1;
              buttn_delay=chrono;
          }
       }
       else if(buttn_pressed)
       {
          buttn_pressed=0;   
          if ((chrono-buttn_delay)<300)       
          {
             player.unit->target=target_unit_near_reticule();
             player.target_monitor_interference=chrono+120;
             play_sample (sound[6], 32, 128, 1000, 0);       
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
	   if (key[KEY_SPACE] && ((chrono-player.unit->shotdelay) > 0))
	   {
	      if (player.unit->gun==1)
	      {
	         start_shot(0, "lemit", player.unit->udata->shottype);
             play_sample (sound[1], 48, 228, 1000, 0);
	         player.unit->gun=0;
             player.unit->shotdelay = chrono+player.unit->udata->shootrate;
          }
          else if (!player.unit->gun)
          {
	         start_shot(0, "remit", player.unit->udata->shottype);
   	         player.unit->gun=1;
	         play_sample (sound[1], 48, 32, 1000, 0);
             player.unit->shotdelay = chrono+player.unit->udata->shootrate;
          }
          else
          {
	         start_shot(0, "lemit", player.unit->udata->shottype);
	         start_shot(0, "remit", player.unit->udata->shottype);
             play_sample (sound[1], 48, 128, 1000, 0);
             player.unit->shotdelay = chrono+2*player.unit->udata->shootrate;
          }
       }
       if ( key[KEY_LCONTROL] && ((chrono-player.unit->shotdelay) > MISSILESHOOTRATE))
       {
          if(player.unit->target!=NO_TARGET && !buttn_pressed)start_missile(0, "lmount", AI_SEEKING);
          else start_missile(0, "rmount", AI_DUMBFIRE);
          play_sample (sound[9], 32, 128, 1000, 0);
          player.unit->shotdelay = chrono;
       }
    }
    clear_keybuf();
}


void proceed_player_ship()
{
    player.zero_m=player.unit->m;  
    zero_matrix(&player.zero_m);  
    invert_matrix(&player.unit->m, &player.camera.m);
    zero_matrix(&player.camera.m);
    VECTOR pos_delta;
    apply_matrix_f(&player.camera.m, player.camera.offset.t[0], player.camera.offset.t[1], player.camera.offset.t[2], &player.camera.pos.x, &player.camera.pos.y, &player.camera.pos.z);
    apply_matrix_f(&player.camera.m, player.an, 0, 0, &player.camera.stereo.x, &player.camera.stereo.y, &player.camera.stereo.z);
    player.camera.pos.x+=player.unit->m.t[0];
    player.camera.pos.y+=player.unit->m.t[1];
    player.camera.pos.z+=player.unit->m.t[2]; 
    get_matrix_delta(&player.camera.m, &player.camera.m_old, &player.camera.m_delta);
    apply_matrix_f(&player.zero_m, player.unit->flightdir.x, player.unit->flightdir.y, player.unit->flightdir.z, &player.camera.m_delta.t[0], &player.camera.m_delta.t[1], &player.camera.m_delta.t[2]);
    player.camera.m_delta.t[0]*=fmult;
    player.camera.m_delta.t[1]*=fmult;
    player.camera.m_delta.t[2]*=fmult;
    player.camera.m_old=player.camera.m;
}

void refresh_unit_energy(short u)
{
    accelerate(100, &unit[u]->gunpower[0], 5);
    accelerate(100, &unit[u]->gunpower[1], 5);
    accelerate(100, &unit[u]->shield, 1);
}

void proceed_units()
{
    int u;
	for (u=0; u<unit_anz; u++)
	{
	   if (unit[u]->justhit) if(chrono-unit[u]->justhit > 100)unit[u]->justhit=0;
       switch(unit[u]->status)
       {
          case AI_PLAYER:
          user_input();
          turn_unit(u);
          move_ship(u);
          proceed_player_ship();
          refresh_unit_energy(u);
          break;
          
          
          case AI_ATTACKING:
          align_target(u, 30);
          turn_unit(u);
          fire_at_target(u);
          move_ship(u);
          refresh_unit_energy(u);
    /*      if(unit[u]->model->engine)
          {
             proceed_engine_outline(u);
             if((chrono-unit[u]->enginetimer) > 100)
             {
                start_engine_outline(u);
                unit[u]->enginetimer=chrono;
             }
          } */
          break;
          
          case AI_SEEKING:
          align_target(u, MISSILETURN);
          turn_unit(u);
          move_ship(u);
    /*      if(unit[u]->model->engine)
          {
             proceed_engine_outline(u);
             if((chrono-unit[u]->enginetimer) > 100)
             {
                start_engine_outline(u);
                unit[u]->enginetimer=chrono;
             }
          }
     */
          if((chrono-unit[u]->t)>MISSILETIMER)
          {
             start_explosion(&unit[u]->m, &unit[u]->speed, unit[u]->property);
             kill_unit(u);
  //           u--;
          }
          break;
          
          case AI_DUMBFIRE:
          move_ship(u);
          if((chrono-unit[u]->t)>MISSILETIMER)
          {
             start_explosion(&unit[u]->m, &unit[u]->speed, unit[u]->property);
             kill_unit(u);
  //           u--;
          }
          break;
          case AI_JUSTFLYING:
          move_ship(u);
          if((chrono-unit[u]->t)>MISSILETIMER)
          break;
       }
    }
}

void init_display_lists()
{
    l_kam_list = glGenLists (1);
    glNewList(l_kam_list, GL_COMPILE);
    set_view_l();
    glEndList();

    r_kam_list = glGenLists (1);
    glNewList(r_kam_list, GL_COMPILE);
    set_view_r();
    glEndList();
    
    draw_cross_list = glGenLists (1);
    glNewList(draw_cross_list, GL_COMPILE);
 //   draw_cross();
    glEndList();
}



short init_collision_model(MODEL *mod, O3D *o3d)
{
    int p, j=0;
    mod->colmodel = newCollisionModel3D();
    for(p=0;p<o3d->poly_anz;p++)
    {
       mod->colmodel->addTriangle(&o3d->vert[o3d->i[j]].x,
                                  &o3d->vert[o3d->i[j+1]].x,
                                  &o3d->vert[o3d->i[j+2]].x);
       j+=o3d->ind_anz[p];
    }
    mod->colmodel->finalize();
}

void init_3d_world()
{
    short u;
    
	lese_3do(model[0],  "3do/huede1.3do");
	lese_3do(model[1],  "3do/huede2.3do");
	lese_3do(model[2],  "3do/raptor.3do");
//	lese_3do(model[2],  "3do/anthony02.3do");
	lese_3do(model[3],  "3do/huede1_col2.3do");
	lese_3do(model[5],  "3do/station0.3do");
	lese_3do(model[6],  "3do/asteroid01.3do");
	lese_3do(model[7],  "3do/asteroid02.3do");
	lese_3do(model[8],  "3do/station0_col.3do");
	lese_3do(model[9],  "3do/anthony_col.3do");
	lese_3do(model[10], "3do/huede2_col.3do");
	lese_3do(model[11], "3do/asteroid03.3do");
	lese_3do(model[12], "3do/GryphRak1_col.3do");
	lese_3do(model[13], "3do/debris01.3do");
	
	lese_3do(model[4], "3do/huede2.3do");
	
	init_collision_model(model[0], &model[3]->o3d[0]);
	init_collision_model(model[1], &model[10]->o3d[0]);
	init_collision_model(model[2], &model[9]->o3d[0]);
	init_collision_model(&mod_dumbfire,&model[12]->o3d[0]);
	mod_seeker_missile.colmodel=mod_dumbfire.colmodel;
	
	init_collision_model(model[5], &model[8]->o3d[0]);
	init_collision_model(model[6], &model[6]->o3d[0]);
	init_collision_model(model[7], &model[7]->o3d[0]);
	init_collision_model(model[11],&model[11]->o3d[0]);
	init_collision_model(model[4],&model[10]->o3d[0]);

	player.camera.m =identity_matrix_f;
	player.camera.m_delta = identity_matrix_f;
	player.camera.m_old = identity_matrix_f;

	unit[unit_anz]->name="Martin";
	unit[unit_anz]->status=AI_PLAYER;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[2];
	unit[unit_anz]->m =identity_matrix_f;

	unit[unit_anz]->m.t[0] = 5;
	unit[unit_anz]->m.t[1] = 0;
	unit[unit_anz]->m.t[2] = 20;
	unit[unit_anz]->target = unit[2];
	unit[unit_anz]->shield=100;
	unit[unit_anz]->hull=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=1;
	unit[unit_anz]->speed_soll.z=0;
	unit[unit_anz]->speed=1;
	unit[unit_anz]->udata=&craft1;
	unit[unit_anz]->gunpower[0]=90;
	unit[unit_anz]->gunpower[1]=90;
    unit_anz++;

 	unit[unit_anz]->name="pruegelknabe";
	unit[unit_anz]->status=AI_JUSTFLYING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[2];
	get_z_rotate_matrix_f(&unit[unit_anz]->m, -30);
	unit[unit_anz]->m.t[0] = 0;
	unit[unit_anz]->m.t[1] = 0;
	unit[unit_anz]->m.t[2] = 0;
	unit[unit_anz]->speed_soll.z=2;
	unit[unit_anz]->target = NO_TARGET;
	unit[unit_anz]->shield=80;
    unit[unit_anz]->hull=100;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
	unit[unit_anz]->udata=&craft1;
    unit_anz++;
    
 /*  	unit[unit_anz]->name="Enemy II";
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[2];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 10;
	unit[unit_anz]->m.t[1] = 20;
	unit[unit_anz]->m.t[2] = 30;
	unit[unit_anz]->speed_soll.z=35;
	unit[unit_anz]->target = player.unit;
	unit[unit_anz]->shield=80;
    unit[unit_anz]->hull=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=2;
	unit[unit_anz]->udata=&craft1;
	unit[unit_anz]->gunpower[0]=90;
	unit[unit_anz]->gunpower[1]=90;
    unit_anz++; */

 	unit[unit_anz]->name="Station";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[5];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 300;
	unit[unit_anz]->m.t[1] = 300;
	unit[unit_anz]->m.t[2] = -1700;
	unit[unit_anz]->vulnerable=0; 
	unit[unit_anz]->party=0;
	unit[unit_anz]->big=1;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;
    
   	unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[6];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 100;
	unit[unit_anz]->m.t[1] = 100;
	unit[unit_anz]->m.t[2] = -1000;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 200;
	unit[unit_anz]->m.t[1] = 40;
	unit[unit_anz]->m.t[2] = -860;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 120;
	unit[unit_anz]->m.t[1] = -40;
	unit[unit_anz]->m.t[2] = -460;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;


    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[7];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 0;
	unit[unit_anz]->m.t[1] = 10;
	unit[unit_anz]->m.t[2] = -360;
 	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[6];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = -40;
	unit[unit_anz]->m.t[1] = 20;
	unit[unit_anz]->m.t[2] = -1300;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[7];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 280;
	unit[unit_anz]->m.t[1] = 300;
	unit[unit_anz]->m.t[2] = -1600;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	spin(unit[unit_anz], "base", Z_AXIS, 10);
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 268;
	unit[unit_anz]->m.t[1] = 250;
	unit[unit_anz]->m.t[2] = -1500;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
    unit_anz++;

    unit[unit_anz]->name="Asteroid";
	unit[unit_anz]->status=AI_NOTHING;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = -20;
	unit[unit_anz]->m.t[1] = -10;
	unit[unit_anz]->m.t[2] = -100;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
    unit_anz++;
/*
	
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->target = unit[1];
	unit[unit_anz]->model = model[1];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 20;
	unit[unit_anz]->m.t[1] = 0;
	unit[unit_anz]->m.t[2] = -50;
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->shield=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=0;
    unit_anz++;

	
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->target = player.unit;
	unit[unit_anz]->model = model[0];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 0;
	unit[unit_anz]->m.t[1] = -10;
	unit[unit_anz]->m.t[2] = -120;
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->shield=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=1;
    unit_anz++;

	
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[12];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->m.t[0] = 0;
	unit[unit_anz]->m.t[1] = 0;
	unit[unit_anz]->m.t[2] = 200;
	unit[unit_anz]->party=0;
	unit[unit_anz]->target=unit[13];
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->shield=100;
    unit_anz++;
    */
    
    for(u=0;u<unit_anz;u++)prepare_object_matrices(u);
    
    prepare_raptor(unit[0]);
    prepare_raptor(unit[1]);
//    prepare_raptor(unit[2]);

}

void init_player_data()
{
    player.aa                   =    0.068;
	player.mb                   =    0.32;
	player.db                   =    0.55;
	player.an                   =    0.034;
    player.ppm                  = 2000;
	player.aap                  =  136;
	player.far_clip             = 3000;
    player.aufl_w               =  640;
    player.aufl_h               =  480;
    player.farbtiefe            =   32;
    player.z_puffer             =   32;
    player.half_viewport_heigth =    0.09875;
    
    player.wait_for_release     = FALSE;
    player.keyboard_was_pressed = FALSE;
    player.unit=unit[0];

    craft1.label                = "AV-01 SC";
    craft1.maxspeed             =   70;
    craft1.acceleration         =   40;
    craft1.rotacceleration      =  200;
    craft1.hullpower            = 1250;
    craft1.shieldpower          =  800;
    craft1.mass                 =    6;
    craft1.maxrotationspeed     =   60;
    craft1.shootrate            =  150;
    craft1.shottype             = SHOT_LASER;
    craft1.gunpower             =  160;
    craft1.gun_erergy_drain     =    7;
    
    craft2.label                = "MH-02 UVF";
    craft2.maxspeed             =   55;
    craft2.acceleration         =   25;
    craft2.rotacceleration      =  120;
    craft2.hullpower            = 2300;
    craft2.shieldpower          = 1750;
    craft2.mass                 =   12;
    craft2.maxrotationspeed     =   50;
    craft2.shootrate            =  450;
    craft2.shottype             = SHOT_PLASMA;
    craft2.gunpower             =  270;
    craft2.gun_erergy_drain     =   20;

    seeker_missile.label                = "MISS-01-SKR";
    seeker_missile.maxspeed             =   130;
    seeker_missile.acceleration         =   195;
    seeker_missile.rotacceleration      =   360;
    seeker_missile.hullpower            =    25;
    seeker_missile.shieldpower          =     0;
    seeker_missile.mass                 =     0.15;
    seeker_missile.maxrotationspeed     =    360;
    seeker_missile.shootrate            =     0;
    seeker_missile.shottype             =     0;
    
    dumb_missile.label                = "MISS-001-DMB";
    dumb_missile.maxspeed             =   250;
    dumb_missile.acceleration         =    60;
    dumb_missile.rotacceleration      =   100;
    dumb_missile.hullpower            =    15;
    dumb_missile.shieldpower          =     0;
    dumb_missile.mass                 =     0.05;
    dumb_missile.maxrotationspeed     =    20;
    dumb_missile.shootrate            =     0;
    dumb_missile.shottype             =     0;
    
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
	glClearColor(0.156, 0.156, 0.156, 1.0);      //Hintergrundfarbe
    glEnableClientState (GL_VERTEX_ARRAY);       //wird noch nicht gebraucht
    glBlendFunc(GL_ONE, GL_ONE);                 //Blending-Methode. RGB-Werte werden zusammenaddiert 

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
  	    #ifdef MUSIK
        al_poll_duh(dp);
	    #endif
        proceed_actions();
		collision_detection();
		proceed_shots();
		proceed_explosion();
		proceed_units();
		proceed_debris();
		get_headings();
		make_RL();
        if(player.unit->target!=NO_TARGET)
        {
           player.distance_to_target=distance(&player.camera.pos, (VECTOR*)&player.unit->target->m.t[0]);
           player.time_to_impact=time_to_impact(player.unit->target->index);
           calc_vorhalt(player.unit->target->index, player.time_to_impact);
          if(player.target_is_visible=spot_is_visible(&player.unit->target->heading))
           {
             if(player.gunhits[0]=check_aiming(0,"lemit"))
              {
 
                 player.unit->target->model->colmodel->getCollisionPoint (&player.hitpoint[0].x, 0); 
              }
              player.gunhits[1]=check_aiming(0, "remit");
              {
                  player.unit->target->model->colmodel->getCollisionPoint (&player.hitpoint[1].x, 0); 
              }
           }
        }

		render();

	}
}

int main(void)
{
    NO_TARGET->index=-1;
	allegro_init();
    #ifdef MUSIK
    atexit(&dumb_exit);
   	#endif

	install_allegro_gl();
	install_keyboard();
	set_keyboard_rate(1, 10);
	init_pointers();
	init_player_data();
//	init_color_table();

	init_gfx();
	load_texture();
    init_actions(&chrono, &fmult);
    init_mathematics(&fmult);
    init_drawprim();
	init_3d_world();
    player.camera.camera_object=get_object_by_name(unit[0]->model, "camera");
	player.camera.offset=get_object_matrix(0, player.camera.camera_object);


	install_int(the_timer, 1);
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);
	init_stick(&stick);
	stick.state=JOY_2;
	load_bitmaps();
	init_sinus10();
	init_lighting();
//	init_display_lists();
//	init_crosses();
	init_stars();
	init_debris();
	init_cd(&fmult);
	init_sound();
    init_explosion();
    #ifdef MUSIK
    dumb_register_stdfiles();
    musicpiece = dumb_load_mod("mod/3d_demo_tune.mod");
    dp=al_start_duh(musicpiece, 1, 0, 1.0, 4096, 22050);
   	#endif

   	lese_3do(&font3d, "3do/font.3do");
	lese_3do(&ziffern, "3do/ziffern.3do");
	lese_3do(&laser, "3do/laser.3do");
	lese_3do(&mod_dumbfire, "3do/dumbfire.3do");
	lese_3do(&mod_explosions, "3do/explosions.3do");
	lese_3do(&mod_seeker_missile,  "3do/GryphRak.3do");

	play_sample (sound[0], 8, 128, 1000, TRUE);
//    printf(" %d \n", allegro_gl_is_extension_supported ("GL_ARB_vertex_buffer_object"));
    fcount=chrono;
    game_loop();

    #ifdef MUSIK
    al_stop_duh(dp);
    unload_duh(musicpiece);
	#endif
  
	destroy_samples();
    destroy_bitmaps();
//	allegro_exit();
	return 0;
}
END_OF_MAIN();
