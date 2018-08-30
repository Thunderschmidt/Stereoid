#ifndef JSTICK
#define JSTICK
    
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
