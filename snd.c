/* Hier steht alles, was mit Sound und Musik zu tun hat
 *
 *
 */

//#define MUSIK
#include <aldumb.h>
#include <stdio.h>
#include <math.h>
#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "harbst.h"
#include "snd.h"
DUH *musicpiece;
AL_DUH_PLAYER *dp;



void get_sound_heading(MATRIX_f m, VECTOR *in, VECTOR *out)
{
    m.t[0]=0;
    m.t[1]=0;
    m.t[2]=0;
    out->x=m.t[0]-in->x;
    out->y=m.t[1]-in->y;
    out->z=m.t[2]-in->z;
    apply_matrix_f(&m, out->x, out->y, out->z, &out->x, &out->y, &out->z);
    normalize_vector_f(&out->x, &out->y, &out->z);
}

void init_sound()
{
    #ifdef MUSIK
    atexit(&dumb_exit);
   	#endif
    if (install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL) < 0)
    {
       printf ("Error initializing sound card");
    }
    #ifdef MUSIK
    dumb_register_stdfiles();
    musicpiece = dumb_load_mod("mod/3d_demo_tune.mod");
    dp=al_start_duh(musicpiece, 1, 0, 1.0, 4096, 22050);
   	#endif
}

void proceed_music()
{
    #ifdef MUSIK
    al_poll_duh(dp);
    #endif
}

void terminate_sound()
{
    #ifdef MUSIK
    al_stop_duh(dp);
    unload_duh(musicpiece);
	#endif
}

SOUND_CLASS::SOUND_CLASS(const char filedir[], MATRIX_f *m, VECTOR *pos, float max_distance)
{
    total=0;
    dir=filedir;
    camera_m=m;
    camera_pos=pos;
    maxdist=max_distance;
    maxdist2=maxdist*maxdist;
   
}

SOUND_CLASS::~SOUND_CLASS()
{
    int i;
    for(i=0;i<total;i++)
    destroy_sample(sample[i]);
}

int SOUND_CLASS::load(const char filename[])
{
    char buf[32];
    sprintf(buf, "%s%s.wav", dir, filename);
    if(!(sample[total]=load_wav(buf))) return(-1);
    name[total]=filename;
    index[total]=total;
    total++;
    return(total-1); 
}

void SOUND_CLASS::play(int i, VECTOR *pos)
{
    float dist;
    dist=vector_length_f(camera_pos->x - pos->x, camera_pos->y - pos->y, camera_pos->z - pos->z);
//    dist=distance(camera_pos, pos);
    if (dist <= maxdist)
    {
       VECTOR heading;
       float vol=8*maxdist/(dist+0.1);
       if(vol>224)vol=224;
       get_sound_heading(*camera_m, pos, &heading);
       play_sample (sample[i], vol, 128-128*heading.x, 1000, 0);
    }
}


void SOUND_CLASS::play(const char filename[], VECTOR *pos)
{
    int i;
    for(i=0;i<total;i++)
    if(!strcmp(filename, name[i]))
    {
       play(i, pos);
       break;
    }
}

void SOUND_CLASS::play(const char filename[], float pos[3])
{
    int i;
    for(i=0;i<total;i++)
    if(!strcmp(filename, name[i]))
    {
       play(i, (VECTOR*)pos);
       break;
    }
}

void SOUND_CLASS::play(const char filename[], unsigned char volume, unsigned char pan, int speed, bool loop)
{
    int i;
    for(i=0;i<total;i++)
    if(!strcmp(filename, name[i]))
    {
       play_sample (sample[i], volume, pan, speed, loop);
       break;
    }
}

void SOUND_CLASS::adjust(int sound_nr, unsigned char volume, unsigned char pan, int speed, bool loop)
{
    adjust_sample(sample[sound_nr], volume, pan, speed, loop);
}
