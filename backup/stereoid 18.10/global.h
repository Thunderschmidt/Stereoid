#ifndef GLOBAL
#define GLOBAL

/******************K O N S T A N T E N***********************/

//Joystick
#define TOTZONE                10
//Arrays
#define MAXSHOTS        128
#define MAXEXPLOSIONS     16
#define MAXMISSILE        12
#define MAXUNITS 32
#define MAXMODELS 32
#define MAXENGINE 10
#define MAXSTARS 2000
#define MAXACTIONS 64
#define MAXDEBRIS 64
#define MAXJETS  128

//Werte
#define SHOTSPEED         300
#define SHOTDAMAGE        15
#define SHOTTIME          3500

#define MISSILETIMER 3500
#define MISSILESTARTSPEED 35
#define DUMBMISSILESTARTSPEED 55
#define MISSILETURN 15
#define MISSILEMAXSPEED 200
#define MISSILEACC 50
#define MISSILEDAMAGE 40
#define SHOOTRATE 400
#define MISSILESHOOTRATE 250

#define ROTDEC 0.015 //Wie stark soll das Raumschiff bei einer Drehung abbremsen?
#define MAXSPEED 90
#define ROTACC              200
#define TRANSACC          30
#define MAXROT 90

//pi
#ifndef M_PI
#define M_PI   3.1415926535897932384626433832795
#endif
#define RAD_2_DEG(x) ((x) * 180 / M_PI)
#define DEG_2_RAD(x) ((x) / (180 / M_PI))

//joystick

#define SQR2_128 11.3137085
#define SQR3_128 25.39841683
#define SQR4_128 38.05462768
#define SQR5_128 48.50293013

//Kreuze
#define CROSSBOX    12
#define CROSSGRID   15
#define CROSSSIZE   0.15

//Debris
#define DBOX_X	60
#define DBOX_Y	60
#define DBOX_Z	200
#define DBOX_ZZ	0.39
#define DNUM    256

//Render-Selektion
#define XHEADING 0.48
#define YHEADING 0.32

//Collsion-Detection
#define CHECKDISTANCE 2
#define SHOTLENGTH    1
#define MISSILELENGTH    1

//Sonstiges

#define NONE -1

/********************T Y P E D E F S*************************/

typedef struct VECTOR
{
    float x, y, z;
}VECTOR;

typedef struct EL
{
    VECTOR pos;
    MATRIX_f m;
    short u;
    int t;
    float speed;
    bool active;
}EL;

typedef enum AXIS
{
    X_AXIS = 0,
    Y_AXIS,
    Z_AXIS
};
   
typedef enum BUTTON_TYPES
{
    BTTN_DEFAULT = 0,
    BTTN_FAST
};

typedef enum UNIT_TYPE
{
    UNIT_FIGHTER = 0,
    UNIT_MISSILE,
    UNIT_OBJECT,
    UNIT_BINARY
};

typedef enum UNIT_AI_STATUS
{
    AI_PLAYER = 0,
    AI_ATTACKING,
    AI_FLEEING,
    AI_DUMBFIRE,
    AI_SEEKING,
    AI_NOTHING,
    AI_WAITING,
    AI_EVADING
};

typedef struct O3D
{
	char name[32];
	char facetype;
	short vert_anz;
	short poly_anz;
	short line_anz;
	short sibl_o;
	short child_o;
	VECTOR pos;
	VECTOR *n;
	VECTOR *vert;
	char *ind_anz;
	char *color;
	short *line;
	short *i;
	char *c;
	bool visible;
} O3D;

typedef struct MODEL
{
    short index;
   	char obj_anz;
   	O3D  *o3d;
   	float size;
   	float radius;
   	VECTOR min;
   	VECTOR max;
   	char camera;
   	char lemit;
   	char remit;
   	char engine;
   	char lmounting;
   	char rmounting;
   	char lbarrel;
   	char rbarrel;
   	char lgun;
   	char rgun;
   	short colmodel;
} MODEL;

typedef struct OBJECT
{
    MATRIX_f m;
    bool moved;
    bool visible;
    float winkel[3];
} OBJECT;


typedef struct JET
{
   VECTOR v[4];
   int t;
   JET *nextjet;
   JET *lastjet;
} JET;

typedef struct UNIT
{
    char *name;
    short index;
   	bool vulnerable;
   	bool big;
   	UNIT *property;
	char gun;
	char status;
	MODEL *model;
	short party;
	int type;
	int t;
	int shotdelay;
	int justhit;
	float hull;
	float shield;
	VECTOR move;
	VECTOR speed_soll;
	VECTOR speed_ist;
	VECTOR wink_soll;
	VECTOR wink_ist;
	VECTOR heading;
	VECTOR start_pos;
	OBJECT *object;
	MATRIX_f m;
	UNIT *target;
	int enginetimer;
	EL engine[MAXENGINE];
	JET *jet;
} UNIT;

typedef enum SHOT_TYPE
{
    SHOT_LASER=15,
    SHOT_PLASMA=25
}SHOT_TYPE;


typedef enum ACTIONTYPES
{
	
   ACT_TURN = -127,
   ACT_MOVE = -126,
   ACT_SPIN,
   ACT_WAITFOR,
   ACT_WAIT,
   
   ACT_ACTIVE,
   ACT_INACTIVE,
   ACT_FINISHED,
   ACT_KILL_IT,
   ACT_NOW,
   ACT_ALL
};


typedef struct SHOT
{
	MATRIX_f m;
	int t;
	float speed;
	UNIT *property;
	char type;
	VECTOR heading;
}SHOT;

typedef struct CAMERA
{
    VECTOR stereo;
    VECTOR pos;
    MATRIX_f m;
    MATRIX_f m_old;
    MATRIX_f m_delta;
    MATRIX_f offset;    
}CAMERA;

typedef struct EXPLOSION
{
    char type;
    UNIT *property;
    short anz;
    int t;
    float partikel[64];
    float mult;
    float speed;
    VECTOR pos;
    MATRIX_f m;
}EXPLOSION;

typedef enum FACETYPES
{
    TRIANGLE = 3,
    QUAD,
    POLYGON
}FACETYPES;

typedef struct INIT
{
    float 	     aa;        //Augenabstand
	float        mb;        //Monitorbreite
	float        db;        //Distanz zum Bildschirm
	float        an;        //Augen-Nasenwurzelabstand
	float        zoom;		//Kamera-Winkel
    float	     ppm;		//Pixel pro Meter
	int          aap;		//Augenabstand in Pixeln
	float        aspect;	//Verhältnis X:Y der Kamera-Pyramide
	float        near_clip; //
	float        far_clip;  //Bis zu welcher Entfernung werden Objekte dargestellt?
    short        aufl_w;    //Bildschirmauflösung in der Breite
    short        aufl_h;    //Bildschirmauflösung in der Höhe
    short        farbtiefe; //Farbqualität (8, 16, 24 oder 32)
    short        z_puffer;  //Auflösung des Tiefenpuffers (8, 16, 24 oder 32)
}INIT;

typedef struct PLAYER
{
    UNIT *unit;
    MATRIX_f vorhalt;
    bool gunhits[2];
    float distance_to_target;
    float time_to_impact;
    bool target_is_visible;
    int target_monitor_inteference;
}PLAYER;

/*******************V A R I A B L E N************************/


#endif
