//Generation de bruit
#include "types_communs.hh"


float ranf(void);
float box_muller(float m, float s);
int uniform(int vmax);

void normalnoise(regionC** &bm, int Wi, int Le, float m, float s);
void saltandpepper(regionC** &bm, int Wi, int Le, int noise);
void randomnoise(regionC** &bm, int Wi, int Le, int noise) ;

void bigcircle(regionC** &bm, int Wi, int Le, int Qiou);
void troisRectangles(regionC** &bm, int Wi, int Le, int Qiou);
void transformImage(regionC** &bm, int Wi, int Le, int Qiou, int numConcept);
void gen_occlusion(regionC** &bm, int Wi, int Le);
