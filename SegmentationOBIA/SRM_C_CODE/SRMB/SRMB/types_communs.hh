#ifndef types_communs_hh
#define types_communs_hh

struct voisin {
  int idvois;
  int nbcontour;
  int  nbfront;
  voisin *suiv;
};

struct Case {
  unsigned short int contour;
  int Hposs, Hreg;
  int Cont, Front;
  voisin* ptvois;
};

struct regionC;

struct cell {
  Case* tab;
  int H, ind;
  cell *pred, *suiv;
  regionC* region;
};

typedef struct metaRegion metaRegion;

struct regionC { 
  int somR, somG, somB, somAlpha, nb, no, valoriR, valoriG, valoriB, valoriAlpha,
    valoriRSauv, valoriGSauv, valoriBSauv, valoriAlphaSauv, randR, randG, randB;
  
  regionC* parent;
  cell* ptliste;

  int numMod;

  metaRegion* meta;
};

typedef regionC region;

struct sommet {
  regionC* ptregion;
  voisin* listevoisin;
};

struct metaRegion {
  int nom;
  int nMod;
  regionC** tabMod;
  int* tabI;
  int* tabJ;

  int somR, somG, somB, somAlpha, nb;
};

#endif
