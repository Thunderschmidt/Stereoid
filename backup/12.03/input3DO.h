#ifndef INPUT3DO
#define INPUT3DO

int lese_3do(MODEL *model, const char datei[]); 
void del_polys(MODEL *mod);
short init_collision_model(MODEL *mod, O3D *o3d);

#endif
