#include <stdio.h>
/* Der Joystick
 *
 */

#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "harbst.h"
#include "stick.h"

#define SQR2_128 11.3137085
#define SQR3_128 25.39841683
#define SQR4_128 38.05462768
#define SQR5_128 48.50293013
#define TOTZONE                10

void init_stick(STICK *stick)
{
	stick->present=FALSE;
	if (install_joystick(JOY_TYPE_AUTODETECT) != 0) return;
	if (!num_joysticks) return;
	poll_joystick();
    stick->cy=joy[0].stick[0].axis[0].pos;
	stick->cx=joy[0].stick[0].axis[1].pos;
	stick->cz=0;
	stick->present=TRUE;
	stick->state=JOY_2;
	return;
}


void refresh_stick(STICK *stick)
{
    int i;
    signed char sign;
    poll_joystick();
    stick->axis[0]=joy[0].stick[0].axis[1].pos-stick->cx;
    stick->axis[1]=-joy[0].stick[0].axis[0].pos-stick->cy;
    stick->axis[2]=joy[0].stick[0].axis[2].pos-stick->cz;
    stick->b[0]=joy[0].button[0].b;
    stick->b[1]=joy[0].button[1].b;
    stick->b[2]=joy[0].button[2].b;
    stick->b[3]=joy[0].button[3].b;


    stick->head_x=joy[0].stick[1].axis[0].pos>>7;
    stick->head_y=joy[0].stick[1].axis[1].pos>>7;
    for(i=0;i<3;i++) if((stick->axis[i]<TOTZONE)&&(stick->axis[i]>-TOTZONE))stick->axis[i]=0;
    
   	switch(stick->state)
    {
        case JOY_1:
             break;
        case JOY_2:
             for(i=0;i<3;i++)
             {
                sign=(stick->axis[i]>0)*2-1;
                stick->axis[i]/=SQR2_128;
                stick->axis[i]*=stick->axis[i]*sign;
             }
             break;
        case JOY_3:
             for(i=0;i<3;i++)
             {
                stick->axis[i]/=SQR3_128;
                stick->axis[i]*=stick->axis[i]*stick->axis[i];
             }
             break;
        case JOY_4:
             for(i=0;i<3;i++)
             {
                sign=(stick->axis[i]>0)*2-1;
                stick->axis[i]/=SQR4_128;
                stick->axis[i]*=stick->axis[i]*stick->axis[i]*stick->axis[i]*sign;             }
             break;
        case JOY_5:
             for(i=0;i<3;i++)
             {
                stick->axis[i]/=SQR5_128;
                stick->axis[i]*=stick->axis[i]*stick->axis[i]*stick->axis[i]*stick->axis[i];
             }
             break;             
             
    }
    for(i=0;i<3;i++) stick->axis[i]/=128;
}
     
