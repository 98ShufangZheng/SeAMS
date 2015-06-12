/* les differentes procedures appelees par scanline en 4 et 8-connexite */

#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
extern "C" {
#include "tiff_color.h"
}

#include "types_communs.hh"

/*****
 * Implementation of loading functions
 *****/

int readBmRGB(const char *filename, regionC** &bm, int &Wi, int &Le);

/*****
 * Implementation of misc. functions
 *****/

char* itoa(int i);
int isTop(regionC* p); 
regionC* FindCompress(regionC* p); 
void Union (regionC* x, regionC* y); 
int checkpixel(regionC* r1, regionC* r2, int color);

int numerote(regionC** &bm, int Wi, int Le);
int renumerote(regionC** &bm, int Wi, int Le);

/*****
 * Implementation of noise generating functions
 *****/

void randomnoise(regionC** &bm, int Wi, int Le, int noise);
void saltandpepper(regionC** &bm, int Wi, int Le, int noise);
float ranf(void);
float box_muller(float m, float s);
void normalnoise(regionC** &bm, int Wi, int Le, float m, float s);
void gen_occlusion(regionC** &bm, int Wi, int Le, int nocc);

/*****
 * Implementation of ${\cal P}(R,R')$
 *****/

int mergingOracle(int Wi, int Le, regionC* x, regionC* y);

/*****
 * Implementation of function $f(R, R')$ prior to sorting
 *****/

int ecart(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2);
int ecartSobelGeneralized(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2);
int ecartSquareBruit(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2, int delta);
int ecartNonBruit(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2);

/*****
 * Implementation of \texttt{sort$\_$increasing}
 *****/

void quicksort(int** refTab, int l, int r);
int** sorting(region** &bm, int Wi, int Le);

/*****
 * SRM
 *****/

void SRM(region** &bm, int Wi, int Le);
void eliminer_petites_regions(regionC** &bm, int Wi, int Le);

/*****
 * Implementation of functions for sorting/merging for occlusion merging
 *****/

void OcclusionSortSRM(region** &bm, int Wi, int Le);
void OcclusionSRM(region** &bm, int** refCour, int* tabI, int* tabJ, int numTot, int Wi, int Le);

/*****
 * Implementation of saving functions
 *****/

void contour(unsigned int* data, int i, int j, int Wi, int Le, int curblack, int curwhite);
void sauveRGBimageNoisy(const char *filename, regionC** &bm, int Wi, int Le);
void sauveRGBimage(const char *filename, regionC** &bm, int Wi, int Le);
void sauveRegion(int numero, int numeroR, regionC** &bm, int Wi, int Le);
void sauveParts(region** &bm, int Wi, int Le);
