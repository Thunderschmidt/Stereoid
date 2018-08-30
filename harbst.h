#ifndef HARBST
#define HARBST



////////////////////////////////////////////////////////////////////////////////
////////////////////////////        TIMER         //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void proceed_timer();
float get_fps();
float get_fmult();
int get_chrono();
void init_timer();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////        MATH         ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct VECTOR
{
    float x, y, z;
} VECTOR;

class MATR_STACK_CLASS
{
    private:
    int total;
    MATRIX_f actual;
    MATRIX_f *stack[128];
    public:
    void mul(MATRIX_f *m);
    void mul_alg(MATRIX_f *m);
    void push();
    void pop();
    void loadIdentity();
    MATRIX_f get();
    void translate(VECTOR *v);
    MATR_STACK_CLASS();
};

VECTOR trans_matrix_to_vector(MATRIX_f *m);
bool accelerate(float soll, float *ist, float a);
void translate_matrix_v(MATRIX_f *m, VECTOR *pos);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////       MODEL3D       ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////


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
   ACT_BLINKING,
   NOW = 1933
};

typedef enum FACETYPES
{
    TRIANGLE = 3,
    QUAD,
    POLYGON
}FACETYPES;

typedef enum AXIS
{
    X_AXIS = 0,
    Y_AXIS,
    Z_AXIS
};

typedef struct O3D  //Unterobjekt, Teil der Model-Datenstruktur
{
	char name[32];   //Name (aus 3do-File)
	char facetype;   //Typ der Flächen: Dreieck, Rechteck, Polygon
	short vert_anz;  //wieviel Koordinatenpunkte?
	short poly_anz;  //wieviel Flächen?
	short line_anz;  //wieviel Linien (f. Outlines)
	short sibl_o;    //Schwesterobjekt
	short child_o;   //Tochterobjekt
	VECTOR pos;      //Offset
	VECTOR *n;       //Zeiger auf Normalvektoren
	VECTOR *vert;    //Zeiger auf Koordinatenpunkte
	char *ind_anz;   //Zeiger auf Datenfeld mit der Anzahl der Indizes zum Inhalt
	char *color;     //Zeiger auf Farbinformationen
	short *line;     //Zeiger auf Linien
	short *i;        //Zeiger auf Indizes
	char *c;         //
   bool single_colored; //ist das Objekt einfarbig?
} O3D;

typedef struct MODEL
{
   	char obj_anz;               //Anzahl der Objekte (O3D)
   	O3D  *o3d;                  //Zeiger auf Unterobjekte
   	float size;                 //
   	float radius;               //Größter Abstand vom Mittelpunkt
   	VECTOR min;                 //
   	VECTOR max;                 //
   	VECTOR *n;                  //Normalvektoren des Kollisionsmodells
	const char *name;                   //filename des 3dos
} MODEL;

typedef struct OBJECT
{
    MATRIX_f m;                   //Trans. und Rot. des Objektes
    bool moved;                   //ist Objekt nicht auf Standardposition?
    bool visible;                 //ist Objekt sichtbar?
    float winkel[3];              //Drehung des Objektes in Winkelangaben
    bool grid_is_visible;         //Outlines sichtbar?
    bool fill_is_visible;         //Flächen sichtbar?
    char color;                   //Farbe des Objektes (überschreibt Standard)
    bool single_colored;          //Objekt einfarbig?
    char alpha;                   //Objekt (halb)durchsichtig?
} OBJECT;

    void init_actions();
    void proceed_actions();
    int get_a_action_anz();
    int get_i_action_anz();
    
typedef void (*ANIM_FUNCT)(void*); //Funktionspointer

class MODEL3D_CLASS
{
    public:
    void kill_all_actions();
    bool wait_for_move(const char name[], char axis);
    bool wait_for_turn(const char name[], char axis);
    void wait(int t);
    void spin(const char name[], char axis, float speed);
    void turn(const char name[], short axis, float amount, float speed);
    void move(const char name[], short axis, float amount, float speed);
    char get_object_by_name(const char name[]);
    void hide(const char name[]);
    void show(const char name[]);
    void hide(int o);
    int prepare_object_matrices();
    MATRIX_f get_object_matrix_world(int o, MATRIX_f *m);
    MATRIX_f get_object_matrix(int o);
    void start_function(ANIM_FUNCT anim_funct);
    void hide_grid(const char name[]);
    void hide_grid(int o);
    void show_grid(const char name[]);
    void hide_fill(const char name[]);
    void show_fill(const char name[]);
    void start_blinking(const char name[]);
    void stop_blinking(const char name[]);
    void assign_3do(const char name[]);
    void clear_a_action(short o, char axis, signed char type);
    void reset_object_matrix(short o);
   	OBJECT *object;               //Zeiger auf Baumstruktur mit den Eigenschaften der Unterobjekte zum Inhalt
   	void MODEL3D_CLASS::draw_objects(short o);
    void MODEL3D_CLASS::draw_objects_lines(short o);
    ~MODEL3D_CLASS();
    MODEL *model;
    void draw_objects_alpha(short o, char alpha);
    private:
   	bool get_object_matrix_recursive(int o, int o_soll);
};


MODEL* load_3do(const char name[]);

   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////       STEREO       ////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class STEREO_CLASS
{
    private:
    float distance_to_monitor;
    float half_eye_seperation;
    float monitor_width;
    float half_monitor_width;
    float monitor_height;
    float half_monitor_height;
    float far_clip;
    float clipscale;
    float near_clip;
    public:
    float eye_seperation;
    void set_to(bool eye);
    void position_camera(MATRIX_f *camera_matrix);
    void set_eye_seperation(float value);
    float get_eye_seperation();
    void set_monitor_width(float value);
    float get_monitor_width();
    void set_monitor_height(float value);
    float get_monitor_height();
    void set_distance_to_monitor(float value);
    float get_distance_to_monitor();
    void set_far_clip(float value);
    float get_far_clip();
    void set_near_clip(float value);
    float scale;
    void init_lighting();
    STEREO_CLASS();
};

typedef enum STEREO_VIEW
{
    LEFT_EYE=0,
    RIGHT_EYE,
    MONITOR_PLANE
};

void init_color_table(char file[]);
void color16(char lumi);
void color16(char lumi, char alpha);
bool get_eye();
void update_color_fx();


#endif


