#ifndef MAT
#define MAT
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
VECTOR get_normal_vector(VECTOR *pos0, VECTOR *pos1, VECTOR *pos2);
void cross_product_v(VECTOR *pos0, VECTOR *pos1, VECTOR *pos2, VECTOR *out);
VECTOR vect(float x, float y, float z);
float vector_product_v(VECTOR *v1, VECTOR *v2);
float time_to_impact(UNIT *u1, UNIT *u2);
MATRIX_f matr(float x, float y, float z, float xrot, float yrot, float zrot);
void vector_add(VECTOR *in1, VECTOR *in2, VECTOR *out);
void vector_sub(VECTOR *in1, VECTOR *in2, VECTOR *out);
void vector_mul(VECTOR *v1, float mul);
void apply_matrix_v(MATRIX_f *m, VECTOR *in, VECTOR *out);
void normalize_vector_v(VECTOR *v1);
void calc_vorhalt(VECTOR *pos, UNIT *u, float t);
bool spot_is_visible(VECTOR *heading, float bounding);
float vector_length_v(VECTOR *v1);
VECTOR v_mul(VECTOR *in, float mul);
MATRIX_f align_matrix_z(VECTOR z);
void get_heading(MATRIX_f *m, VECTOR *in, VECTOR *out);
VECTOR operator+(VECTOR a, VECTOR b);
VECTOR operator+=(VECTOR a, VECTOR b);
VECTOR operator*(VECTOR a, float b);
#endif


