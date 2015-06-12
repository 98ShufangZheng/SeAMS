// This is the SRM Source code
// It goes with our TPAMI Submission
// Consult on-line help for more details

#include <iostream.h>
#include "var_commune.hh"
#include "srmproc.hh"
#include "options.hh"
#include <string.h>

inline void iSRM(region** &bm, int Wi, int Le){
  SRM(bm,Wi,Le);
}

/*---------------------------------------------------------*/ 

int main(int argc, char * argv[]) { 
  int Le, Wi;
  regionC** bm; 
  if(!lire_options(argc,argv)) exit(1);

  readBmRGB(inputfile, bm, Wi, Le);
  if (TypeOfNoise>0)
    {
      printf("Noisification of the image using ");
      if (TypeOfNoise==1)
	{
	  printf("transmission noise...");
	  randomnoise(bm,Wi,Le,Noise);
	}
      else if (TypeOfNoise==2)
	{
	  printf("salt and pepper noise...");
	  saltandpepper(bm,Wi,Le,Noise);
	}
      else if (TypeOfNoise==3)
	{
	  printf("Gaussian noise...");
	  normalnoise(bm,Wi,Le,0,Noise);
	}
      else if (TypeOfNoise==4)
	{
	  printf("artificial occlusions...");
	  gen_occlusion(bm,Wi,Le,Noise);
	}
      else{
	printf("...Bad noise value (-T)\n");
	exit(0);
      }
      strcpy(outputfileNoise,outputfile);
      strcat(outputfileNoise,"-original-noisified.tif");
      printf("done\n");
    }
  if (TypeOfNoise>0)
    {
      printf("Saving the noisified image...");
      sauveRGBimageNoisy(outputfileNoise,bm,Wi,Le);
      printf("done\n");
    }

  iSRM(bm,Wi,Le);
  eliminer_petites_regions(bm, Wi, Le);
  if (OMerge==1)
    OcclusionSortSRM(bm,Wi,Le);

  if (PartialRegions==1)
      sauveParts(bm, Wi, Le);

  sauveRGBimage(outputfile,bm,Wi,Le);
  free(bm);

  return 1; 
} 
