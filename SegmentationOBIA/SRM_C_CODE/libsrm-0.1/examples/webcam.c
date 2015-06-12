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

	CvCapture* capture;
   IplImage* image;
   IplImage* segmented;

	capture = cvCaptureFromCAM(CV_CAP_ANY);
   /* cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320); */
   /* cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240); */

   image = cvQueryFrame(capture);
   printf("nchannels %i\ndepth %i\nwidthStep %i\n", image->nChannels, image->depth, image->widthStep);
   segmented = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);

	cvNamedWindow("Statistical Region Merging", CV_WINDOW_AUTOSIZE);

   struct srm *srm = srm_new(512.0, image->width, image->height, image->nChannels, 0);

	while (key != 'q') {
	   image = cvQueryFrame(capture);

      srm_run(srm, image->widthStep, (uint8_t*)image->imageData, segmented->widthStep, (uint8_t*)segmented->imageData);

	   cvShowImage("Statistical Region Merging", segmented);

	   key = cvWaitKey(1);
	}

   srm_delete(srm);
	cvReleaseCapture(&capture);
	cvDestroyWindow("Statistical Region Merging");

	return 0;

}
