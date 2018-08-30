#ifndef SHIPS
#define SHIPS
void prepare_raptor(UNIT *u);
void raptor_extend_gear(UNIT *u);
void raptor_retract_gear(UNIT *u);
void raptor_fire_left_gun(UNIT *u);
void raptor_fire_right_gun(UNIT *u);
void raptor_fire_guns(UNIT *u);
void raptor_open_cabin(UNIT *u);
void raptor_close_cabin(UNIT *u);
char raptor_fire_missile(UNIT *u, char type);
void the_raptor(UDATA *ud);
void the_seeker_missile(UDATA *ud);
void the_dumb_missile(UDATA *ud);

void animate_door(UNIT *u);
void animate_light(void*);

#endif

