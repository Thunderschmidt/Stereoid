#ifndef GLOBAL
#define GLOBAL

/******************K O N S T A N T E N***********************/

//Joystick
#define TOTZONE                10
//Arrays
#define MAXSHOTS        128
#define MAXEXPLOSIONS     64
#define MAX_HEMI     16
#define MAXMISSILE        12
#define MAXUNITS 32
#define MAXMODELS 32
#define MAXENGINE 10
#define MAXSTARS 2000
#define MAXACTIONS 256
#define MAXDEBRIS 64
#define MAXJETS  128
#define MAXFLUSHS 128

//Werte
#define SHOTSPEED         300
#define SHOTDAMAGE        15
#define SHOTTIME          2000

#define MISSILETIMER 5500
#define MISSILESTARTSPEED 0
#define DUMBMISSILESTARTSPEED 55
#define MISSILETURN 15
#define MISSILEMAXSPEED 200
#define MISSILEACC 50
#define MISSILEDAMAGE 40
#define SHOOTRATE 400
#define MISSILESHOOTRATE 250
#define HIGHWAYWIDTH 8

#define ROTDEC 0.015 //Wie stark soll das Raumschiff bei einer Drehung abbremsen?
#define MAXSPEED 90
#define ROTACC              200
#define TRANSACC          30
#define MAXROT 90

#define AUFL 1024
//#define NOW 7122.0

//pi
#ifndef M_PI
#define M_PI   3.1415926535897932384626433832795
#endif
#define RAD_2_DEG(x) ((x) * 180 / M_PI)
#define DEG_2_RAD(x) ((x) / (180 / M_PI))
#define RAD_2_256(x) ((x) * 128 / M_PI)

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


//Collision-Detection - Werte in metern
#define CHECKDISTANCE 1
#define SHOTLENGTH    1.5
#define MISSILELENGTH    2
#define OBJECT_COLLISION_ACCURACY 0.5



//Render-Selektion
#define XHEADING 0.333
#define YHEADING 0.183


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
    UNIT_LEVEL
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
    AI_EVADING,
    AI_JUSTFLYING
};

typedef enum RENDER_MODE
{
    RND_NOT_RENDERED = 0,
    RND_NORMAL,
    RND_NO_OUTLINES,
    RND_ONLY_OUTLINES,
    RND_BIG_POINT,
    RND_SMALL_POINT
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
   bool single_colored;
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
   	CollisionModel3D* colmodel;
   	VECTOR *n;
} MODEL;

typedef struct OBJECT
{
    MATRIX_f m;
    bool moved;
    bool visible;
    float winkel[3];
    bool grid_is_visible;
    bool fill_is_visible;
    char color;
    bool single_colored;
    char alpha;
} OBJECT;

typedef struct JET
{
   VECTOR v[4];
   int t;
   JET *nextjet;
   JET *lastjet;
} JET;

typedef struct UDATA
{
    char *label;
    char type;
    float maxspeed;
    float acceleration;
    float rotacceleration;
    float hullpower;
    float shieldpower;
    float mass;
    float maxrotationspeed;
    int   shootrate;
    signed char shottype;
    float gunpower;
    float gun_erergy_drain;
    float gun_x_offset;
    float gun_max_angel;
    char *mount_obj[4];
    char *collision_model;
    float max_y_speed;
    float max_x_speed;
    char engine_obj;
    float afterburner_speed;
    MODEL model;
    MODEL col_model;    
}UDATA;


typedef struct UNIT
{
   	bool vulnerable;
   	bool big;
    char rendered;
	char gun;
	char status;
    char *name;
    short index;
	short party;
	int t;
	int shotdelay;
	int justhit;
	int enginetimer;
	float hull;
	float shield;
	float speed;
	float gunpower[2];
	float dist;                   //Distanz zur Kamera
	float presence;               //Wie präsent ist die Einheit auf dem Schirm?
	float gunangel;               //Drehung der Kanonen
    char mount[4];
	VECTOR dir_delta;
	VECTOR flightdir;
	VECTOR speed_soll;
	VECTOR wink_soll;
	VECTOR wink_ist;
	VECTOR heading;
	VECTOR start_pos;
	MATRIX_f m;
	OBJECT *object;
	UNIT *target;
	EL engine[MAXENGINE];
	JET *jet;
	UNIT *property;               //zu welcher Einheit gehöre ich?
	UDATA *udata;                 //Zeiger auf Datasheet
	char missile;                 //welcher Raketentyp ist ausgewählt?
	char outlinecolor;            //wie hell werden die Outlines gezeichnet? Abhängig von presence
	char dumbmissiles;
	char seekmissiles;
	bool afterburner;
} UNIT;

