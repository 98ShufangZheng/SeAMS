#include "var_commune.hh"
#include "srmBproc.hh"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <tiff.h>
#include <tiffio.h>
#include <string.h>

static char BUF2[12];
char* itoa2(int i)
{
  char *pos = BUF2 + sizeof(BUF2) - 1;
  unsigned int u;
  int negative = 0;
  if (i < 0){
    negative = 1;
    u = ((unsigned int)(-(1+i))) + 1;
  } else {
    u = i;
  }
  *pos = 0;
  do {
    *--pos = '0' + (u % 10);
    u /= 10;
  } while (u);
  if (negative) {
    *--pos = '-';
  }
  return pos;
}

int saveRGBtiffRect(const char *filename, const char *soft, const char *desc,
                   unsigned int *data, short photometric, int width, int height)
{
  unsigned int ori=1;
  TIFF *tif;
  int row;
  unsigned int *dummy;

  if((filename==NULL) || !strcmp(filename,"")) return 0;

  if(data==NULL){  /* if no output image given */
    fprintf(stderr,"sauvegarde image::pas d'image de sortie / no output image !\n");
    return 0;
  }

  tif = TIFFOpen(filename, "w");
  TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(tif, TIFFTAG_COMPRESSION,COMPRESSION_NONE );
  TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
  TIFFSetField(tif, TIFFTAG_ORIENTATION, ori);
  //TIFFSetField(tif, TIFFTAG_COLORMAP, ColorMap);
  TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
  TIFFSetField(tif, TIFFTAG_ARTIST, "Richard Nock");
  if(desc==NULL){             
    if(desc==NULL) desc = ""; 
    else                      
      TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, desc);
  }
  else
    TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, desc);
  TIFFSetField(tif, TIFFTAG_SOFTWARE, soft);

  dummy = data;
  for (row = 0; row < height; row++) {
    TIFFWriteScanline(tif, dummy, row, 0);
    dummy += 1*width;
  }

  TIFFClose(tif);

  return 1;
}


void sauvegardeRect(regionC** &bm, int Wi, int Le, int lux, int luy, int rdx, int rdy, int nummod, int iter){
  unsigned int *data;
  int i,j,k;
  regionC* c;
  metaRegion* m;
  int tx=(lux>rdx) ? (lux-rdx) : (rdx-lux);
  int ty=(luy>rdy) ? (luy-rdy) : (rdy-luy);
  data = (unsigned int*) (malloc(sizeof(unsigned int)*tx*ty));
  int pixelcolorR, pixelcolorG, pixelcolorB, pixelcolorA;
  
  char filename[255];
  strcpy(filename,"fileRect");
  strcat(filename,itoa2(iter));

  for (i = 0; i < tx; i++){	
    for (j = 0; j < ty; j++){
      c=FindCompress(bm[((Le-lux-i)*Wi) + luy +j]);
      m=c->meta;
      k=c->numMod;

      if (m->nom==nummod){
	pixelcolorR = (int) ((256/m->nMod)*k);
	pixelcolorG = 255 -(int) ((256/m->nMod)*k);
	pixelcolorB = (int) ((256/m->nMod)*k);
	//printf("%d\n",pixelcolorR);
      }else{
	pixelcolorR = 255;
	pixelcolorG = 255;
	pixelcolorB = 255;
      }
      pixelcolorA = 255;

      data[i*ty+j] = pixelcolorR
	+ (pixelcolorG << 8)
	+ (pixelcolorB << 16)
	+ (pixelcolorA << 24);
    }
  }

   //on renverse l'image
   /*int tmp;
   for (int i = 0; i < (tx/2); i++)
     {
       for (int j = 0; j < ty; j++) 
	 {
	   tmp=data[(tx-1-i)*ty+j];
	   data[(tx-1-i)*ty+j]=data[i*ty+j];
	   data[i*ty+j]=tmp;
	 }
	 }*/

   if (saveRGBtiffRect(filename, "", "", data, PHOTOMETRIC_RGB, ty, tx) == 0)
	{	printf ("erreur readtiffgl"); }

   delete [] data;
}

