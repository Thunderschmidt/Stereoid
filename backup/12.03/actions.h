#ifndef ACTIONS
#define ACTIONS
void kill_unit_actions(UNIT *unit);
void init_actions(volatile int *chrono1, float *fmult1);
void proceed_actions();
bool wait_for_move(UNIT *unit, const char name[], char axis);
bool wait_for_turn(UNIT *unit, const char name[], char axis);
void wait(UNIT *unit, int t);
void spin(UNIT *unit, const char name[], char axis, float speed);
void turn(UNIT *unit, const char name[], short axis, float amount, float speed);
void move(UNIT *unit, const char name[], short axis, float amount, float speed);
void stop_spin (UNIT *unit, short o, short axis);
void stop_turn (UNIT *unit, short o, short axis);
bool wait_for_move(UNIT *unit, const char name[], char axis);
int get_a_action_anz();
int get_i_action_anz();
char get_object_by_name(MODEL *model, const char name[]);
void hide(UNIT *unit, const char name[]);
void show(UNIT *unit, const char name[]);
void hide(UNIT *unit, int o);
int prepare_object_matrices(UNIT *u);
MATRIX_f get_object_matrix(UNIT *u, short o_soll);
void get_object_matrix_world(MATRIX_f *m, UNIT *u, short o);
void start_function(UNIT *unit, ANIM_FUNCT anim_funct);
void hide_grid(UNIT *unit, const char name[]);
void show_grid(UNIT *unit, const char name[]);
void hide_fill(UNIT *unit, const char name[]);
void show_fill(UNIT *unit, const char name[]);

#endif


