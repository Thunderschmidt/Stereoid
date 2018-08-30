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

//CAMERA camera;

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



GLuint  tex;
//GLuint  r_kam_list;
//GLuint  l_kam_list;
//GLuint  draw_model_list;
//GLuint  draw_cross_list;
MODEL       mod_dumbfire;
MODEL       mod_explosions;
MODEL       mod_seeker_missile;

//CollisionModel3D* colmodel[MAXMODELS];

VECTOR stars[MAXSTARS];

BITMAP *bmp;
BITMAP *rauschen;
BITMAP *logo[4];

UNIT no_target;
UNIT *NO_TARGET=&no_target;
UNIT units[MAXUNITS];
UNIT *unit[MAXUNITS];

short unit_anz=0;
short colmodel_anz=0;

int testval=0;

PLAYER player;


UDATA raptor;
UDATA seeker_missile;
UDATA dumb_missile;
UDATA asteroid00;
UDATA asteroid01;
UDATA asteroid02;
UDATA asteroid03;
UDATA asteroid04;
UDATA asteroid05;
UDATA speicher;
UDATA level;
UDATA flea;
STICK stick;

//neutral_fp fp_move_camera[2];
//neutral_fp fp_set_view[2];

	DEBRIS_CLASS debris;
	EXPLOSION1_CLASS explosion1;
	EXPLOSION2_CLASS explosion2;
	BACKGROUND_CLASS background;
	FLUSH_CLASS flush;
	SHOT_CLASS shot;
	SOUND_CLASS sound("sounds/", &player.zero_m, &player.camera.pos, 512.0);
	STEREO_VIEW_CLASS stereo_view;

