#ifndef GLOBAL
#define GLOBAL

/******************K O N S T A N T E N***********************/

//Arrays
#define MAXSHOTS        128
#define MAXEXPLOSIONS     64
#define MAX_HEMI     16
#define MAXMISSILE        12
#define MAXUNITS 32
#define MAXMODELS 32
#define MAXENGINE 10
#define MAXACTIONS 256
#define MAXDEBRIS 64
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
#define MISSILESHOOTRATE 250
#define HIGHWAYWIDTH 8

//pi
#ifndef M_PI
#define M_PI   3.1415926535897932384626433832795
#endif
#define RAD_2_DEG(x) ((x) * 180 / M_PI)
#define DEG_2_RAD(x) ((x) / (180 / M_PI))
#define RAD_2_256(x) ((x) * 128 / M_PI)

//Debris
#define DBOX_X	60
#define DBOX_Y	60
#define DBOX_Z	200
#define DBOX_ZZ	0.39
#define DNUM    256


//Collision-Detection - Werte in metern
#define SHOTLENGTH    1.5
#define MISSILELENGTH    2




//Render-Selektion
#define XHEADING 0.333
#define YHEADING 0.183


//Sonstiges

#define NONE -1

#include "harbst.h"
#include "cd.h"
#include "stick.h"

/********************T Y P E D E F   E N U M*************************/

typedef enum BUTTON_TYPES
{
    BTTN_DEFAULT = 0,
    BTTN_FAST,
    BTTN_WEAPON
};

