/* Funktionen zum Aufschalten von units
 *
 *
 */

#include <stdio.h>
#include <math.h>

#include <allegro.h>
#include <alleggl.h>
#include <GL/glu.h>
#include "coldet.h"
#include "global.h"
#include "mat.h"
#include "usel.h"


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


