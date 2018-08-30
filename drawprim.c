/* Allerlei Zeichen-Funktionen.
 * u.a. auch draw_string()
 *
 */


#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "global.h"
#include "mat.h"
#include "drawprim.h"

GLubyte      circleindex[45];
float        sinus8[45][2];
MODEL3D_CLASS       font3d;
//MODEL       ziffern;

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
	glFrontFace(GL_CCW);

    glScalef(scale, scale, scale);
    glIndexPointer(GL_UNSIGNED_BYTE, 0, circleindex);
    glVertexPointer(2, GL_FLOAT, 0, sinus8[0]);
    glDrawElements(GL_POLYGON, 45, GL_UNSIGNED_BYTE, circleindex);
	glFrontFace(GL_CW);

}

void draw_circle_outline(float scale)
{
    glIndexPointer(GL_UNSIGNED_BYTE, 0, circleindex);
    glVertexPointer(2, GL_FLOAT, 0, sinus8[0]);
    glScalef(scale, scale, scale);
    glDrawElements(GL_LINE_LOOP, 45, GL_UNSIGNED_BYTE, circleindex);
}

void draw_arrow()
{
	glDisable(GL_DEPTH_TEST);
    glBegin(GL_TRIANGLES);
    glVertex2f(0,0);
    glVertex2f(16,20);
    glVertex2f(-16,20);
    glEnd();
   	glEnable(GL_DEPTH_TEST);
}

void draw_scanner_point(VECTOR *heading)
{
	if(heading->z>0)
	{
		glViewport(100, 20, 64, 64);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-32, 32, -32, 32);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else
	{
		glViewport(170, 20, 64, 64);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-32, 32, -32, 32);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	glBegin(GL_POINTS);
	glVertex2i(-heading->x*30,-heading->y*30);
	glEnd();
	glPopMatrix();
}

void draw_icosahedron(char c)
{
/*    glScalef(20,20,20);
    glColor3ubv(col[c][8]); 
    glVertexPointer(3, GL_FLOAT, 0, icosahedron_v);
    glDrawElements(GL_LINES, 60, GL_UNSIGNED_BYTE, icosahedron_i); */
}


void draw_energy_bar(float width, float percent)
{
    int i;
    glBegin(GL_LINES);
    for(i=0;i<percent;i+=2)
    {
	   glVertex2f(0, i);
	   glVertex2f(width, i);
    }
    glEnd();
}

GLuint flashtex[2];
#define FLASHSIZE 128
static GLubyte flashpic[2][FLASHSIZE][FLASHSIZE][4];

void init_flash_texture()
{
    int x, y, alpha, middle=FLASHSIZE/2;
    float al;
    float mul=256.0/middle;
    for(x=0;x<FLASHSIZE;x++)
    for(y=0;y<FLASHSIZE;y++)
    {
       al=255-vector_length_f(middle-x, middle-y, 0.0)*mul;
       alpha=al;
       if(alpha<0)alpha=0;
       flashpic[0][x][y][0]=160;
       flashpic[0][x][y][1]=  5;
       flashpic[0][x][y][2]= 20;
       flashpic[0][x][y][3]=alpha;

       flashpic[1][x][y][0]=  0;
       flashpic[1][x][y][1]=  0;
       flashpic[1][x][y][2]=240;
       flashpic[1][x][y][3]=alpha;
    }
    
glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
glGenTextures(1, &flashtex[0]);
glBindTexture(GL_TEXTURE_2D, flashtex[0]);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, flashpic[0]);

glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
glGenTextures(1, &flashtex[1]);
glBindTexture(GL_TEXTURE_2D, flashtex[1]);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, flashpic[1]);

}

void draw_flash()
{
//	glPolygonMode(GL_FRONT, GL_POINT);            //F�lle die sichtbaren Polygone aus
//	glEnable (GL_BLEND);
	glColor4f(1, 1, 1, 1);

	glEnable (GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_BLEND, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, flashtex[get_eye()]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glTexCoord2f(1, 0);
	glVertex2f(-1, 1);
	glTexCoord2f(1, 1);
	glVertex2f( 1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(1,  -1);
 	glEnd(); 
	glDisable (GL_TEXTURE_2D);
//	glPolygonMode(GL_FRONT, GL_FILL);            //F�lle die sichtbaren Polygone aus

}
/*
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
*/




void draw_char(unsigned char a)
{
   if((a >= 33) && (a <=96)) a-= 33;
   else if((a >= 97) && (a <=122)) a-= 65;
   else if((a >= 123) && (a <=128)) a-= 63;
   else if (a=='�') a=71;
   else if (a=='�' ||a=='�') a=72;
   else if (a=='�' ||a=='�') a=73;
   else if (a=='�' ||a=='�') a=74;
   else if (a==223) a=73;
   glRotatef(180.0, 0.0, 1.0, 0.0);
   glVertexPointer(3, GL_FLOAT, 0, font3d.o3d[a].vert);
   glDrawElements(GL_QUADS, 4*font3d.o3d[a].poly_anz, GL_UNSIGNED_SHORT, &font3d.o3d[a].i[0]);
   glRotatef(-180.0, 0.0, 1.0, 0.0);
}

void draw_string(char string[])
{
     glFrontFace(GL_CCW);
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
    glFrontFace(GL_CW);
}

void translate_target_pointer(VECTOR heading, float x, float y, float aspect)
{
    heading.z=0;
    normalize_vector_v(&heading);    
    if(heading.x<-aspect)
      glTranslatef(x,heading.y/heading.x*x,0);    
    else if(heading.x>aspect)
      glTranslatef(-x,-heading.y/heading.x*x,0);    
    else if(heading.y>=0)
      glTranslatef(-heading.x/heading.y*y, -y,0);    
    else if(heading.y<0)
      glTranslatef(heading.x/heading.y*y, y,0);   
}

void init_drawprim()
{
	char a;
	for(a=0; a<45; a++)
	{
		sinus8[a][0]=sin( DEG_2_RAD(a*8));
		sinus8[a][1]=cos(  DEG_2_RAD(a*8));
		circleindex[a]=44-a;
	}
	init_flash_texture();
	font3d.assign_3do("3do/font.3do");

}
