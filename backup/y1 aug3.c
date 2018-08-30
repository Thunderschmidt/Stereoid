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



#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
//#include <GL/glext.h>
#include <GL/glu.h>

#include "global.h"
#include "input3DO.h"
#include "inputINI.h"
#include "coldet.h"

#include "y1.h"
#include "actions.h"
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
   }
   for(i=0; i<MAXEXPLOSIONS; i++)
   {
       explosion[i]=&explosions[i];
   }
}

void kill_shot(short i)
{
   SHOT *temp;
   temp=shot[i];
   shot[i]=shot[shot_anz-1];
   shot[shot_anz-1]=temp;
   shot_anz--;
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

void glMultMatrix_allegro(MATRIX_f *m)
{
    allegro_gl_MATRIX_f_to_GLfloat(m, glm);
    glMultMatrixf(glm);
}

void time_handler()
{
    int i;
    for (i=0; i<MAXTIMEACTIONS; i++)
    {
       if (timeaction[i].active)
       {
          if(timeaction[i].t<chrono)
          {
             switch (timeaction[i].a)
             {
                case 0: break;
                case 1: break;
             }
          }
       }
    }
}

void get_heading(MATRIX_f *m, VECTOR *pos0, VECTOR *pos1, VECTOR *out)
{
	out->x=pos0->x-pos1->x;
	out->y=pos0->y-pos1->y;
	out->z=pos0->z-pos1->z;
	apply_matrix_f(m, out->x, out->y, out->z, &out->x, &out->y, &out->z);
	normalize_vector_f(&out->x, &out->y, &out->z);
}

bool accurate_collision_check(short u0, short u1)
{
    get_translation_matrix_f(&tm0, unit[u0]->pos.x, unit[u0]->pos.y, unit[u0]->pos.z);
    get_translation_matrix_f(&tm1, unit[u1]->pos.x, unit[u1]->pos.y, unit[u1]->pos.z);
    matrix_mul_f(&unit[u0]->m, &tm0, &tm0);
    matrix_mul_f(&unit[u1]->m, &tm1, &tm1);
    allegro_gl_MATRIX_f_to_GLfloat(&tm0, glm0);
    allegro_gl_MATRIX_f_to_GLfloat(&tm1, glm1);
    colmodel[unit[u0]->model->colmodel]->setTransform (glm0);
    colmodel[unit[u1]->model->colmodel]->setTransform (glm1);
    return(colmodel[unit[u0]->model->colmodel]->collision (colmodel[unit[u1]->model->colmodel]));
}                

/*   
float get_approaching_speed(short u, short i)
{
    MATRIX_f m;
    float speed;
    VECTOR heading;
    get_heading(&unit[u]->m, &unit[u]->pos, &shot[i]->pos, &heading);
    speed=heading.z*unit[u]->speed_ist.z;
    return(speed);
}
*/

void init_sound()
{
    if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
       printf ("Error initializing sound card");
    }	
    sound[0] = load_wav ("sounds/hum.wav");
    sound[1] = load_wav ("sounds/Fusion.wav");
    sound[2] = load_wav ("sounds/lashit2.wav");
    sound[3] = load_wav ("sounds/shieldhit2.wav");
    sound[4] = load_wav ("sounds/tirantarionmissile.wav");
    sound[5] = load_wav ("sounds/TTaDth00.wav");
    sound[6] = load_wav ("sounds/vdu_a.wav");
    sound[7] = load_wav ("sounds/wolf_fire_hit.wav");
}

void destroy_samples()
{
    int i;
    for (i=0; i<8; i++) destroy_sample(sound[i]);
}

