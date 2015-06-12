/* les differentes procedures appelees par scanline en 4 et 8-connexite */

#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
extern "C" {
#include "tiff_greylevel.h"
}

#include "types_communs.hh"

int readBm(const char *filenameR, const char *filenameG, const char *filenameB, regionC** &bm, regionC** &bmS, int &Wi, int &Le);
void readModeles(const char *prefixfile,int* &nummod, int Wi, int Le);
void claqueModeles(regionC** &bm,int* &nummod, int Wi, int Le);
int readBmRGB(const char *filename, regionC** &bm, int &Wi, int &Le);

void char_image(regionC** &bm, int Wi, int Le);
void char_data_image(unsigned int* data, int Wi, int Le);
void char_models(int* &nummod, regionC** &bm, int Wi, int Le);
void char_models_part(regionC** &bm, int num, int Wi, int Le);

int isTop(regionC* p); 
regionC* FindCompress(regionC* p); 
metaRegion* MetaFindCompress(regionC* p);
void Flatten(int i, int Wi, regionC** &bm); 
void Union (regionC* x, regionC* y); 
void FlatUnion (regionC* x, regionC* y);
int nouvelle_region(regionC* x, cell* &ptfree, int i, int &cpfree, int
                    Wi);

void puthead(cell* x, cell* &l, cell* &ptsuppr);
cell* first(cell* &p);
int numerote(regionC** &bm, int Wi, int Le);
int renumerote(regionC** &bm, int Wi, int Le);
int numeroteModels(regionC** &bm, int Wi, int Le);
int renumeroteModels(regionC** &bm, int Wi, int Le);

int metacheckpixel(metaRegion* r1, metaRegion* r2, int color);
int checkpixel(regionC* r1, regionC* r2, int color);

void sauveRGBimageModels(int* &nummod, const char *filename, regionC** &bm, int Wi, int Le);
void sauveRGBimage(const char *filename, regionC** &bm, int Wi, int Le);
void contour(unsigned int* data, int i, int j, int Wi, int Le, int curblack, int curwhite);

void point(unsigned int* data, int x, int y, int T, int Wi, int Le, int num);
void putModels(regionC** &bm, unsigned int* data, int Wi, int Le);
void cercle(int num, unsigned int* data, int x, int y, int R, int T, int Wi, int Le);
void ligne(int num, unsigned int* data, int y0, int x0, int y1, int x1, int T, int Wi, int Le);
void carre(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le);
void croix(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le);
void plus(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le);
void trianglesup(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le);
void triangleinf(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le);
void losange(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le);

void sauveRegion(int numero, int numeroR, regionC** &bm, int Wi, int Le);
void sauveModel(int numero, int numeroR, regionC** &bm, int Wi, int Le);
void sauveParts(region** &bm, int Wi, int Le);
void sauvePartsModels(region** &bm, int Wi, int Le);

void eliminer_petites_regions(regionC** &bm, int Wi, int Le);

/*********************************************************************
 * delires hypocarbotchernobylesques de Richard
 *********************************************************************/

int OracleProbabiliste(int Wi, int Le, regionC* x, regionC* y);
double maxdiff(regionC* x, regionC* y);


void quicksortcroisslinkage(int** refTab, int l, int r);
int** sortedlinkage(region** &bm, int Wi, int Le);
void PSIS(int* &nummod,region** &bm, int Wi, int Le);

metaRegion* topModel(regionC* x);
int AuthorizedModels(regionC* x, regionC* y);
int ForcedMerging(regionC* x, regionC* y);
int noModel(regionC* x);

/*****
 * procedures permettant la prise en compte d'occlusions
 *****/

//sans réorganiser les couples
void OcclusionMerge(region** &bm, int Wi, int Le);
void finalMerge(region** &bm, int** refCour, int numTot, int Wi, int Le);

//en réordonnant les couples au préliminaire
void OcclusionMergeSort(region** &bm, int Wi, int Le);
void finalMergeSort(region** &bm, int** refCour, int* tabI, int* tabJ, int numTot, int Wi, int Le);

/*****
 * procedures permettant la prise en compte d'images bruitées
 *****/

int ecartSquare(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2, int delta);
int ecartOriginal(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2);
int sobel(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2);
int ecart(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2);
void sauveRGBimageNoisy(const char *filename, regionC** &bm, int Wi, int Le);

