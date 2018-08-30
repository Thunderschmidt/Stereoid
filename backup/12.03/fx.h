#ifndef FX
#define FX

void init_fx(float *fmult, volatile int *chrono);

class FLUSH_CLASS
{
    private:
    int total;
    FLUSH *flush[MAXEXPLOSIONS];
    MATRIX_f *zero_m;
    void kill(short i);
    public:
    void init(MATRIX_f *m);
    void start(VECTOR *pos, char type);
    void proceed();
    void draw();
};

class EXPLOSION1_CLASS
{
    private:
    int total;
    int particle_total;
    MATRIX_f     *particles;
    EXPLOSION1   *explosion1[MAXEXPLOSIONS];
    MODEL model;
    void kill(short i);
    public:
    void init(const char model_name[], int part_total);
    void draw();
    void start(VECTOR *flightdir, MATRIX_f *m);
    void proceed();
};

class EXPLOSION2_CLASS
{
    private:
    int total;
    int particle_total;
    VECTOR *particles;
    MATRIX_f *zero_m;
    EXPLOSION2 *explosion2[MAXEXPLOSIONS];
    void kill(short i);
    public:
    void init(int part_total, MATRIX_f *m);
    void draw();
    void proceed();
    void start(MATRIX_f *m, float *speed, UNIT *property);
};

class DEBRIS_CLASS
{
    private:
    MATRIX_f *rotation;
    MATRIX_f *debris;
	 MODEL model;
    int total;
    int rotation_total;
    public:
    void init(const char model_name[], int debris_total, int rot_total, float max_rotation_speed);
    void proceed(MATRIX_f *m);
    void draw();
};

class BACKGROUND_CLASS
{
    private:
    MODEL model;
    MATRIX_f *m;
    public:
    void init(const char model_name[], MATRIX_f *pcm);
    void draw();
};

class SHOT_CLASS
{
    private:
    MODEL model;
    public:
    int total;
    SHOT *shot[MAXSHOTS];
    void init(char model_name[]);
    void kill(short i);
    void proceed();
    void start(UNIT *u, const char name[], char type, signed char sign);
    void draw();
};

class EXPLOSION3_CLASS
{
    private:
    int total;
    int particle_total;
    MATRIX_f     *particles;
    EXPLOSION1   *explosion1[MAXEXPLOSIONS];
    MODEL model;
    void kill(short i);
    public:
    void init(const char model_name[], int part_total);
    void draw();
    void start(VECTOR *flightdir, MATRIX_f *m);
    void proceed();
};

#endif
