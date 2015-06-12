/* Fonctions de chargement et de sauvegarde d'une image en niveau de gris */

#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>

unsigned int ori;

/*
 *  int readRGBtiffallparam(const char *filename, char **soft, char **desc, 
 *                       short *photometric, int *width, int *height)
 *
 *  read a tiff file in order to get the parameters of the image
 *  return 0 if an error occurs or not a greylevel image
 *  store in photometric the PHOTOMETRIC (PHOTOMETRIC_MINISWHITE or
 *  PHOTOMETRIC_MINISBLACK) of the image
 */
int readRGBtiffallparam(const char *filename, char **soft, char **desc, 
		     short *photometric, int *width, int *height){
  TIFF *tif;
  short BitsPerSample;	   /* Number of bits per component ( usually 8 ).         */
  short SamplesPerPixel;    /* Number of components per pixel (ex : RGB = 3 ).     */
  short PlanarConfig;       /* How the components of each pixel are stored.        */
  int opentif_ok = 0;
  
  if((filename==NULL) || !strcmp(filename,"")) return 0; /* no file name given */
  
  tif = TIFFOpen(filename, "r");


  if( tif != NULL ){
    opentif_ok = 1;  /* TIFFOpen is ok */
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);  
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplesPerPixel);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &PlanarConfig);
    TIFFGetField(tif, TIFFTAG_ORIENTATION, &ori);
    if (!TIFFGetField(tif, TIFFTAG_IMAGEDESCRIPTION, desc)) *desc = NULL;
    if (!TIFFGetField(tif, TIFFTAG_SOFTWARE, soft)) *soft = NULL;
    TIFFClose(tif);
  }
  else {
    printf("Error opening tiff file : %s\n",filename);
  }
  
  if( (*photometric!=PHOTOMETRIC_PALETTE) ){
    opentif_ok = 0; /* not a RGB */
   }

  return opentif_ok;
}

/*--------------------------------------------------------------------------------- */


/*
 * readRGBtiff(const char *filename, unsigned int *buffer)
 * lit une image TIFF couleur et stocke les niveaux de RBG
 * dans buffer
 */
int readRGBtiff(const char *filename, unsigned int *buffer){
  TIFF *tif;
  int width,height;
  short photometric;
  short BitsPerSample;	   /* Number of bits per component ( usually 8 ).         */
  short SamplesPerPixel;    /* Number of components per pixel (ex : RGB = 3 ).     */
  short PlanarConfig;       /* How the components of each pixel are stored.        */
  unsigned int* current;
  
  current=buffer;

  if((filename==NULL) || !strcmp(filename,"")) return 0; /* no file name given */
  
  tif = TIFFOpen(filename, "r");

  if( tif != NULL ){
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &BitsPerSample);  
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &SamplesPerPixel);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &PlanarConfig);

    if(photometric!=PHOTOMETRIC_PALETTE){
      printf("Image in file %s not RGB\n",filename);
      return 0;
    }
    TIFFReadRGBAImage(tif, width, height, (unsigned long*) current, 0);

    TIFFClose(tif);
    /* the reading of the image is ok */
    return 1;
  }
  else  /* bad reading */
    return 0;
}

/*--------------------------------------------------------------------------------- */

int readRGBtiffparam(const char *filename, int *width, int *height){
  char *strdummy;
  short dummy;
  return readRGBtiffallparam(filename, &strdummy, &strdummy, &dummy, width, height);
 
}
         
/*--------------------------------------------------------------------------------- */

int saveRGBtiff(const char *filename, const char *soft, const char *desc, 
		   unsigned int *data, short photometric, int width, int height){

  TIFF *tif;
  int row;
  unsigned int *dummy;

  if((filename==NULL) || !strcmp(filename,"")) return 0;
  

  if(data==NULL){  /* if no output image given */
    fprintf(stderr,"sauvegarde::savetiff no output image given!\n");
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
  if(desc==NULL){             /* if no description done                  */
    if(desc==NULL) desc = ""; /* a problem with NULL value ?!            */
    else                      /* we get the original image's description */
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


/*--------------------------------------------------------------------------------- */
