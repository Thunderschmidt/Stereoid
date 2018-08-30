#ifndef JSTICK
#define JSTICK

typedef struct STICK
{
    bool present;
    float   axis[3];
    bool    b[4];
    int    head_x;
    int    head_y;
    char    state;
    signed char cx, cy, cz;
} STICK;

enum JOY_STATUS
{
    JOY_1 = 0,
    JOY_2,
    JOY_3,
    JOY_4,
    JOY_5
};

void init_stick(STICK *stick);
void refresh_stick(STICK *stick);
#endif