void load_bitmaps (void)
{
	bmp=create_video_bitmap(640, 113);
	bmp = load_bitmap ("cockpit.bmp", NULL);
	logo[0]=create_video_bitmap(35, 30);
	logo[0] = load_bitmap ("logo1.bmp", NULL);
	logo[1]=create_video_bitmap(35, 30);
	logo[1] = load_bitmap ("logo2.bmp", NULL);
	logo[2]=create_video_bitmap(35, 30);
	logo[2] = load_bitmap ("logo3.bmp", NULL);
	rauschen=create_video_bitmap(256, 256);
	rauschen=load_bitmap ("rauschen.bmp", NULL);
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


void init_stick()
{
	stick.present=FALSE;
	if (install_joystick(JOY_TYPE_AUTODETECT) != 0) return;
	if (!num_joysticks) return;
	poll_joystick();
    stick.cy=joy[0].stick[0].axis[0].pos;
	stick.cx=joy[0].stick[0].axis[1].pos;
	stick.cz=0;
	stick.present=TRUE;
	return;
}

void refresh_stick()
{
    int i;
    signed char sign;

    poll_joystick();

    stick.axis[0]=joy[0].stick[0].axis[1].pos-stick.cx;
    stick.axis[1]=joy[0].stick[0].axis[0].pos-stick.cy;
    stick.axis[2]=joy[0].stick[0].axis[2].pos-stick.cz;
    stick.b[0]=joy[0].button[0].b;
    stick.b[1]=joy[0].button[1].b;
    stick.b[2]=joy[0].button[2].b;
    stick.b[3]=joy[0].button[3].b;

    for(i=0;i<3;i++) if((stick.axis[i]<TOTZONE)&&(stick.axis[i]>-TOTZONE))stick.axis[i]=0;
    
   	switch(stick.state)
    {
        case JOY_1:
             break;
        case JOY_2:
             for(i=0;i<3;i++)
             {
                sign=(stick.axis[i]>0)*2-1;
                stick.axis[i]/=SQR2_128;
                stick.axis[i]*=stick.axis[i]*sign;
             }
             break;
        case JOY_3:
             for(i=0;i<3;i++)
             {
                stick.axis[i]/=SQR3_128;
                stick.axis[i]*=stick.axis[i]*stick.axis[i];
             }
             break;
        case JOY_4:
             for(i=0;i<3;i++)
             {
                sign=(stick.axis[i]>0)*2-1;
                stick.axis[i]/=SQR4_128;
                stick.axis[i]*=stick.axis[i]*stick.axis[i]*stick.axis[i]*sign;             }
             break;
        case JOY_5:
             for(i=0;i<3;i++)
             {
                stick.axis[i]/=SQR5_128;
                stick.axis[i]*=stick.axis[i]*stick.axis[i]*stick.axis[i]*stick.axis[i];
             }
             break;             
             
    }
    for(i=0;i<3;i++) stick.axis[i]/=128;
}
       
void init_sinus10()
{
	char a;
	for(a=0; a<36; a++)
	{
		sinus10[a][0]=sin( DEG_2_RAD(a*10));
		sinus10[a][1]=cos(  DEG_2_RAD(a*10));
		circleindex[a]=35-a;
	}
}

void draw_quad(float scale)
{
    glScalef(scale, scale, scale);
    glIndexPointer(GL_UNSIGNED_BYTE, 0, circleindex);
    glVertexPointer(2, GL_FLOAT, 0, sinus10[0]);
    glDrawElements(GL_LINES, 36, GL_UNSIGNED_BYTE, circleindex);
}

void draw_marker(float scale)
{
    glScalef(scale, scale, scale);
    glBegin(GL_LINES);
    glVertex2f(-1,0.9);
    glVertex2f(-1,1);
    glVertex2f(-1,1);
    glVertex2f(-0.9, 1);

    glVertex2f(-1,-0.9);
    glVertex2f(-1,-1);
    glVertex2f(-1,-1);
    glVertex2f(-0.9, -1);

    glVertex2f( 1,0.9);
    glVertex2f( 1,1);
    glVertex2f( 1,1);
    glVertex2f( 0.9, 1);

    glVertex2f( 1,-0.9);
    glVertex2f( 1,-1);
    glVertex2f( 1,-1);
    glVertex2f( 0.9, -1);

    glEnd();
}

void draw_circle(float scale)
{
    glScalef(scale, scale, scale);
    glIndexPointer(GL_UNSIGNED_BYTE, 0, circleindex);
    glVertexPointer(2, GL_FLOAT, 0, sinus10[0]);
    glDrawElements(GL_POLYGON, 36, GL_UNSIGNED_BYTE, circleindex);
}

void invert_matrix(MATRIX_f *in, MATRIX_f *out)
{
	out->v[0][0]=in->v[0][0]; out->v[0][1]=in->v[1][0]; out->v[0][2]=in->v[2][0];
	out->v[1][0]=in->v[0][1]; out->v[1][1]=in->v[1][1]; out->v[1][2]=in->v[2][1];
	out->v[2][0]=in->v[0][2]; out->v[2][1]=in->v[1][2]; out->v[2][2]=in->v[2][2];
}

int check_col(VECTOR *pos0, VECTOR *pos1, float bounding)
{
    if (((pos0->x-pos1->x)<bounding) && ((pos0->x-pos1->x)> -bounding))
    if (((pos0->y-pos1->y)<bounding) && ((pos0->y-pos1->y)> -bounding))
    if (((pos0->z-pos1->z)<bounding) && ((pos0->z-pos1->z)> -bounding))
    return(1);
    return(0);
}

void get_next_target(short u)
{
    unit[u]->target=unit[unit[u]->target->index+1];
    if(unit[u]->target->index>=unit_anz)unit[u]->target=unit[0];
    if(unit[u]->target == unit[u]) get_next_target(u);
}

void get_last_target(short u)
{
    if(unit[u]->target->index >0) unit[u]->target=unit[unit[u]->target->index-1];
    else unit[u]->target=unit[unit_anz-1];
    if(unit[u]->target == unit[u]) get_last_target(u);
}

UNIT *get_nearest_enemy_target(short u)
{
    short i, ret; float dist0, dist1=0;
    for (i=1; i<unit_anz; i++)
    if ((u!=i) && unit[i]->party==1)
    {
        dist0=1/vector_length_f(unit[u]->pos.x-unit[i]->pos.x,
                                unit[u]->pos.y-unit[i]->pos.y,
                                unit[u]->pos.z-unit[i]->pos.z);
        if(dist0>dist1)
        {
           ret=i;
           dist1=dist0;
        }
    }
    return(unit[ret]);
}

void accelerate(float soll, float *ist, float a)
{
	a*=fmult;
	if (*ist<soll )
	{
		*ist+=a;
		if (*ist > soll) *ist=soll;
	}
	if (*ist > soll)
	{
		*ist-=a;
		if (*ist < soll) *ist=soll;
	}
}

void init_new_unit()
{
   
   unit[unit_anz]->model = model[0];
   unit[unit_anz]->m =identity_matrix_f;
   get_x_rotate_matrix_f(&unit[unit_anz]->m, 128);
   unit[unit_anz]->pos.x = 300;
   unit[unit_anz]->pos.y = 300;
   unit[unit_anz]->speed_soll.z=50;
   unit[unit_anz]->speed_ist.z=0;
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
    get_heading(&unit[u]->m, &unit[u]->pos, &unit[0]->pos, &heading);
    appspeed=heading.z*unit[u]->speed_ist.z;

    dist=vector_length_f(unit[u]->pos.x-unit[0]->pos.x,
                         unit[u]->pos.y-unit[0]->pos.y,
                         unit[u]->pos.z-unit[0]->pos.z);
    t=dist/(unit[0]->speed_ist.z+SHOTSPEED+appspeed);
    return(t);
}
 


void calc_vorhalt(short u)
{
    float t;
    t=time_to_impact(u);
    vorhalt.x=unit[u]->pos.x-unit[u]->m.v[2][0]*unit[u]->speed_ist.z*t;
    vorhalt.y=unit[u]->pos.y-unit[u]->m.v[2][1]*unit[u]->speed_ist.z*t;
    vorhalt.z=unit[u]->pos.z-unit[u]->m.v[2][2]*unit[u]->speed_ist.z*t;
}

void get_reticule()
{
    float dist;
    if (unit[0]->target->heading.z > 0)
    {
             dist=vector_length_f(camera.pos.x-unit[0]->target->pos.x,
                                  camera.pos.y-unit[0]->target->pos.y,
                                  camera.pos.z-unit[0]->target->pos.z);            
             dist=unit[0]->target->model->size /dist;
             if (unit[0]->target->heading.x < (XHEADING+dist))
             if (unit[0]->target->heading.x >-(XHEADING+dist))
             if (unit[0]->target->heading.y < (YHEADING+dist))
             if (unit[0]->target->heading.y >-(YHEADING+dist))
             {    
                        
 
    VECTOR vo;
    vo.x=-unit[0]->m.v[2][0];
    vo.y=-unit[0]->m.v[2][1];
    vo.z=-unit[0]->m.v[2][2];
    get_translation_matrix_f(&tm0, unit[0]->target->pos.x, unit[0]->target->pos.y, unit[0]->target->pos.z);
    matrix_mul_f(&unit[0]->target->m, &tm0, &tm0);
    allegro_gl_MATRIX_f_to_GLfloat(&tm0, glm0);
    colmodel[unit[0]->target->model->colmodel]->setTransform (glm0);
    if(colmodel[unit[0]->target->model->colmodel]->rayCollision (&unit[0]->pos.x, &vo.x, 0))
    {
    //   getCollisionPoint(&reticule.pos.x, 1)
       reticule.active=1;
    }
    else   reticule.active=0;
             }
       }
}
void make_RL()
{
    short i=0, i2=0; float dist;
    for(i=1; i<unit_anz; i++)
    {
       if (unit[i]->heading.z > 0)
       {
             dist=vector_length_f(camera.pos.x-unit[i]->pos.x,
                                  camera.pos.y-unit[i]->pos.y,
                                  camera.pos.z-unit[i]->pos.z);            
             dist=unit[i]->model->size /dist;
             if (unit[i]->heading.x < (XHEADING+dist))
             if (unit[i]->heading.x >-(XHEADING+dist))
             if (unit[i]->heading.y < (YHEADING+dist))
             if (unit[i]->heading.y >-(YHEADING+dist))
             {    
                RL.u.i[i2]=i;
                i2++;
             }
       }
    }
    RL.u.anz=i2; i2=0;
    for(i=0; i<shot_anz; i++)
    {
       if (shot[i]->heading.z > 0)
       {
             if (shot[i]->heading.x < XHEADING)
             if (shot[i]->heading.x >-XHEADING)
             if (shot[i]->heading.y < YHEADING)
             if (shot[i]->heading.y >-YHEADING)
             {    
                RL.shot.i[i2]=i;
                i2++;
             }
       }
    }
    RL.shot.anz=i2; i2=0;
}

void get_headings()
{
    int i;
    for (i=1; i<unit_anz; i++)
    {
       get_heading(&unit[0]->m, &camera.pos, &unit[i]->pos, &unit[i]->heading);
    }
    for (i=0; i<shot_anz; i++)
    {
       get_heading(&unit[0]->m, &camera.pos, &shot[i]->pos, &shot[i]->heading);
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
    
void play_3D_sound(VECTOR *pos, SAMPLE *sample)
{
    VECTOR heading; float dist;
    get_heading(&unit[0]->m, &camera.pos, pos, &heading);
    dist=vector_length_f(camera.pos.x-pos->x, camera.pos.y-pos->y, camera.pos.z-pos->z);            
    if (dist < 400)play_sample (sample, 2048/(dist+32), 128+heading.x*-128, 1000, 0);
}
/*
void prepare_unit(VECTOR *pos, short o)
{
    oposi[o].x=pos->x+model[0].o3d[o].pos.x;
    oposi[o].y=pos->y+model[0].o3d[o].pos.y;
    oposi[o].z=pos->z+model[0].o3d[o].pos.z;
    if (model[0].o3d[o].child_o > 0) prepare_unit(&oposi[o], model[0].o3d[o].child_o);
    if (model[0].o3d[o].sibl_o > 0)prepare_unit(pos, model[0].o3d[o].sibl_o);
}
*/
void init_debris()
{
	int aufl=1024, i;
	for (i=0; i<DNUM; i++)
	{
		debris[i].x = (rand() %  aufl)*(DBOX_X/aufl) - DBOX_X/2;
		debris[i].y = (rand() %  aufl)*(DBOX_Y/aufl) - DBOX_Y/2;
		debris[i].z = -(rand() %  aufl)*(DBOX_Z/aufl);// - DBOX_Z/2;

	}
}

void proceed_debris()
{
	int i;
	for (i=0; i<DNUM; i++)
	{
       apply_matrix_f(&camera.md, debris[i].x,  debris[i].y,  debris[i].z,
                                 &debris[i].x, &debris[i].y, &debris[i].z);
       debris[i].z+= unit[0]->speed_ist.z*fmult;
       if (debris[i].x <  (-DBOX_X/2)) debris[i].x+=DBOX_X;
       else if (debris[i].x >= (DBOX_X/2)) debris[i].x-=DBOX_X;
       if (debris[i].y <  (-DBOX_Y/2)) debris[i].y+=DBOX_Y;
       else if (debris[i].y >=(DBOX_Y/2)) debris[i].y-=DBOX_Y;
       if (debris[i].z <  (-DBOX_Z -DBOX_ZZ) ) debris[i].z+=DBOX_Z;
       else if (debris[i].z >= -DBOX_ZZ ) debris[i].z-=DBOX_Z;
	}
}

void draw_debris(char c)
{
    int i;
    glColor4ubv(col[c][11]);
	glPointSize(2.0);
	glBegin(GL_POINTS);
	for (i = 0; i < DNUM; i++)
	{
		glVertex3fv(&debris[i].x);
	}
	glEnd();
	glPopMatrix();
}

/*
void init_explode()
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

void start_explode(VECTOR *pos, MATRIX_f *m, float *speed, char property)
{
    int i, j;
	for (i=0; i<MAXEXPLODES; i++)
	{
		if (!explosion[i].active)
		{
		    explosion[i].active=1;
		    explosion[i].pos=*pos;
			explosion[i].t=chrono;
			explosion[i].mult=0.1;
			explosion[i].anz=64;
			explosion[i].m=*m;
			explosion[i].speed=*speed;
			explosion[i].property=property;
			
			for(j=0;j<explosion[i].anz;j++)
            {explosion[i].partikel[j]=rand() % 1024;explosion[i].partikel[j]/=2048;}
			break;
		}
	}
}

void draw_explode(int j, char c)
{
    int i;
    float mult;
   	glColor3ubv(col[c][14]);
   	for (i=0; i<explosion[j].anz;i++)
   	{
   	   mult=explosion[j].partikel[i]*explosion[j].mult;
   	   glTranslatef(partikel[i].x*mult, partikel[i].y*mult, partikel[i].z*mult);
   	   glBegin(GL_QUADS);
   	   glVertex3f(-0.1, 0.1,0);
   	   glVertex3f(-0.1,-0.1,0);
   	   glVertex3f( 0.1,-0.1,0);
   	   glVertex3f( 0.1, 0.1,0);
   	   glEnd();
    }
}
void draw_circle_explode(float scale)
{
    glIndexPointer(GL_UNSIGNED_BYTE, 0, circleindex);
    glVertexPointer(2, GL_FLOAT, 0, sinus10[0]);
    glScalef(scale, scale, scale);
    glDrawElements(GL_LINE_LOOP, 36, GL_UNSIGNED_BYTE, circleindex);
}
void proceed_explode()
{
    int i, j;
    float add;
  	for (i=0; i<MAXEXPLODES; i++)
	{
		if (explosion[i].active)
		{
		  add=(chrono-explosion[i].t+400);
		  add=2/add;
		  accelerate(0, &explosion[i].speed, 30);
		  explosion[i].pos.x-=explosion[i].m.v[2][0]*explosion[i].speed*fmult;
		  explosion[i].pos.y-=explosion[i].m.v[2][1]*explosion[i].speed*fmult;
		  explosion[i].pos.z-=explosion[i].m.v[2][2]*explosion[i].speed*fmult;
		  if (rand() % 2 ==1) explosion[i].anz--;
		  for(j=0;j<explosion[i].anz;j++) explosion[i].mult+=add;
		  if ((chrono-explosion[i].t)>1800)explosion[i].active=0;
        }
    }
}
*/

void kill_unit(short u)
{
    UNIT *temp;
    temp=unit[u];
    unit[u]=unit[unit_anz-1];
    unit[unit_anz-1]=temp;
    unit[u]->index=temp->index;
    unit_anz--;
}

void check_unit_death(short u)
{
    if (unit[u]->shield <=0)
    {
        kill_unit(u);
        if(unit[u]==unit[0]->target)
        {
           mrauschen=chrono+1000;
           unit[0]->target=0;
        }
//        start_explode(&unit[u]->pos, &unit[u]->m, &unit[u]->speed_ist.z, 0);
        if (u!=0)play_3D_sound(&unit[u]->pos, sound[7]);
        init_new_unit();
    }
}

bool damage_unit(short u, float damage)
{
   unit[u]->shield-=damage;
   if(unit[u]->shield<0)
   {
      unit[u]->hull+=unit[u]->shield;
      unit[u]->shield=0;
   }
   if (unit[u]->hull<0) return(TRUE);
   else return(FALSE);
}


float get_approaching_speed(MATRIX_f *m1, VECTOR *pos1, float *speed1, MATRIX_f *m2, VECTOR *pos2, float *speed2)
{
   VECTOR heading1, heading2;
   get_heading(m1, pos1, pos2, &heading1);
   get_heading(m2, pos2, pos1, &heading2);
   return(heading1.z**speed1+heading2.z**speed2);
}

void collision_check()
{
    int u, i, j, checksperframe;
    float speed, mult, speed1, speed2;
    VECTOR vo; VECTOR pos1, pos2;
    bool hit;
    for(i=0; i<shot_anz; i++)
    {
       for(u=0;u<unit_anz;u++)
       {
          if (shot[i]->property!=u)
  		  if (check_col(&shot[i]->pos, &unit[u]->pos, unit[u]->model->radius + (shot[i]->speed + unit[u]->speed_ist.z)*fmult));
  		  {  
  		     speed=get_approaching_speed(&shot[i]->m, &shot[i]->pos, &shot[i]->speed, &unit[u]->m, &unit[u]->pos, &unit[u]->speed_ist.z);
  		     checksperframe=speed*fmult/CHECKDISTANCE;
  		     if(checksperframe<1)checksperframe=1;   
             vo.x=shot[i]->m.v[2][0];
             vo.y=shot[i]->m.v[2][1];
             vo.z=shot[i]->m.v[2][2];
             pos1=unit[u]->pos;
             pos2=shot[i]->pos;
             speed1=unit[u]->speed_ist.z/checksperframe;
             speed2=shot[i]->speed/checksperframe;
             
  		     for(j=0;j<checksperframe;j++)
             {
                get_translation_matrix_f(&tm0, pos1.x, pos1.y, pos1.z);
                matrix_mul_f(&unit[u]->m, &tm0, &tm0);
                allegro_gl_MATRIX_f_to_GLfloat(&tm0, glm0);
                colmodel[unit[u]->model->colmodel]->setTransform (glm0);
                hit=colmodel[unit[u]->model->colmodel]->rayCollision (&pos2.x, &vo.x, 0, 0, SHOTLENGTH+CHECKDISTANCE);
                if(hit)break;
                pos1.x-=unit[u]->m.v[2][0]*speed1;
                pos1.y-=unit[u]->m.v[2][1]*speed1;
                pos1.z-=unit[u]->m.v[2][2]*speed1;
                pos2.x-=unit[i]->m.v[2][0]*speed2;
                pos2.y-=unit[i]->m.v[2][1]*speed2;
                pos2.z-=unit[i]->m.v[2][2]*speed2;
             }
             
  		     if(hit)
  		     {  
// 		        start_explode(&shot[i]->pos, &unit[u].m, &unit[u]->speed_ist.z, shot[i]->property);
  		        shot[i]->active=0;
  		        if (u != 0)play_3D_sound(&shot[i]->pos, sound[2]);
  		        else     play_sample (sound[3], 32, 128, 1000, 0);
  		        unit[u]->justhit=chrono;
  		        if(unit[u]->vulnerable)
                {
                   damage_unit(u, SHOTDAMAGE);
                   check_unit_death(u);
                }
  		     break;
             }
	      }
       }
    }


 /*   for(i=0; i<unit_anz; i++)
    {
       for(u=i;u<unit_anz;u++)
       {
          if(i!=u)
          if (check_col(&unit[i]->pos, &unit[u].pos, model[unit[u].model].size))   //ACHTUNG!!
          {
//             if(accurate_collision_check(i, u))
  //           {       
                unit[u].justhit=chrono;
                unit[i]->justhit=chrono;
    //         }
             break;
          }
       }
    }
*/
}

void start_shot(VECTOR *pos, MATRIX_f *m, short property, float speed)
{
    shot[shot_anz]->pos=*pos;
    shot[shot_anz]->property=property;
    shot[shot_anz]->m=*m;
    shot[shot_anz]->speed=SHOTSPEED+speed;
    shot[shot_anz]->t=chrono;
    shot_anz++;
}

void proceed_shots()
{
   int i;
   for (i=0; i<shot_anz; i++)
   {
	  shot[i]->pos.x-=shot[i]->m.v[2][0]*shot[i]->speed*fmult;
	  shot[i]->pos.y-=shot[i]->m.v[2][1]*shot[i]->speed*fmult;
	  shot[i]->pos.z-=shot[i]->m.v[2][2]*shot[i]->speed*fmult;
	  if (chrono-shot[i]->t>SHOTTIME) kill_shot(i);
    }
}

void set_view_l()
{
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_l);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_l);
	glViewport(0, 113, SCREEN_W, SCREEN_H-113);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-init.mb/2+init.an, init.mb/2+init.an, -0.09175, 0.09175, init.db, init.far_clip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void move_camera_l()
{
    glMultMatrix_allegro(&camera.m);
	glTranslatef(	-camera.pos.x+camera.stereo.x,
			        -camera.pos.y+camera.stereo.y,
		            -camera.pos.z+camera.stereo.z);
    glPushMatrix();
}

void set_view_r()
{
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_r);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse_r);
	glViewport(0,  113, SCREEN_W, SCREEN_H-113);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-init.mb/2-init.an, init.mb/2-init.an, -0.09175, 0.09175, init.db, init.far_clip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void move_camera_r()
{
    glMultMatrix_allegro(&camera.m);
	glTranslatef(	-camera.pos.x-camera.stereo.x,
			        -camera.pos.y-camera.stereo.y,
		            -camera.pos.z-camera.stereo.z);
    glPushMatrix();
}
/*
void draw_object(short mod, short o, char c)
{
    int i, p;
 	for (p = 0; p < model[mod].o3d[o].poly_anz; p++)
	{ 
	    glNormal3fv(&model[mod].o3d[o].n[p].x);
		glColor3ubv(col[c][model[mod].o3d[o].poly[p].color]);
		glVertexPointer(3, GL_FLOAT, 0, model[mod].o3d[o].vert);
		glDrawElements(GL_POLYGON, model[mod].o3d[o].poly[p].ind_anz, GL_UNSIGNED_SHORT, model[mod].o3d[o].poly[p].i);
	}
}

*/
void draw_object(MODEL *mod, short o, char c)
{
    int i, p, j=0;
 	for (p = 0; p < mod->o3d[o].poly_anz; p++)
	{ 
	    glNormal3fv(&mod->o3d[o].n[p].x);
		glColor3ubv(col[c][mod->o3d[o].color[p]]);
		glVertexPointer(3, GL_FLOAT, 0, mod->o3d[o].vert);
		glDrawElements(GL_POLYGON, mod->o3d[o].ind_anz[p], GL_UNSIGNED_SHORT, &mod->o3d[o].i[j]);
		j+=mod->o3d[o].ind_anz[p];
	}
}

void draw_unit(short u, char c)
{
    int o;
    for (o=0; o<unit[u]->model->obj_anz;o++)
    {
       glPushMatrix();
       glTranslatef(unit[u]->pos.x, unit[u]->pos.y, unit[u]->pos.z);
//       glMultMatrix_allegro(&unit[u]->m);
//       glTranslatef(oposi[o].x, oposi[o].y, oposi[o].z);
       draw_object(unit[u]->model, o, c);
       glPopMatrix();
    }
}

void draw_object_lines(MODEL *mod, short o)
{
    glVertexPointer(3, GL_FLOAT, 0, mod->o3d[o].vert);
    glDrawElements(GL_LINES, mod->o3d[o].line_anz*2, GL_UNSIGNED_SHORT, mod->o3d[o].line);
}

void draw_unit_lines(short u)
{
    int o;
    for (o=0; o<unit[u]->model->obj_anz;o++)
    {
       glPushMatrix();
       glTranslatef(unit[u]->pos.x, unit[u]->pos.y, unit[u]->pos.z);
   //    glMultMatrix_allegro(&unit[u]->m);
   //    glTranslatef(oposi[o].x, oposi[o].y, oposi[o].z);
       draw_object_lines(unit[u]->model, o);
       glPopMatrix();
    }
}

void draw_engine_outline(short u, bool c)
{
    int i;
    for (i=0; i<MAXENGINE; i++)
    if (unit[u]->engine[i].active)
    {
   	   glColor3ubv(col[c][15-((chrono-unit[u]->engine[i].t)/25)]);
  // 	 glColor3ubv(col[c][15]);
       glPushMatrix();
       glTranslatef(unit[u]->engine[i].pos.x, unit[u]->engine[i].pos.y, unit[u]->engine[i].pos.z);
       glMultMatrix_allegro(&unit[u]->engine[i].m);
       draw_object_lines(unit[u]->model, unit[u]->model->engine);
       glPopMatrix();
    }
}

void start_engine_outline(short u)
{
    int i;
    for (i=0; i<MAXENGINE; i++)
    if (!unit[u]->engine[i].active)
    {
       unit[u]->engine[i].pos=unit[u]->pos;
       unit[u]->engine[i].m=unit[u]->m;
       unit[u]->engine[i].t=chrono;
  //     unit[u]->engine[i].speed=(unit[u]->speed_ist.z-unit[u]->speed_soll.z);
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
    /*   speed=unit[u]->engine[i].speed*fmult;
       unit[u]->engine[i].pos.x-=unit[u]->engine[i].m.v[2][0]*speed;
       unit[u]->engine[i].pos.y-=unit[u]->engine[i].m.v[2][1]*speed;
       unit[u]->engine[i].pos.z-=unit[u]->engine[i].m.v[2][2]*speed;
      */ if ((chrono-unit[u]->engine[i].t)>300)unit[u]->engine[i].active=0;
    }
}

void draw_scanner_point(VECTOR *heading)
{
	if(heading->z>0)
	{
		glViewport(17, 37, 43, 43);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-22, 22, -22, 22);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else
	{
		glViewport(582, 37, 43, 43);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-22, 22, -22, 22);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	glBegin(GL_POINTS);
	glVertex2i(-heading->x*22,-heading->y*22);
	glEnd();
	glPopMatrix();
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

void draw_char(char a)
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
       if(string[i]=='n')
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

void draw_monitor(short u)
{
    int i; int o=0; float monwink;
    VECTOR heading;
    get_heading(&unit[0]->m, &unit[0]->pos, &vorhalt, &heading);
	glViewport(265, 8, 162, 100);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-65, 65, -40, 40);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
//					printf("XXXXXXXXXXXXXXXXXX\n");

	monwink=atan(unit[u]->model->size/vector_length_f(camera.pos.x - unit[u]->pos.x, camera.pos.y - unit[u]->pos.y, camera.pos.z - unit[u]->pos.z))* 180 / M_PI*1.2;
	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(monwink, 1.902, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z, unit[u]->pos.x, unit[u]->pos.y, unit[u]->pos.z, unit[0]->m.v[1][0], unit[0]->m.v[1][1], unit[0]->m.v[1][2]);
	glPushMatrix();
	glColor4ub(100, 128, 0,0);
	glTranslatef(unit[u]->pos.x, unit[u]->pos.y, unit[u]->pos.z);
    glMultMatrix_allegro(&unit[u]->m);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	for (i=0; i<unit[u]->model->o3d[o].line_anz*2; i++)
	{
	    glVertex3fv(&unit[u]->model->o3d[o].vert[unit[u]->model->o3d[o].line[i]].x);
	} 
	glEnd();
	glPopMatrix();
	for (i=0; i<shot_anz; i++)
	if(shot[i]->property==0)
    {
	   glPushMatrix();
	   glTranslatef(shot[i]->pos.x, shot[i]->pos.y, shot[i]->pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
//	   glColor3ubv(col[c][15]);
	   draw_circle(0.2);
	   glPopMatrix();
	}
	glColor4ub(125, 160, 0,0);
/*	for (i=0; i<AL.explosion.anz; i++)
	{
	   if(!explosion[AL.explosion.i[i]].property)
	   {
	   glPushMatrix();
	   glTranslatef(explosion[AL.explosion.i[i]].pos.x, explosion[AL.explosion.i[i]].pos.y, explosion[AL.explosion.i[i]].pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
	   draw_circle_explode(explosion[AL.explosion.i[i]].mult);
	   glPopMatrix();
	   }
	}
*/
	
	
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
	glColor4ub(100,128, 0,0);
	glVertex3f( 10, sinus10[0][0]*10, sinus10[0][1]*10);
	glVertex3f(-10, sinus10[0][0]*10, sinus10[0][1]*10);
	glColor4ub(50, 64, 0,0);
	for (i=1; i<36; i++)
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
	glColor4ub(100,128, 0,0);
	glVertex3f(sinus10[0][0]*10, 10, sinus10[0][1]*10);
	glVertex3f(sinus10[0][0]*10,-10, sinus10[0][1]*10);
	glColor4ub(50, 64, 0,0);
	for (i=1; i<36; i++)
	{
		glVertex3f(sinus10[i][0]*10, 10, sinus10[i][1]*10);
		glVertex3f(sinus10[i][0]*10,-10, sinus10[i][1]*10);
	}
	glEnd();
	glPopMatrix();
	
}

void draw_monitor2()
{
    int i;
   	glViewport(265, 8, 175, 92);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(init.zoom, init.aspect, init.near_clip, init.far_clip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glMultMatrix_allegro(&camera.m);
	glTranslatef(	-camera.pos.x,
			        -camera.pos.y,
		            -camera.pos.z);
 	glColor4ub(100, 128, 0,0);
	for (i=0;i<RL.u.anz;i++) draw_unit_lines(RL.u.i[i]);
	
	for (i=0; i<shot_anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(shot[i]->pos.x, shot[i]->pos.y, shot[i]->pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
//	   glColor3ubv(col[c][15]);
	   draw_circle(0.2);
	   glPopMatrix();
	}
/*	for (i=0; i<RL.missile.anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(missile[RL.missile.i[i]].pos.x, missile[RL.missile.i[i]].pos.y, missile[RL.missile.i[i]].pos.z);
	   glMultMatrix_allegro(&missile[RL.missile.i[i]].m);
	   draw_object_lines(4, 0);
	   glPopMatrix();
	} */
	glColor4ub(125, 160, 0,0);
/*	for (i=0; i<AL.explosion.anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(explosion[AL.explosion.i[i]].pos.x, explosion[AL.explosion.i[i]].pos.y, explosion[AL.explosion.i[i]].pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
	   draw_circle_explode(explosion[AL.explosion.i[i]].mult);
	   glPopMatrix();
    } */
	
	
	glLoadIdentity();
	gluOrtho2D(-50, 50, -50, 50);
    glVertexPointer(3, GL_INT, 0, zielkreuz[0]);
    glDrawArrays(GL_LINES, 0, 8);
//    glDrawElements(GL_LINES, model[mod].o3d[o].line_anz*2, GL_UNSIGNED_SHORT, model[mod].o3d[o].line);

}


void draw_cockpit()
{
//glEnable(GL_BLEND);  
//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
allegro_gl_set_allegro_mode();
//masked_blit(bmp, screen, 0, 0, 0, 0, 640, 126); 
//draw_sprite(screen, bmp, 0, 354);
//draw_character(screen, bmp, 0, 354, 1);
blit(bmp, screen, 0, 0, 0, 367, 640, 113);
if(unit[0]->target->index!=NONE)blit(logo[unit[0]->target->party], screen, 0, 0, 267, 380, 30, 35);
blit(logo[0], screen, 0, 0, 463, 398, 30, 35);
//blit(rauschen, screen, rand()%170, rand()%100, 14, 461, 44, 13);
//blit(rauschen, screen, rand()%170, rand()%100, 583, 461, 44, 13);
 // 	glViewport(265, 8, 175, 92);

if(mrauschen>chrono)blit(rauschen, screen, rand()%84, rand()%164, 267, 380, 172, 90);

//allegro_gl_printf(f, 0, 0, 0, makecol(255, 255, 255), "hallo", fps_rate);

//masked_blit(bmp, screen, 0, 0, 0, 354, 640, 126); 
//...allegro functions...
allegro_gl_unset_allegro_mode(); 
//glDisable(GL_BLEND);

/*	glViewport(0, 0, 640, 126);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable (GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glColor4f(0, 0, 0, 1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f( 1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(-1,  1);
 	glEnd(); */
 //	glScalef(0.1, 0.1, 0.1);
 //	draw_integer(5);
//	glDisable (GL_TEXTURE_2D);
//	glDisable(GL_ALPHA_TEST);

glViewport(0, 0, 640, 480);
glDisable(GL_DEPTH_TEST);         // disable depth
glMatrixMode(GL_PROJECTION);      // switch to projection matrix
glLoadIdentity();                 // load identity matrix
    
glOrtho(0.0f, SCREEN_W, 0, SCREEN_H, -1.0f, 1.0f);
glMatrixMode(GL_MODELVIEW);       // back to modelview matrix for rendering the image
glLoadIdentity();                 // load identity matrix

//lRasterPos2f(0, 0);
//lDrawPixels(bmp->w, bmp->h, allegro_gl_bitmap_color_format(bmp), allegro_gl_bitmap_type(bmp), bmp->line[0]);
//glEnable(GL_DEPTH_TEST);         // disable depth
 
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


void draw_icosahedron(char c)
{
 //   glScalef(20,20,20);
 //   glColor3ubv(col[c][8]); 
  //  glVertexPointer(3, GL_FLOAT, 0, icosahedron_v);
 //   glDrawElements(GL_LINES, 60, GL_UNSIGNED_BYTE, icosahedron_i);
}
  
void render_pipeline(char c)
{
    int i;
/*	for (i=0; i<AL.explosion.anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(explosion[AL.explosion.i[i]].pos.x, explosion[AL.explosion.i[i]].pos.y, explosion[AL.explosion.i[i]].pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
	   draw_explode(AL.explosion.i[i], c);
	   glPopMatrix();
    } */
	for (i=0; i<shot_anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(shot[i]->pos.x, shot[i]->pos.y, shot[i]->pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
	   glColor3ubv(col[c][15]);
	   draw_circle(0.2);
	   glPopMatrix();
	}
	
/*	for (i=0; i<RL.missile.anz; i++)
	{
	   glPushMatrix();
	   glTranslatef(missile[RL.missile.i[i]].pos.x, missile[RL.missile.i[i]].pos.y, missile[RL.missile.i[i]].pos.z);
	   glMultMatrix_allegro(&missile[RL.missile.i[i]].m);
	   draw_object(4, 0, c);
	   draw_object(4, 1, c);
	   glPopMatrix();
	} */

	for (i=0;i<RL.u.anz;i++)
	{
	   if   (unit[RL.u.i[i]]->justhit &&!unit[RL.u.i[i]]->big) draw_unit(RL.u.i[i], c+2);
	   else                          draw_unit(RL.u.i[i], c);//, c);
    if(unit[RL.u.i[i]]->model->engine)
    draw_engine_outline(RL.u.i[i], c);
    }
    if (unit[0]->target!=NO_TARGET)
    {
       glDisable(GL_DEPTH_TEST);
	   glPushMatrix();
	   glTranslatef(unit[0]->target->pos.x, unit[0]->target->pos.y, unit[0]->target->pos.z);
	   glMultMatrix_allegro(&unit[0]->m);
	   glColor3ubv(col[c][13]);
	   draw_marker(unit[0]->target->model->size/2);
	   glPopMatrix();

	   glPushMatrix();
	   glTranslatef(vorhalt.x, vorhalt.y, vorhalt.z);
	   glMultMatrix_allegro(&unit[0]->m);
	   glColor3ubv(col[c][13]);
	   draw_marker(5);
	   glPopMatrix();
	   
	   
       draw_unit_lines(unit[0]->target->index);
              glEnable(GL_DEPTH_TEST);

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

    glPointSize(2.0);
    glPushMatrix();
    glMultMatrix_allegro(&camera.m);
    draw_stars(0);
    glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	move_camera_l();
	render_pipeline(0);
	glCallList(l_kam_list);
	glTranslatef(init.an, 0, 0);
    glMultMatrix_allegro(&camera.m);
 //	draw_crosses(0);

	////////////////////////////////////////////////////////////////////////////
	glClear(GL_DEPTH_BUFFER_BIT );
	glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
//lCallList(r_kam_list);
    set_view_r();

   glDisable(GL_DEPTH_TEST); 	

    glPointSize(2.0);
    glPushMatrix();
    glMultMatrix_allegro(&camera.m);
    draw_stars(1);
    glPopMatrix();
   	glEnable(GL_DEPTH_TEST);

/*	
	          glPushMatrix();
		      glMultMatrix_allegro(&camera.m);
		      draw_icosahedron(1);
		      glPopMatrix();
*/
	move_camera_r();
	render_pipeline(1);
	glCallList(r_kam_list);
	glTranslatef(-init.an, 0, 0);
	glMultMatrix_allegro(&camera.m);
 //	draw_crosses(1);
	////////////////////////////////////////////////////////////////////////////
	glViewport(0,  0, SCREEN_W, SCREEN_H);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	draw_cockpit();
	for (u=1;u<unit_anz; u++)
	{
		  glColor4ub(200, 255, 0,0);
		  glPointSize(2.0);
	      draw_scanner_point(&unit[u]->heading);
    }
    
   	for (u=0;u<shot_anz; u++)
	{
		  glColor4ub(200, 255, 0,0);
		  glPointSize(1.0);

	      draw_scanner_point(&shot[u]->heading);
    }
    
    glColor4ub(255, 255, 255,0);
    glPointSize(3.0);

    if(unit[0]->target!=NO_TARGET)draw_scanner_point(&unit[0]->target->heading);
    
    if(mrauschen<chrono)
    {
	   if (unit[0]->target!=NO_TARGET && !button_pressed)draw_monitor(unit[0]->target->index);
	   else draw_monitor2();
	}
	
	glViewport(0, 0, init.aufl_w, init.aufl_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0 , init.aufl_w, 0, init.aufl_h);
 	glColor4ub(100, 128, 0,0);
 	
 	glPushMatrix();
    glTranslatef(510,50,0);
	draw_integer(unit[0]->speed_soll.z);
 	glPopMatrix();
 	
 	glPushMatrix();
    glTranslatef(510,42,0);
	draw_integer(unit[0]->speed_ist.z);
 	glPopMatrix();
 	
 	glPushMatrix();
    glTranslatef(510,28,0);
	draw_integer(unit[0]->shield);
 	glPopMatrix();

 	if(unit[0]->target->index!=NONE && (mrauschen<chrono))
 	{
 	glPushMatrix();
    glTranslatef(412,20,0);
	draw_integer(unit[0]->target->speed_ist.z);
 	glPopMatrix();
  	glPushMatrix();
    glTranslatef(412,28,0);
	draw_integer(vector_length_f(unit[0]->target->pos.x-unit[0]->pos.x,
                                 unit[0]->target->pos.y-unit[0]->pos.y,
                                 unit[0]->target->pos.z-unit[0]->pos.z));
 	glPopMatrix();
  	glPushMatrix();
    glTranslatef(412,36,0);
	draw_integer(unit[0]->target->shield);
 	glPopMatrix();
	}
	glPushMatrix();
    glTranslatef(227,19,0);
	draw_integer(fps_rate);
 	glPopMatrix();
 		
	glPushMatrix();
    glTranslatef(227,35,0);
	draw_integer(stick.axis[0]*128);
	glPopMatrix();

	glPushMatrix();
    glTranslatef(100,70,0);
	draw_string("*quatsch mit soße,  ey!!!");
 	glPopMatrix();
 
	
 /*	
	glPushMatrix();
    glTranslatef(30,52,0);
	draw_integer(fmult*1000);
 	glPopMatrix();

	glPushMatrix();
    glTranslatef(600,52,0);
	draw_integer(time_to_impact(unit[0]->target)*1000);
 	glPopMatrix();
*/

	glFlush();
	allegro_gl_flip();
}


void align_target(short u)
{
    VECTOR heading;
    get_heading(&unit[u]->m, &unit[u]->pos, &unit[u]->target->pos, &heading);
    unit[u]->wink_soll=null;
    if (heading.x>0.01)
    { 
       if ((heading.z>0) && (heading.x<0.1)) unit[u]->wink_soll.y=4;
       else {unit[u]->wink_soll.y=MAXROT;unit[u]->wink_soll.z=5;}
    }
    else if (heading.x<-0.01)
    {
       if ((heading.z>0) && (heading.x>-0.1)) unit[u]->wink_soll.y=-4;
       else {unit[u]->wink_soll.y=-MAXROT;unit[u]->wink_soll.z=-5;}

    }
    if (heading.y>0.01)
    { 
       if ((heading.z>0) && (heading.y<0.1)) unit[u]->wink_soll.x=-4;
       else unit[u]->wink_soll.x=-MAXROT;
    }
    else if (heading.y<-0.01)
    {
       if ((heading.z>0) && (heading.y>-0.1)) unit[u]->wink_soll.x=4;
       else unit[u]->wink_soll.x=MAXROT;
    }
}
void turn_unit(short u)
{
    MATRIX_f m;
    accelerate(unit[u]->wink_soll.x, &unit[u]->wink_ist.x, ROTACC);
    accelerate(unit[u]->wink_soll.y, &unit[u]->wink_ist.y, ROTACC);
    accelerate(unit[u]->wink_soll.z, &unit[u]->wink_ist.z, ROTACC);
    unit[u]->speed_ist.z-=unit[u]->speed_ist.z*vector_length_f(unit[u]->wink_ist.x, unit[u]->wink_ist.y, 0)*ROTDEC*fmult;
    get_rotation_matrix_f(&m, unit[u]->wink_ist.x*fmult, unit[u]->wink_ist.y*fmult, unit[u]->wink_ist.z*fmult);
    matrix_mul_f(&unit[u]->m, &m,  &unit[u]->m);
}

void fire_at_target(short u)
{
    VECTOR heading;
    MATRIX_f m;

    get_heading(&unit[u]->m, &unit[u]->pos, &unit[u]->target->pos, &heading);
    if((heading.x<0.15) && (heading.x>-0.15)&& (heading.y<0.15)&& (heading.y>-0.15)&& (heading.z>0))
    {   
       if ( chrono-unit[u]->shotdelay > SHOOTRATE)
       {
          VECTOR emit; invert_matrix(&unit[u]->m, &m);
    //      if (unit[u]->gun)apply_matrix_f(&m, oposi[model[0].lemit].x,  oposi[model[0].lemit].y,  oposi[model[0].lemit].z, &emit.x, &emit.y, &emit.z);
    //      else apply_matrix_f(&m, oposi[model[0].remit].x,  oposi[model[0].remit].y,  oposi[model[0].remit].z, &emit.x, &emit.y, &emit.z);
          emit.x+=unit[u]->pos.x;
          emit.y+=unit[u]->pos.y;
          emit.z+=unit[u]->pos.z;
          start_shot(&emit, &unit[u]->m, u, unit[u]->speed_ist.z);              
          unit[u]->shotdelay=chrono;
          unit[u]->gun=1-unit[u]->gun;
          play_3D_sound(&unit[u]->pos, sound[1]);
       }
    }
}

void move_ship(short u)
{
    float speed;
	if (unit[u]->speed_soll.z != unit[u]->speed_ist.z)
	{
       accelerate(unit[u]->speed_soll.z, &unit[u]->speed_ist.z, TRANSACC);
    }
    speed=unit[u]->speed_ist.z*fmult;
    unit[u]->pos.x-=unit[u]->m.v[2][0]*speed;
    unit[u]->pos.y-=unit[u]->m.v[2][1]*speed;
    unit[u]->pos.z-=unit[u]->m.v[2][2]*speed;
}

void start_missile(short u, char status)
{
   
   unit[unit_anz]->type=UNIT_MISSILE;
   unit[unit_anz]->status=status;
   unit[unit_anz]->pos=unit[u]->pos;
   unit[unit_anz]->m=unit[u]->m;
   unit[unit_anz]->target=unit[u]->target;
   unit[unit_anz]->speed_ist.z=unit[u]->speed_ist.z+MISSILESTARTSPEED;
   unit[unit_anz]->speed_soll.z=MISSILEMAXSPEED;
   unit[unit_anz]->t=chrono;
   unit_anz++;
}

void user_input(short u)
{
    int i, input;
    input_delay++;
    poll_keyboard();
    if(keypressed())
    {
       input=readkey();
       switch(input >> 8)
       {
          case KEY_R:
	      unit[u]->target=get_nearest_enemy_target(u);
          break;
          
 	      case KEY_T:
	      get_next_target(u);
	      break;
	      
          case KEY_Y:
	      get_last_target(u);
	      break;

	      case KEY_O:
	      unit[u]->target=NO_TARGET;
          break;
          
          case KEY_X:
	      if (unit[u]->gun<2)unit[u]->gun=2;
	      else unit[u]->gun=0;
          break;

	      case KEY_ENTER:
          unit[u]->speed_soll.z = MAXSPEED;
          break;
          
  	      case KEY_BACKSPACE:
          unit[u]->speed_soll.z =0;
          break;
              
	      case KEY_PLUS_PAD:
          unit[u]->speed_soll.z++;
          if(unit[u]->speed_soll.z>MAXSPEED)unit[u]->speed_soll.z=MAXSPEED;
          break;
          
	      case KEY_MINUS_PAD:
          unit[0]->speed_soll.z--;
          if(unit[0]->speed_soll.z<0)unit[0]->speed_soll.z=0;
          break;

	      case KEY_J:
	      stick.state++;
	      if(stick.state>JOY_5)stick.state=JOY_1;
          break;
          
	      case KEY_E:
	      if(key_shifts & KB_SHIFT_FLAG) init.aa-=0.005;
	      else init.aa+=0.005;
	      init.an = init.aa/2;
          break;
          

    //      case default:
     //     break;              
       }
    }

	if (stick.present)
	{	 
	   refresh_stick();
       unit[u]->wink_soll.x = stick.axis[0]*MAXROT;
       unit[u]->wink_soll.y = stick.axis[1]*MAXROT;
       unit[u]->wink_soll.z = stick.axis[2]*MAXROT;
	   if (stick.b[0] && ((chrono-unit[u]->shotdelay) > 0))
	   {
	      if (unit[0]->gun==1)
	      {
	         start_shot(&unit[u]->remit, &unit[u]->m, 0, unit[u]->speed_ist.z);
	         play_sample (sound[1], 32, 32, 1000, 0);
	         unit[u]->gun=0;
             unit[u]->shotdelay = chrono+SHOOTRATE;
          }
          else if (!unit[0]->gun)
          {
             start_shot(&unit[u]->lemit, &unit[u]->m, 0, unit[u]->speed_ist.z);
             play_sample (sound[1], 32, 228, 1000, 0);
   	         unit[u]->gun=1;
             unit[u]->shotdelay = chrono+SHOOTRATE;
          }
          else
          {
             start_shot(&unit[u]->lemit, &unit[u]->m, 0, unit[u]->speed_ist.z);
             start_shot(&unit[u]->remit, &unit[u]->m, 0, unit[u]->speed_ist.z);
             play_sample (sound[1], 32, 128, 1000, 0);
             unit[u]->shotdelay = chrono+2*SHOOTRATE;
          }
       }
       if (stick.b[1] && ((chrono-unit[u]->shotdelay) > MISSILESHOOTRATE))
       {
          if(unit[u]->target && !button_pressed)start_missile(u, AI_DUMBFIRE);
          else start_missile(u, AI_DUMBFIRE);
          unit[u]->shotdelay = chrono;
       }
       if (stick.b[3])
       {
          if (!button_pressed)
          {
              button_pressed=1;
              button_delay=chrono;
          }
       }
       else if(button_pressed)
       {
          button_pressed=0;   
          if ((chrono-button_delay)<300)       
          {
             unit[u]->target=target_unit_near_reticule();
             mrauschen=chrono+120;
             play_sample (sound[6], 32, 128, 1000, 0);       
             button_delay=chrono-300;
          }
       }
    }
}


void proceed_player_ship(short u)
{
/*    MATRIX_f m;
    float speed;
    accelerate(unit[u]->wink_soll.x, &unit[u]->wink_ist.x, ROTACC);
    accelerate(unit[u]->wink_soll.y, &unit[u]->wink_ist.y, ROTACC);
    accelerate(unit[u]->wink_soll.z, &unit[u]->wink_ist.z, ROTACC);
    unit[u]->speed_ist.z-=unit[u]->speed_ist.z*vector_length_f(unit[u]->wink_ist.x, unit[u]->wink_ist.y, 0)*ROTDEC*fmult;
    get_rotation_matrix_f(&m, unit[u]->wink_ist.x*fmult, unit[u]->wink_ist.y*fmult, unit[u]->wink_ist.z*fmult);
    get_rotation_matrix_f(&camera.md, -unit[u]->wink_ist.x*fmult, -unit[u]->wink_ist.y*fmult, -unit[u]->wink_ist.z*fmult);
    matrix_mul_f(&m, &camera.m, &camera.m);
*/
    invert_matrix(&unit[u]->m, &camera.m);
    camera.pos=unit[u]->pos;
    apply_matrix_f(&camera.m, init.an, 0, 0, &camera.stereo.x, &camera.stereo.y, &camera.stereo.z);

//    adjust_sample(sound[0], 8, 128, 1000+unit[u]->speed_ist.z*5, TRUE);
//	apply_matrix_f(&camera.m, oposi[model[0].camera].x, oposi[model[0].camera].y, oposi[model[0].camera].z, &camera.pos.x, &camera.pos.y, &camera.pos.z);
//	apply_matrix_f(&camera.m, oposi[model[0].remit].x,  oposi[model[0].remit].y,  oposi[model[0].remit].z, &unit[u]->remit.x, &unit[u]->remit.y, &unit[u]->remit.z);
//	apply_matrix_f(&camera.m, oposi[model[0].lemit].x,  oposi[model[0].lemit].y,  oposi[model[0].lemit].z, &unit[u]->lemit.x, &unit[u]->lemit.y, &unit[u]->lemit.z);

/*	unit[u]->lemit.x= unit[u]->pos.x + unit[u]->lemit.x;
	unit[u]->lemit.y= unit[u]->pos.y + unit[u]->lemit.y;
	unit[u]->lemit.z= unit[u]->pos.z + unit[u]->lemit.z;
	camera.pos.x= unit[u]->pos.x + camera.pos.x;
	camera.pos.y= unit[u]->pos.y + camera.pos.y;
	camera.pos.z= unit[u]->pos.z + camera.pos.z;
	unit[u]->remit.x= unit[u]->pos.x + unit[u]->remit.x;
	unit[u]->remit.y= unit[u]->pos.y + unit[u]->remit.y;
	unit[u]->remit.z= unit[u]->pos.z + unit[u]->remit.z; */
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
          user_input(u);
          turn_unit(u);
          move_ship(u);
          proceed_player_ship(u);
          break;
          
          case AI_ATTACKING:
          align_target(u);
          turn_unit(u);
          fire_at_target(u);
          move_ship(u);
          if(unit[u]->model->engine)
          {
             proceed_engine_outline(u);
             if((chrono-unit[u]->enginetimer) > 100)
             {
                start_engine_outline(u);
                unit[u]->enginetimer=chrono;
             }
          }
          break;
          
          case AI_SEEKING:
          align_target(u);
          turn_unit(u);
          move_ship(u);
          if(unit[u]->model->engine)
          {
             proceed_engine_outline(u);
             if((chrono-unit[u]->enginetimer) > 100)
             {
                start_engine_outline(u);
                unit[u]->enginetimer=chrono;
             }
          }
          break;
          
          case AI_DUMBFIRE:
          move_ship(u);
          if(unit[u]->model->engine)
          {
             proceed_engine_outline(u);
             if((chrono-unit[u]->enginetimer) > 100)
             {
                start_engine_outline(u);
                unit[u]->enginetimer=chrono;
             }
          }
          break;
          
          case AI_SELFROTATE:
          matrix_mul_f(&unit[u]->selfrotate, &unit[u]->m, &unit[u]->m);
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


short init_collision_model(MODEL *mod)
{
    int p;
    colmodel[colmodel_anz] = newCollisionModel3D();
    for(p=0;p<mod->o3d[0].poly_anz;p++)
       colmodel[colmodel_anz]->addTriangle(&mod->o3d[0].vert[mod->o3d[0].i[p]].x,
                                           &mod->o3d[0].vert[mod->o3d[0].i[p+1]].x,
                                           &mod->o3d[0].vert[mod->o3d[0].i[p+2]].x);
    colmodel[colmodel_anz]->finalize();
    colmodel_anz++;
    return(colmodel_anz-1);
}

void init_3d_world()
{
    char u;
    
	lese_3do(model[0],  "3do/huede1.3do");
	lese_3do(model[1],  "3do/huede2.3do");
	lese_3do(model[2],  "3do/anthony02.3do");
	lese_3do(model[3],  "3do/huede1_col2.3do");
	lese_3do(model[4],  "3do/GryphRak1.3do");
	lese_3do(model[5],  "3do/station0.3do");
	lese_3do(model[6],  "3do/asteroid01.3do");
	lese_3do(model[7],  "3do/asteroid02.3do");
	lese_3do(model[8],  "3do/station0_col.3do");
	lese_3do(model[9],  "3do/anthony_col.3do");
	lese_3do(model[10], "3do/huede2_col.3do");
	lese_3do(model[11], "3do/asteroid03.3do");
	lese_3do(model[12], "3do/GryphRak1_col.3do");


	
	model[0]->colmodel=init_collision_model(model[3]);
	model[1]->colmodel=init_collision_model(model[10]);
	model[2]->colmodel=init_collision_model(model[9]);

	model[4]->colmodel=init_collision_model(model[12]);
	model[5]->colmodel=init_collision_model(model[8]);

	model[6]->colmodel=init_collision_model(model[6]);
	model[7]->colmodel=init_collision_model(model[7]);
	
	model[11]->colmodel=init_collision_model(model[11]);



	camera.m =identity_matrix_f;
	
	unit[unit_anz]->status=AI_PLAYER;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[0];
	get_z_rotate_matrix_f(&unit[unit_anz]->m, -30);
	unit[unit_anz]->pos.x = 0;
	unit[unit_anz]->pos.y = 0;
	unit[unit_anz]->pos.z = 30;
	unit[unit_anz]->target = 0;
	unit[unit_anz]->shield=100;
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=0;
	unit[unit_anz]->speed_soll.z=0;
	unit[unit_anz]->speed_ist.z=1;
	unit[unit_anz]->target=unit[1];
    unit_anz++;
    
	
 	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[0];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 0;
	unit[unit_anz]->pos.y = 10;
	unit[unit_anz]->pos.z = -140;
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->target = unit[0];
	unit[unit_anz]->shield=10;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=1;
    unit_anz++;

	/*
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[5];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 300;
	unit[unit_anz]->pos.y = 300;
	unit[unit_anz]->pos.z = -1700;
	get_z_rotate_matrix_f(&unit[unit_anz]->selfrotate, 0.3);
	unit[unit_anz]->vulnerable=0; 
	unit[unit_anz]->party=0;
	unit[unit_anz]->big=1;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[6];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 100;
	unit[unit_anz]->pos.y = 100;
	unit[unit_anz]->pos.z = -1000;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, 0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[7];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 100;
	unit[unit_anz]->pos.y = 50;
	unit[unit_anz]->pos.z = -1000;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, 0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 200;
	unit[unit_anz]->pos.y = 40;
	unit[unit_anz]->pos.z = -860;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, 0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 120;
	unit[unit_anz]->pos.y = -40;
	unit[unit_anz]->pos.z = -460;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, -0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[7];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 0;
	unit[unit_anz]->pos.y = 10;
	unit[unit_anz]->pos.z = -360;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, 0.1);
 	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[6];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = -40;
	unit[unit_anz]->pos.y = 20;
	unit[unit_anz]->pos.z = -1300;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, -0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[7];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 280;
	unit[unit_anz]->pos.y = 300;
	unit[unit_anz]->pos.z = -1600;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, -0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 268;
	unit[unit_anz]->pos.y = 250;
	unit[unit_anz]->pos.z = -1500;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, 0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_SELFROTATE;
	unit[unit_anz]->type=UNIT_OBJECT;
	unit[unit_anz]->model = model[11];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = -20;
	unit[unit_anz]->pos.y = -10;
	unit[unit_anz]->pos.z = -100;
	get_y_rotate_matrix_f(&unit[unit_anz]->selfrotate, -0.1);
	unit[unit_anz]->vulnerable=0;
	unit[unit_anz]->party=2;
    unit_anz++;

	
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->target = unit[1];
	unit[unit_anz]->model = model[1];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 20;
	unit[unit_anz]->pos.y = 0;
	unit[unit_anz]->pos.z = -50;
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->shield=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=0;
    unit_anz++;

	
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->target = unit[0];
	unit[unit_anz]->model = model[0];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 0;
	unit[unit_anz]->pos.y = -10;
	unit[unit_anz]->pos.z = -120;
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->shield=100;
	unit[unit_anz]->vulnerable=1;
	unit[unit_anz]->party=1;
    unit_anz++;

	
	unit[unit_anz]->status=AI_ATTACKING;
	unit[unit_anz]->type=UNIT_FIGHTER;
	unit[unit_anz]->model = model[12];
	unit[unit_anz]->m =identity_matrix_f;
	unit[unit_anz]->pos.x = 0;
	unit[unit_anz]->pos.y = 0;
	unit[unit_anz]->pos.z = 200;
	unit[unit_anz]->party=0;
	unit[unit_anz]->target=unit[13];
	unit[unit_anz]->speed_soll.z=50;
	unit[unit_anz]->shield=100;
    unit_anz++;
    */
}



void calc_reticule()
{
    float dist, t, vorhalt;
    dist=vector_length_f(unit[0]->pos.x-unit[0]->target->pos.x,
                         unit[0]->pos.y-unit[0]->target->pos.y,
                         unit[0]->pos.z-unit[0]->target->pos.z);
    t=dist/(SHOTSPEED+unit[0]->speed_ist.z);
    vorhalt=unit[0]->target->speed_ist.z*t;
    reticule.pos.x=unit[0]->target->pos.x-unit[0]->target->m.v[2][0]* vorhalt;
    reticule.pos.y=unit[0]->target->pos.y-unit[0]->target->m.v[2][1]* vorhalt;
    reticule.pos.z=unit[0]->target->pos.z-unit[0]->target->m.v[2][2]* vorhalt;
}
    
void init_gfx()
{
	allegro_gl_clear_settings();
	set_color_depth(init.farbtiefe);
	allegro_gl_set (AGL_COLOR_DEPTH, init.farbtiefe);
	allegro_gl_set (AGL_Z_DEPTH, init.z_puffer);
	allegro_gl_set (AGL_DOUBLEBUFFER, 1);
	allegro_gl_set (AGL_RENDERMETHOD, 1);
	allegro_gl_set (GFX_OPENGL_FULLSCREEN, TRUE);
	allegro_gl_use_mipmapping(TRUE);
 	if (set_gfx_mode(GFX_OPENGL, init.aufl_w, init.aufl_h, 0, 0) != 0)
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
    glEnableClientState (GL_VERTEX_ARRAY);
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

		collision_check();
		proceed_shots();

//		proceed_explode();
  //      proceed_actions(unit);
		proceed_debris();
		proceed_units();

		get_headings();
		make_RL();
        if(unit[0]->target->index!=NONE)calc_vorhalt(unit[0]->target->index);
//		printf("\n test2000 \n");
		render();
	}
}



int main(void)
{
    NO_TARGET->index=NONE;
	allegro_init();
	install_allegro_gl();
	install_keyboard();
	init_pointers();
	lese_ini("init.ini", &init);
	init_gfx();
	init_3d_world();
	install_int(the_timer, 1);
	LOCK_FUNCTION(the_timer);
	LOCK_VARIABLE(chrono);
	init_stick();
	stick.state=JOY_2;
	load_bitmaps();
	init_sinus10();
	init_lighting();
//	init_display_lists();
//	init_debris();
//	init_crosses();
	init_stars();
	init_sound();
	lese_3do(&font3d, "3do/font.3do");
	lese_3do(&ziffern, "3do/ziffern.3do");

	play_sample (sound[0], 8, 128, 1000, TRUE);
//    printf(" %d \n", allegro_gl_is_extension_supported ("GL_ARB_vertex_buffer_object"));
    fcount=chrono;
    game_loop();
	destroy_samples();
    destroy_bitmaps();
//	allegro_exit();
	return 0;
}
END_OF_MAIN();
