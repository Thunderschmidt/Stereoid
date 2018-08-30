#ifndef MATHEMATICS
#define MATHEMATICS
void init_mathematics(float *fmult);
void get_heading(MATRIX_f *m1, MATRIX_f *m2, VECTOR *out);
void invert_matrix(MATRIX_f *in, MATRIX_f *out);
bool accelerate(float soll, float *ist, float a);
void get_matrix_delta(MATRIX_f *m_act, MATRIX_f *m_old, MATRIX_f *m_delta);
float get_approaching_speed(MATRIX_f *m1, float *speed1, MATRIX_f *m2, float *speed2);
void glMultMatrix_allegro(MATRIX_f *m);
void zero_matrix(MATRIX_f *m);
VECTOR trans_matrix_to_vector(MATRIX_f *m);
void translate_matrix_v(MATRIX_f *m, VECTOR *pos);
float distance(VECTOR *pos1, VECTOR *pos2);
#endif


