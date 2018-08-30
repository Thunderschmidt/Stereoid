#ifndef CD
#define CD
//bool cd_fighter_object(UNIT *u1, UNIT *u2);
bool cd_shot_unit(SHOT *sht,UNIT *u);
bool cd_missile_unit(UNIT *u1, UNIT *u2);
void init_cd(float *fmult1);
bool check_col(MATRIX_f *m1, MATRIX_f *m2, float bounding);
#endif