typedef enum SHOT_TYPE
{
    SHOT_LASER=15,
    SHOT_PLASMA=25
}SHOT_TYPE;

typedef enum FLUSH_TYPE
{
    FLUSH_CIRCLE=0,
    FLUSH_STRETCHED=1
}FLUSH_TYPE;

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
   ACT_HIDE,
   ACT_SHOW,
   ACT_ALL,
   ACT_FUNCTION,
   ACT_HIDE_GRID,
   ACT_SHOW_GRID,
   ACT_HIDE_FILL,
   ACT_SHOW_FILL,

   NOW = 1933
};


typedef struct SHOT
{
    bool active;
    bool hitslevel;
	MATRIX_f m;
	short index;
	char rendered;
	int t;
	float speed;
	UNIT *property;
	char type;
	VECTOR heading;
}SHOT;

typedef enum EXPLOSION_TYPE
{
    EXPLOSION_NORMAL = 0,
    EXPLOSION_HEMI
}EXPLOSION_TYPE;

typedef struct EXPLOSION2
{
    char type;
    UNIT *property;
    short total;
    int t;
    float *particles;
    float mult;
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

typedef enum FACETYPES
{
    TRIANGLE = 3,
    QUAD,
    POLYGON
}FACETYPES;

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

typedef struct HIGHWAY
{
    VECTOR segment[32];
    VECTOR left[32], up[32], fro[32];
    float curviness[32];
    float scale[32];
    short segment_anz;
    float width;
} HIGHWAY;

typedef enum CAMERA_VIEW
{
    CAMERA_COCKPIT = 0,
    CAMERA_BACK,
    CAMERA_FRONT
}CAMERA_VIEW;

typedef struct PLAYER
{
    UNIT *unit;
    MATRIX_f vorhalt;
    bool gunhits;
    bool target_is_visible;
    float distance_to_target;
    float time_to_impact;
    int target_monitor_interference;
    VECTOR hitpoint;
    VECTOR norm_flightdir;
    bool wait_for_release;
    bool keyboard_was_pressed;
    MATRIX_f zero_m;
    STICK stick;
 //   
//    float 	     aa;        //Augenabstand
//	float        mb;        //Monitorbreite
//	float        db;        //Distanz zum Bildschirm
//	float        an;        //Augen-Nasenwurzelabstand
    float	     ppm;		//Pixel pro Meter
	int          aap;		//Augenabstand in Pixeln
	float        aspect;	//Verhältnis X:Y der Kamera-Pyramide
	float        near_clip; //
	float        far_clip;  //Bis zu welcher Entfernung werden Objekte dargestellt?
    short        aufl_w;    //Bildschirmauflösung in der Breite
    short        aufl_h;    //Bildschirmauflösung in der Höhe
    short        farbtiefe; //Farbqualität (8, 16, 24 oder 32)
    short        z_puffer;  //Auflösung des Tiefenpuffers (8, 16, 24 oder 32)
    float        half_viewport_heigth;
    bool         bg_is_drawn;
    bool         debris_is_drawn;
    struct camera
    {
       char camera_object;
       char perspective;
       VECTOR stereo;
       VECTOR pos;
       VECTOR pos_old;
       VECTOR pos_delta;
       MATRIX_f m;
       MATRIX_f m_inv;
       MATRIX_f m_zero_inv;
       MATRIX_f m_old;
       MATRIX_f m_delta;
       MATRIX_f offset;    
    }camera;
}PLAYER;

typedef struct LEVEL_SEGMENT
{
	short  triangle_vert_anz;
	short  line_vert_anz;
	short  triple_anz;
	short  line_anz;
	VECTOR *normals;
	VECTOR *triangle_vertices;
	VECTOR *line_vertices;
	short  *line_indices;
	short  *triangle_indices;
	char   *triangle_color;
}LEVEL_SEGMENT;

typedef struct LEVEL
{
    unsigned char x_segment_anz;
    unsigned char z_segment_anz;
    VECTOR pos;
    LEVEL_SEGMENT **level_segments;
}LEVEL;

//typedef void (*neutral_fp)();

typedef void (*ANIM_FUNCT)(UNIT*);



#endif
