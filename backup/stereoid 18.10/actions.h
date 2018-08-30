#ifndef ACTIONS
#define ACTIONS

void kill_unit_actions(UNIT *unit);
void init_actions(volatile int *chrono1, float *fmult1);
void proceed_actions();
bool wait_for_move(UNIT *unit, short o, char axis);
void wait(UNIT *unit, int t);
void spin(UNIT *unit, short o, char axis, float speed);
void turn(UNIT *unit, short o, short axis, float amount, float speed);
void move(UNIT *unit, short o, short axis, float amount, float speed);
void stop_spin (UNIT *unit, short o, short axis);
void stop_turn (UNIT *unit, short o, short axis);
void stop_move (UNIT *unit, short o, short axis);
int get_a_action_anz();
int get_i_action_anz();

#endif


