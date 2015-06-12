#include <stdio.h>

#if OpenCV_OLD
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/core/core_c.h>
#else
#include <opencv/highgui.h>
#include <opencv/cv.h>
#endif

#include "srm.h"

struct stat {
  unsigned int size;
  double mu[5];
  double Sigma[25];
};

int main(int argc, char* argv[]) {
   IplImage* image;
   IplImage* segmented;
   unsigned int i;

   image = cvLoadImage(argv[1], 1);
   segmented = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
   unsigned int n = image->width * image->height;

   struct srm *srm = srm_new(512.0, image->width, image->height, image->nChannels, 0);

   srm_run(srm, image->widthStep, (uint8_t*)image->imageData, segmented->widthStep, (uint8_t*)segmented->imageData);

   unsigned int *regions = srm_regions(srm);
   unsigned int *sizes   = srm_regions_sizes(srm);
   unsigned int count    = srm_regions_count(srm);
   unsigned int *regions_id = malloc(n * sizeof(unsigned int));
   struct stat* stats = malloc(count * sizeof(struct stat));
   printf("Number of regions %i\n", count);

   memset(stats, 0, count * sizeof(struct stat));

   unsigned int id = 0;
   for(i=0; i < n; i++) {
     if (regions[i] == i) {
       regions_id[i] = id;
       stats[id].size = sizes[i];
       id++;
     }
     else {
       regions_id[i] = 0;
     }
   }

   for(i=0; i < n; i++) {
     unsigned int region = regions[i];
     unsigned int reg = regions_id[region];

     double x = i / image->width;
     double y = i % image->width;
     unsigned int offset = x * image->widthStep + image->nChannels * y;
     double r = ((uint8_t*)image->imageData)[offset    ];
     double g = ((uint8_t*)image->imageData)[offset + 1];
     double b = ((uint8_t*)image->imageData)[offset + 2];

     stats[reg].mu[0] += r;
     stats[reg].mu[1] += g;
     stats[reg].mu[2] += b;
     stats[reg].mu[3] += x;
     stats[reg].mu[4] += y;

     stats[reg].Sigma[5 * 0 + 0] += r*r;
     stats[reg].Sigma[5 * 0 + 1] += r*g;
     stats[reg].Sigma[5 * 0 + 2] += r*b;
     stats[reg].Sigma[5 * 0 + 3] += r*x;
     stats[reg].Sigma[5 * 0 + 4] += r*y;
                                
     stats[reg].Sigma[5 * 1 + 1] += g*g;
     stats[reg].Sigma[5 * 1 + 2] += g*b;
     stats[reg].Sigma[5 * 1 + 3] += g*x;
     stats[reg].Sigma[5 * 1 + 4] += g*y;
                                
     stats[reg].Sigma[5 * 2 + 2] += b*b;
     stats[reg].Sigma[5 * 2 + 3] += b*x;
     stats[reg].Sigma[5 * 2 + 4] += b*y;
                                
     stats[reg].Sigma[5 * 3 + 3] += x*x;
     stats[reg].Sigma[5 * 3 + 4] += x*y;
                                
     stats[reg].Sigma[5 * 4 + 4] += y*y;
   }

   for(i=0; i < count; i++) {
     stats[i].mu[0] /= stats[i].size;
     stats[i].mu[1] /= stats[i].size;
     stats[i].mu[2] /= stats[i].size;
     stats[i].mu[3] /= stats[i].size;
     stats[i].mu[4] /= stats[i].size;

     stats[i].Sigma[5 * 0 + 0] /= stats[i].size;
     stats[i].Sigma[5 * 0 + 1] /= stats[i].size;
     stats[i].Sigma[5 * 0 + 2] /= stats[i].size;
     stats[i].Sigma[5 * 0 + 3] /= stats[i].size;
     stats[i].Sigma[5 * 0 + 4] /= stats[i].size;

     stats[i].Sigma[5 * 1 + 1] /= stats[i].size;
     stats[i].Sigma[5 * 1 + 2] /= stats[i].size;
     stats[i].Sigma[5 * 1 + 3] /= stats[i].size;
     stats[i].Sigma[5 * 1 + 4] /= stats[i].size;

     stats[i].Sigma[5 * 2 + 2] /= stats[i].size;
     stats[i].Sigma[5 * 2 + 3] /= stats[i].size;
     stats[i].Sigma[5 * 2 + 4] /= stats[i].size;

     stats[i].Sigma[5 * 3 + 3] /= stats[i].size;
     stats[i].Sigma[5 * 3 + 4] /= stats[i].size;

     stats[i].Sigma[5 * 4 + 4] /= stats[i].size;
   }

   for(i=0; i < count; i++) {
     printf("size:  %i\n", stats[i].size);
     printf("mu:    %3.2f %3.2f %3.2f %3.2f %3.2f\n",
            stats[i].mu[0],
            stats[i].mu[1],
            stats[i].mu[2],
            stats[i].mu[3],
            stats[i].mu[4]
            );
     printf("Sigma: %8.2f %8.2f %8.2f %8.2f %8.2f\n",
            stats[i].Sigma[5 * 0 + 0],
            stats[i].Sigma[5 * 0 + 1],
            stats[i].Sigma[5 * 0 + 2],
            stats[i].Sigma[5 * 0 + 3],
            stats[i].Sigma[5 * 0 + 4]
            );
     printf("                %8.2f %8.2f %8.2f %8.2f\n",
            stats[i].Sigma[5 * 1 + 1],
            stats[i].Sigma[5 * 1 + 2],
            stats[i].Sigma[5 * 1 + 3],
            stats[i].Sigma[5 * 1 + 4]
            );
     printf("                         %8.2f %8.2f %8.2f\n",
            stats[i].Sigma[5 * 2 + 2],
            stats[i].Sigma[5 * 2 + 3],
            stats[i].Sigma[5 * 2 + 4]
            );
     printf("                                  %8.2f %8.2f\n",
            stats[i].Sigma[5 * 3 + 3],
            stats[i].Sigma[5 * 3 + 4]
            );
     printf("                                           %8.2f\n",
            stats[i].Sigma[5 * 4 + 4]
            );
   }

   srm_delete(srm);

	return 0;

}
