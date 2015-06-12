#include <iostream.h>
#include "var_commune.hh"
#include "srmBproc.hh"
#include "options.hh"
#include "noise.hh"
#include <string.h>

inline void iPSIS(int* &nummod,region** &bm, int Wi, int Le){
  PSIS(nummod,bm,Wi,Le);
}

/*---------------------------------------------------------*/ 

int main(int argc, char * argv[]) { 
  int Le, Wi;
  regionC** bm; 
  int* nummod;

  if(!lire_options(argc,argv)) exit(1);

  readBmRGB(inputfile, bm, Wi, Le);
  readModeles(inputfile, nummod, Wi, Le);
  claqueModeles(bm, nummod, Wi, Le);

  if (TypeOfNoise>0)
    {
      if (TypeOfNoise==1)
	{
	  randomnoise(bm,Wi,Le,Noise);
	}
      else if (TypeOfNoise==2)
	{
	  saltandpepper(bm,Wi,Le,Noise);
	}
      else if (TypeOfNoise==3)
	{
	  normalnoise(bm,Wi,Le,0,Noise);
	}
      strcpy(outputfileNoise,outputfile);
      strcat(outputfileNoise,"-original-noise");
    }
  //gen_occlusion(bm,Wi,Le);
  if (TypeOfNoise>0)
    {
      sauveRGBimageNoisy(outputfileNoise,bm,Wi,Le);
    }

  //char_models_part(bm, 2, Wi, Le);
  iPSIS(nummod,bm,Wi,Le);
  //char_models_part(bm, 2, Wi, Le);

  eliminer_petites_regions(bm, Wi, Le);

  if (OMerge==1)
    OcclusionMerge(bm,Wi,Le);
  else if (OMerge==2)
    OcclusionMergeSort(bm,Wi,Le);

  if ( (PartialRegions==1) && (NModels==0) ) sauveParts(bm, Wi, Le);

  if (NModels>0) sauveRGBimageModels(nummod,outputfile,bm,Wi,Le);
  else 
    sauveRGBimage(outputfile,bm,Wi,Le);

  if ( (PartialRegions==1) && (NModels>0) ) sauvePartsModels(bm, Wi, Le);

  free(bm);

  return 1; 
} 
