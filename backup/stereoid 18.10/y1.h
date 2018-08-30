
const GLubyte col[4][16][3] =
{
{{ 0, 40, 40},
{ 11, 40, 40},
{ 21, 40, 40},
{ 32, 40, 40},
{ 43, 40, 39},
{ 53, 40, 39},
{ 64, 39, 39},
{ 75, 38, 39},
{ 85, 37, 38},
{ 96, 36, 37},
{107, 32, 34},
{117, 28, 34},
{128, 24, 33},
{139, 17, 29},
{149, 15, 29},
{160,  5, 20}},

{{ 0,  0,  0},
{  0,  0, 16},
{  0,  0, 32},
{  0,  0, 48},
{  0,  0, 64},
{  0,  0, 80},
{  0,  0, 96},
{  0,  0,112},
{  0,  0,128},
{  0,  0,144},
{  0,  0,160},
{  0,  0,176},
{  0,  0,192},
{  0,  0,208},
{  0,  0,224},
{  0,  0,240}},

{{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20},
{160,  5, 20}},

{{ 0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240},
{  0,  0,240}}
};


#define X .525731112119133606
#define Z .850650808352039932

 float icosahedron_v[12][3] = {
{-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
{0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
{Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};
 GLubyte icosahedron_i[20][3] = {
{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

const GLint zielkreuz[][3]={{-10,  0, 1}, {- 2,  0, 1},
                            { 10,  0, 1}, {  2,  0, 1},
                            {  0,-10, 1}, {  0, -2, 1},
                            {  0, 10, 1}, {  0,  2, 1}};
                            

const GLfloat light_ambient_l[] = {0.93, 1.0, 1.0, 1.0};
const GLfloat light_diffuse_l[] = {0.1 , 0.0, 0.0, 1.0};
const GLfloat light_ambient_r[] = {1.0 , 1.0, 0.93, 1.0};
const GLfloat light_diffuse_r[] = {0.0 , 0.0, 0.1, 1.0};

VECTOR null={0,0,0};

CAMERA camera;

struct RL
{
    struct u
    {
       short i[MAXUNITS];
       short anz;
    }u;
    struct shot
    {
       short i[MAXSHOTS];
       short anz;
    }shot;
    struct explosion
    {
       short i[MAXEXPLOSIONS];
       short anz;
    }explosion;
} RL;


struct reticule
{
    bool active;
    VECTOR pos;
} reticule;

VECTOR partikel[64];
float        fps_rate = 60.0,
             sinus10[36][2];					//Daten für Zielmonitor
int          frame_count = 0,
             frame_count_time = 0,
             input_delay=0,
             mrauschen=0,
             buttn_delay=0;
bool         buttn_pressed=0;
char         crosscolor[CROSSBOX*CROSSBOX*CROSSBOX];


volatile int chrono = 0;
float        fcount, fmult;

GLubyte      circleindex[36];

GLuint  tex;
GLuint  r_kam_list;
GLuint  l_kam_list;
GLuint  draw_model_list;
GLuint  draw_cross_list;
SAMPLE *sound[12];

MODEL       font3d;
MODEL       ziffern;
MODEL       laser;
MODEL       mod_dumbfire;
MODEL       mod_explosions;

INIT        init;

CollisionModel3D* colmodel[MAXMODELS];

VECTOR stars[MAXSTARS];

	BITMAP *bmp;
	BITMAP *rauschen;
    BITMAP *logo[4];
    

enum JOY_STATUS
{
    JOY_1 = 0,
    JOY_2,
    JOY_3,
    JOY_4,
    JOY_5
};


struct stick
{
    bool present;
    float   axis[3];
    bool    b[4];
    char    state;
    signed char cx, cy, cz;
} stick;

UNIT no_target;
UNIT *NO_TARGET=&no_target;


UNIT units[MAXUNITS];
UNIT *unit[MAXUNITS];
SHOT shots[MAXSHOTS];
SHOT *shot[MAXSHOTS];
MODEL       models[MAXMODELS];
MODEL       *model[MAXMODELS];
EXPLOSION explosions[MAXEXPLOSIONS];
EXPLOSION *explosion[MAXEXPLOSIONS];
JET jets[MAXJETS];
JET *jet[MAXJETS];

JET no_jet;
JET *NO_JET=&no_jet;

short explosion_anz=0;
short shot_anz=0;
short unit_anz=0;
short colmodel_anz=0;
short jet_anz=0;

int testval=0;

DUH *musicpiece;
AL_DUH_PLAYER *dp;

MATRIX_f debris_rot[8];
PLAYER player;

