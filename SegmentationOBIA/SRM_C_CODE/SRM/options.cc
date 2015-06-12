#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __GNUG__
#include "getopt.h"
#endif

#ifndef GETOPT_H
extern int optind, opterr; // pour getopt()
extern char	*optarg;
#endif

#include "var_commune.hh"

char outputfile[255];
char outputfileNoise[255];  
char inputfile[255];
int Sortie;
int BorderColor;
int PartialRegions;
int Noise;
int TypeOfNoise;
int TypeOfNeighbor;
int OMerge;
int SobelGradient;
int fSize;
double Q;

char option_char;

int lire_options(int argc, char * argv[]){

  strcpy(outputfile,"result.tiff");
  Sortie=0;
  BorderColor=0;
  PartialRegions=0;
  Noise=0;
  TypeOfNoise=0;
  TypeOfNeighbor=0;
  OMerge=0;
  SobelGradient=0;
  Q=32;
  fSize=-1;

  while ((option_char = getopt (argc, argv, "h:S:B:P:T:N:U:G:O:F:Q:s:")) != EOF) {
    switch (option_char) {
    case 'S' : 
      Sortie=atoi(optarg);
      break;
    case 'B' :
      BorderColor=atoi(optarg);
      break;
    case 'P' :
      PartialRegions=atoi(optarg);
      break;
    case 'T' : 
      TypeOfNoise=atoi(optarg);
      break;
    case 'N' : 
      Noise=atoi(optarg);
      break;
    case 'U' : 
      TypeOfNeighbor=atoi(optarg);
      break;
    case 'G' : 
      SobelGradient=atoi(optarg);
      break;
    case 'F' : 
      fSize=atoi(optarg);
      break;
    case 'O' : 
      OMerge=atoi(optarg);
      break;
    case 'Q' : 
      Q=(double) (atoi(optarg));
      break;
    case 's' :
      strcpy(inputfile,optarg);
      printf("input file... %s\n",inputfile);
      break;
    case 'h' :
    default  : cout << "Usage :\n";
      cout << argv[0] << " [option]* -s input.tiff output.tiff\n";
      cout << " Welcome on srm (Statistical Region Merging), By R. Nock and F. Nielsen\n";
      cout << " Important notice on this software:\n";
      cout << "    * it goes with our (revised) submission to TPAMI, please do not distribute\n";
      cout << "    * it is provided AS IS, without ANY guarantee\n";
      cout << "    * it is provided for RESEARCH PURPOSE ONLY\n\n";
      cout << " Options:\n";
      cout << "  h : displays this help\n";
      cout << "  S : in {0,1,2,3}, type of output (0=bordered regions with original pixels, \n";
      cout << "                                    1=black-bordered regions with white pixels, \n";
      cout << "                                    2=bordered regions with average-color pixels, \n";
      cout << "                                    3=randomized colored regions)\n";
      cout << "  B : 0-2, border color\n";
      cout << "      0=black (valid for S=0,2)\n";
      cout << "      1=white (valid for S=0,2)\n";
      cout << "      2=no border (valid for S=0,2)\n";
      cout << "  P : 0-1, saves (when P=1) the (at most) 15 largest regions of the segmentation\n";
      cout << "           in files named [output.tiff]_RegionX, for X=0, 1, ...\n";
      cout << "  T : in {0,1,2,3,4}, type of noise (0=no noise, \n";
      cout << "                                     1=transmission noise, \n";
      cout << "                                     2=salt and pepper noise, \n";
      cout << "                                     3=Gaussian (Cf Xu et al., PRL (19) pp 1213--1224, 1998), \n";
      cout << "                                     4=artificial occlusions (s random occlusions) )\n";
      cout << "  N : 0-100, % of pixels having noise (transmission, salt and pepper), \n";
      cout << "      >0   : sigma (Gaussian noise)\n";
      cout << "      >0   : s (random occlusions)\n";
      cout << "  U : 0-1, type of neighborhood for sorting couples:\n";
      cout << "      0  : original (Delta=0)\n";
      cout << "      1  : for noise handling (Delta=10)\n";
      cout << "  G : 0-1, convolution filter for sorting couples:\n";
      cout << "      0  : no\n";
      cout << "      1  : Sobel-type of arbitrary size\n";
      cout << "  F : >1, parameter for the size of the convolution filter when G=1 (actual size=2*F+1):\n";
      cout << "  O : 0-1, occlusions merging (0 is No, 1 is Yes)\n";
      cout << "  Q : >=1, complexity parameter of the statistical model (Cf paper)\n\n";
      cout << " Default settings: S=B=P=T=N=U=G=O=0, Q=32\n\n";
      cout << " Notices: \n";
      cout << "   * noisified images are also saved; the Filename is [output.tiff]-original-noisified\n";
      cout << "   * when P=1 and images are noisified, the regions saved are those of the -original image-\n";
      return(0);
    }
  }
  if (  optind < argc  ) strcpy(outputfile,argv[optind]);
  else{
    cout << "Usage : " << argv[0] << " [option]* -r inputRed.tiff -g inputGreen.tiff -b inputBlue.tiff output.tiff\n";
    cerr << "ERROR: bad line command" << endl;
    return(0);
  }

  if ( (SobelGradient>=1) && (fSize<0) ){
    cerr << "ERROR: please specify the Filter size parameter (-F)\n";
    return(0);
  }

  return 1;
}
