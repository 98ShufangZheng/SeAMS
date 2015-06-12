#include <stdio.h>

#if OpenCV_OLD
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/core_c.h>
#else
#include <opencv/highgui.h>
#include <opencv/cv.h>
#endif

#include "srm.h"

int main(int argc, char* argv[]) {
	unsigned char key;

   IplImage* image;
   IplImage* segmented;

   image = cvLoadImage(argv[1], 1);
   segmented = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);

   struct srm *srm = srm_new(512.0, image->width, image->height, image->nChannels, 0);

   srm_run(srm, image->widthStep, (uint8_t*)image->imageData, segmented->widthStep, (uint8_t*)segmented->imageData);

   unsigned int *regions = srm_regions(srm);
   unsigned int count = srm_regions_count(srm);
   printf("%i\n", count);

   unsigned int i;
   for(i=0; i < image->width * image->height; i++) {
     if (regions[i] == i) {
       unsigned int x = i / image->width;
       unsigned int y = i % image->width;
       unsigned int offset = x * image->widthStep + image->nChannels * y;

       uint8_t r, g, b;
       r = ((uint8_t*)segmented->imageData)[offset    ];
       g = ((uint8_t*)segmented->imageData)[offset + 1];
       b = ((uint8_t*)segmented->imageData)[offset + 2];

       printf("%i %i %i %i %i %i\n",
              i,
              x,
              y,
              r,
              g,
              b
              );
     }
   }

   srm_delete(srm);

	cvNamedWindow("Statistical Region Merging", CV_WINDOW_AUTOSIZE);
   cvShowImage("Statistical Region Merging", segmented);

	while (key != 'q') {
     key = cvWaitKey(10);
	}

	cvDestroyWindow("Statistical Region Merging");

	return 0;

}
