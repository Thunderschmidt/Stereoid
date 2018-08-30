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
void update_color_fx(int chrono, float fmult);




