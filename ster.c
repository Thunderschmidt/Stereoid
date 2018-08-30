/* Hier wird alles Stereoskopische erledigt.
 * - Es werden Berechnungen ausgeführt
 * - Es werden die 3D-Farben initiiert.
 */

#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "mat.h"
#include "Trans_3D.h"
#include "ster.h"

volatile int *Chrono4;
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

void update_color_fx(int chrono, float fmult)
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