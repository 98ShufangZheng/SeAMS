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
int NModels;
int ShowModels;
int ModelSize; //taille carree=1+2*(ModelSize-1)

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
  NModels=0;
  ShowModels=1;
  ModelSize=1;

  while ((option_char = getopt (argc, argv, "h:S:B:P:b:M:s:")) != EOF) {
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
    case 'b' : 
      ShowModels=atoi(optarg);
      break;
    case 'M' : 
      NModels=atoi(optarg);
      break;
    case 's' :
      strcpy(inputfile,optarg);
      printf("input file... %s\n",inputfile);
      break;
    case 'h' :
    default  : 
      cout << "\nWelcome.\n";
      cout << "srmB is (c) 2003/04 R. Nock, F. Nielsen. It was compiled and run on a Mac Pro 64 bits.\n";
      cout << "srmB implements our paper 'Grouping with Bias Revisited', CVPR'04.\n";
      cout << "This program is provided AS IS, without any warranty. Use it at your own risk !\n\n\n";
      cout << "Usage :\n";
      cout << argv[0] << " [option]* -s input.tiff output.tiff\n";
      cout << " Options:\n";
      cout << "  h : display this help\n";
      cout << "  S : 0,1,2,3 type of output (0=Bordered regions with original pixels, \n";
      cout << "                             1=Black-bordered regions with white pixels, \n";
      cout << "                             2=Bordered regions with average-color pixels)\n";
      cout << "                             3=Randomized colored regions)\n";
      cout << "  B : 0-1, border color (0=black, 1=white, valid for S=0,2)\n";
      cout << "  P : 0-1, saves (when P=1) the (at most) 15 largest regions of the segmentation\n";
      cout << "      in files named fileRegionX, for X=0, 1, ...\n";
      cout << "  b: is models >0, Boolean (0=No/1=Yes) stating whether the bias is to be shown on the output\n";
      cout << "  M: Models specified (0 is no model)\n"; 
      cout << "     The models have to be stored into separate files named input.tiff_modeleX.txt, for X=1, 2, ... (do not forget the .txt suffix)\n"; 
      cout << " Default settings: S=B=P=M=0, b=1\n\n"; 
      cout << " Format of model files:\n"; 
      cout << "  each file contains as rows the coordinates (rom, column) of each pixel model\n";
      cout << "  each file ends by the row -1[tab]-1\n";
      cout << "  example for model file flower.tif_modele1.txt:\n\n";
      cout << "   195     245\n";
      cout << "   360     320\n";
      cout << "   290     60\n";
      cout << "   -1      -1\n\n";
      cout << "--- Comments and suggestions much welcomed, at: rnock@martinique.univ-ag.fr\n\n";
      return(0);
    }
  }
  if (  optind < argc  ) strcpy(outputfile,argv[optind]);
  else{
    cout << "Usage : " << argv[0] << " [option]* -s input.tiff output.tiff\n";
    cerr << "ERROR bad line command" << endl;
    return(0);
  }

  return 1;
}
