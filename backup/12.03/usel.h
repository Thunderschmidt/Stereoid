#ifndef USEL
#define USEL
void get_next_target(UNIT *u);
void get_last_target(UNIT *u);
UNIT *get_nearest_enemy_target(UNIT *u);
UNIT *target_unit_near_reticule();
#endif