typedef enum UNIT_TYPE
{
    UNIT_FIGHTER = 0,
    UNIT_MISSILE,
    UNIT_OBJECT,
    UNIT_LEVEL,
    UNIT_ITEM
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

typedef enum CAMERA_VIEW
{
    CAMERA_COCKPIT = 0,
    CAMERA_FOLLOW_PLAYER_SHIP,
    CAMERA_FREE
}CAMERA_VIEW;

/********************T Y P E D E F   S T R U C T*************************/

typedef struct UDATA              //Struct mit Einheiten-Spezifikationen
{
    char *label;
    char type;                    //Typ der Einheit
    float maxspeed;               //Höchstgeschwindigkeit
    float acceleration;           //Beschleunigung
    float rotacceleration;        //Rotationsbeschleunigung
    float hullpower;              //aus wieviel Energie besteht der Rumpf?
    float shieldpower;            //wieviel Energie kann der Schild speichern?
    float mass;                   //wie schwer ist die Einheit?
    float maxrotationspeed;       //wie schnell kann sich die Einheit drehen?
    int   shootrate;              //wie schnell kann geschossen werden
    signed char shottype;         //welche Schüsse werden abgegeben?
    float gunpower;               //weviel Energie können die Kanonen-Akkus speichern?
    float gun_erergy_drain;       //wieviel Energie pro Schuß?
    float gun_x_offset;           //wie weit stehen die Kanonen auseinander? /2
    float gun_max_angel;          //wie weit können die Kanonen ihren Winkel verändern?
    char *mount_obj[4];           //Objektnummern möglicher Raketenlafetten
    float max_y_speed;            //maximale vertikale Geschwindigkeit
    float max_x_speed;            //maximale Seitwärtsgeschwindigkeit
    char engine_obj;              //welches Objekt ist das Triebwerk?
    float afterburner_speed;      //Höchstgeschwindigkeit bei laufendem Nachbrenner
    char *col_model;              //Name des Kollisions-3do
    char *model;                  //Name des 3do
    float score;                  //wie viele Punkte gibt es, wenn man diese Einheit abschießt?
}UDATA;


typedef struct UNIT
{
    bool vulnerable;              //ist die Einheit verwundbar?
    char rendered;                //(wie) wird die Einheit gerendert?
	char gun;                     //welche Kanone feuert als nächste?
	char status;                  //was macht die Einheit?
    char *name;                   //wie heißt die Einheit
    short index;                  //welcher Index im Unit-Array?
 	short party;                  //zu welcher Gruppe gehört die Einheit?
	int t;                        //wie alt ist die Einheit?
	int shotdelay;                //Abstand zwischen den Schüssen
	int justhit;                  //wann wurde die Einheit zuletzt getroffen
	float hull;                   //Stärke des Rumpfes in Prozent
	float shield;                 //Stärke des Schildes in Prozent
	float speed;                  //die normalisierte Geschwindigkeit
	float gunpower[2];            //Energie der Laser-Akkus in Prozent
	float dist;                   //Distanz der Einheit zur Kamera
	float presence;               //wie präsent ist die Einheit auf dem Schirm?
	float gunangel;               //Drehung der Kanonen
    char mount[4];                //Raketenlafetten
	VECTOR dir_delta;
	VECTOR flightdir;             //globaler Flugrichtungs-Vektor, seine Länge ist die absolute Geschwindigkeit
	VECTOR speed_soll;            //speed_soll.z ist die Sollgeschwindigkeit
	VECTOR wink_soll;             //Winkelgeschwindigkeit, Grad pro Sekunde, Soll
	VECTOR wink_ist;              //Winkelgeschwindigkeit, Grad pro Sekunde, Ist
	VECTOR heading;               //Wo befindet sich die Einheit auf der Scannersphäre der Spielereinheit?
	VECTOR start_pos;             //wo entstand die Unit? Wichtig für Raketen
	MATRIX_f m;                   //Translation und Rotation der Einheit
	UNIT *target;                 //Aufgeschaltetes Ziel
	UNIT *property;               //zu welcher Einheit gehöre ich?
	UDATA *udata;                 //Zeiger auf Datasheet
	char missile;                 //welcher Raketentyp ist ausgewählt?
	char outlinecolor;            //wie hell werden die Outlines gezeichnet? Abhängig von presence
	char dumbmissiles;            //wiviel Dumbfires sind noch im Köcher
	char seekmissiles;            //wieviel Seekers sind noch da?
	bool afterburner;             //an oder aus?
	MODEL3D_CLASS model3d;
    COLMODEL_CLASS *colmodel;
} UNIT;

typedef struct WDATA              //Struct mit Waffen-Spezifikationen
{
    MODEL model;
    float damage;
    float damage_modifier;
    float area_of_effect;
    char *name;
    int soundstart;
    int soundhit;
    float velocity;
    float start_velocity;
    int duration;
    int reload_time;
    float energy_drain;
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
	VECTOR heading;
    WDATA *wdata;
}SHOT;


typedef struct HIGHWAY
{
    VECTOR segment[32];
    VECTOR left[32], up[32], fro[32];
    float curviness[32];
    float scale[32];
    short segment_anz;
    float width;
} HIGHWAY;

typedef struct PLAYER
{
    UNIT *unit;
    MATRIX_f vorhalt;                      //wo befindet sich der Punkt, wo der Schuß auftreffen würde?
    bool gunhits;                          //wird die Kanone treffen?
    bool target_is_visible;                //ist das Ziel im Sichtfeld des Spielers?
    float distance_to_target;              //wie weit ist das Ziel weg?
    float time_to_impact;                  //Flugzeit bis zum möglichen Auftreffen eines vom Spieler abgegebenen Schusses
    int target_monitor_interference;       //ist der Zielmonitor gestört?
    VECTOR hitpoint;                       //wo befindet sich der Punkt, wo der Schuß auftreffen würde?
    VECTOR norm_flightdir;                 //normalisierte Flugrichtung
    bool wait_for_release;                 //nächstes gültiges Tastaturkommando erst nach dem Loslassen der Tastatur
    bool keyboard_was_pressed;             //wurde die Tastatur im letzten Frame gedrückt?
    MATRIX_f zero_m;                       //Kamera-Matrix ohne Translation
    STICK stick;                           //Joystick-Daten
    short        aufl_w;    //Bildschirmauflösung in der Breite
    short        aufl_h;    //Bildschirmauflösung in der Höhe
    short        farbtiefe; //Farbqualität (8, 16, 24 oder 32)
    short        z_puffer;  //Auflösung des Tiefenpuffers (8, 16, 24 oder 32)
    bool         bg_is_drawn;               //wird der Hintergrund gemalt?
    bool         debris_is_drawn;           //wird der Weltraumschotter gemalt?
    bool gear_extended;
    struct camera
    {
       char camera_object;                  //Welches Unterobjekt des Spielermodells definiert die Position der Kamera
       char perspective;                    //Ist die Kamera im Cockpit oder hinter dem Schiff?
       VECTOR pos;                          //Positionsvektor der Kamera
       MATRIX_f m;
       MATRIX_f m_old;                      //Position der Kamera im letzten Frame
       MATRIX_f m_delta;                    //Unterschied zwischen alter und neuer Matrix
       MATRIX_f offset;                     //Position der Kamera auf dem Schiff
       MATRIX_f mall;                       //globale Position der Kamera
    }camera;
}PLAYER;




#endif
