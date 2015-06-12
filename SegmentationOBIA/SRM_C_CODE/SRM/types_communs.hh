#ifndef types_communs_hh
#define types_communs_hh

struct regionC { 
  int somR, somG, somB, somAlpha, 
    nb, no, 
    valoriR, valoriG, valoriB, valoriAlpha, 
    valoriRSauv, valoriGSauv, valoriBSauv, valoriAlphaSauv, 
    randR, randG, randB;
  
  regionC* parent;
};

typedef regionC region;

#endif
