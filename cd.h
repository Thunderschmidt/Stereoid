#ifndef CD
#define CD

#include "coldet.h"

class COLMODEL_CLASS
{
    public:
    CollisionModel3D* ColModel3d;
    bool check_projectile(MATRIX_f *pro_m, float pro_speed, float pro_length, MATRIX_f *col_m, float col_speed);
    bool check_colmodel(MATRIX_f *m1, VECTOR *flightdir1, COLMODEL_CLASS *colmodel2, MATRIX_f *m2, VECTOR *flightdir2);
    VECTOR *n;
    const char *name; 
};

bool check_col(MATRIX_f *m1, MATRIX_f *m2, float bounding);
COLMODEL_CLASS* assign_colmodel(const char name[]);
#endif

