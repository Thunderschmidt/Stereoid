#ifndef DRAWPRIM
#define DRAWPRIM
void init_drawprim();
void draw_marker(float scale);
void draw_circle(float scale);
void draw_circle_outline(float scale);
void draw_arrow();
void draw_scanner_point(VECTOR *heading);
void draw_icosahedron(char c);
void draw_energy_bar(float width, float percent);
void draw_flash();
void draw_string(char string[]);
void translate_target_pointer(VECTOR heading, float x, float y, float aspect);
void draw_unit_lines(UNIT *u);
void draw_objects_lines(UNIT *u, short o);
void draw_object_lines(O3D *o3d);
void draw_unit(UNIT *u);
void draw_objects(UNIT *u, short o);
void draw_object(O3D *o3d);
void draw_object_alpha(O3D *o3d, char alpha);
#endif
