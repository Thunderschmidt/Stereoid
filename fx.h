#ifndef FX
#define FX

typedef struct EXPLOSION2
{
    UNIT *property;      //wer löste die Explosion aus
    short total;         //wie viele Partikel sind aktiv?
    int t;               //wie alt ist die Explosion?
    float *particles;    //Zeiger auf Array mit Partikel-Geschwindigkeiten
    float mult;          //wie weit ist die Explosion schon ausgedehnt?
    float speed;
    VECTOR pos;
    MATRIX_f m;
}EXPLOSION2;


typedef struct EXPLOSION1
{
    short total;
    int t;
    float *particles;
    float mult;
    float speed;
    VECTOR flightdir;
    MATRIX_f m;
}EXPLOSION1;


typedef struct FLUSH
{
    int t;
    VECTOR pos;
    char type;
    float scale;
} FLUSH;

class FLUSH_CLASS  //Klasse für die aufleuchtenden Lichtfleckchen
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

class EXPLOSION1_CLASS  //Klasse für eine direktionale Explosion
{
    private:
    int total;                               //wie viele Explosionen sind aktiv?
    int particle_total;                      //wie viele Partikel hat eine Explosion?
    MATRIX_f     *particles;                 //Zeiger auf Array mit Zufalls-Matritzen
    EXPLOSION1   *explosion1[MAXEXPLOSIONS]; //Zeiger auf Array mit Explosionsdaten
    MODEL *model;                             //welches 3d-Modell wird verwendet?
    void kill(short i);
    public:
    void init(const char model_name[], int part_total);
    void draw();
    void start(VECTOR *flightdir, MATRIX_f *m);
    void proceed();
};

class EXPLOSION2_CLASS  //Klasse für  eine nichtdirektionale Explosion
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
	 MODEL *model;
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
    MODEL *model;
    MATRIX_f *m;
    public:
    void init(const char model_name[], MATRIX_f *pcm);
    void draw();
};

class SHOT_CLASS
{
    private:
    MODEL *model;
    public:
    int total;
    SHOT *shot[MAXSHOTS];
    void init(char model_name[]);
    void kill(short i);
    void proceed();
    void start(UNIT *u, const char name[], WDATA *wdata, signed char sign);
    void draw();
};

class EXPLOSION3_CLASS  //Klasse für direktionale Explosionen
{
    private:
    int total;
    int particle_total;
    VECTOR     *particles;
    EXPLOSION1   *explosion1[MAXEXPLOSIONS];
    void kill(short i);
    public:
    void init(int part_total);
    void draw(MATRIX_f *camera_matrix);
    void start(VECTOR *flightdir, MATRIX_f *m);
    void proceed();
};

#endif
