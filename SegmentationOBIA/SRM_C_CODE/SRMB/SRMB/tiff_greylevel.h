#ifndef TIFF_GREYLEVEL
#define TIFF_GREYLEVEL

#include <tiff.h>
#include <tiffio.h>

/*
 *  int readtiffparam(const char *filename, int *width, int *height)
 *
 *  read a tiff file in order to get the parameters of the image
 *  return 0 if an error occurs or not a greylevel image
 */
int readRGBtiffparam(const char *filename, int *width, int *height);

/*
 *  int readtiffallparam(const char *filename, char **soft, char **desc, 
 *                       short *photometric, int *width, int *height)
 *
 *  read a tiff file in order to get the parameters of the image
 *  return 0 if an error occurs or not a greylevel image
 *  store in photometric the PHOTOMETRIC (PHOTOMETRIC_MINISWHITE or
 *  PHOTOMETRIC_MINISBLACK) of the image 
 */
int readRGBtiffallparam(const char *filename, char **soft, char **desc, 
			short *photometric, int *width, int *height);
/*
 *  readtiffgl(const char *filename, unsigned char **buffer)
 *
 *  read a tiff greylevel image recorded in the file filename.
 *  assume that enough space is allocated for *buffer.
 *  return 0 if an error occurs, 
 */
int readRGBtiff(const char *filename, unsigned int *buffer);

/*
 *  int sauvetiffgl(const char *filename, const char *soft, const char *desc,
 *                  const char *data, short photometric,int width, int height){ 
 *
 *  save a tiff image.
 *  data contains the image, filename is the name aof the file, ...
 *  return 0 if an error occurs
 */
int saveRGBtiff(const char *filename, const char *soft, const char *desc, 
		unsigned int *data, short photometric, int width, int height);

#endif
