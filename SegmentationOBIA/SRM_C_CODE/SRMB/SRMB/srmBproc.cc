#include "var_commune.hh"
#include "srmBproc.hh"
#include "saveimage.hh"
#include "limits.h"
#include "float.h"
#include "assert.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define minimum(x,y) ( (x) < (y) ? (x) : (y) )
#define maximum(x,y) ( (x) > (y) ? (x) : (y) )
#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )
#define absolu(x) ( (x) < 0 ? (-x) : (x) )

#define PI_MATH 3.141592
#define RAYON_CERCLE 10
#define TAILLE_POINT 4
#define MONTRE_TXT 0
#define NB_MAX_REGION 25

#define SOBEL_GRADIENT 1

//Lena visage : LUX=180, LUY=200, RDX=400, RDY=360
//New09 Facade: LUX=100, LUY=1, RDX=390, RDY=280
//New17 Chateau: LUX=1, LUY=80, RDX=310, RDY=375
//StPierre Route: LUX=490, LUY=220, RDX=637, RDY=478
//Jerome Notre dame: LUX=20, LUY=50, RDX=350, RDY=460
//Flower bee: LUX=80, LUY=80, RDX=220, RDY=200
//Leopard cheetah: LUX=50, LUY=120, RDX=290, RDY=490
//Chevreuil2 herbe: LUX=270, LUY=1, RDX=425, RDY=190
//Laie laie: LUX=1, LUY=150, RDX=390, RDY=639
//Blaireau Tete: LUX=110, LUY=250, RDX=270, RDY=395
//Bat Chauve-souris: LUX=70, LUY=90, RDX=260, RDY=240
//Chamois Head: LUX=1, LUY=360, RDX=265, RDY=520

#define LUX 1
#define LUY 360
#define RDX 265
#define RDY 520
#define PICK_MOD 0
#define NUMERO_MOD 1

double minint=256;
double maxint=0;
int mergesMade=0;

double maxmax=0;
double minmin=256;

int checkint;
int NIT=0;

static char BUF[12];
char* itoa(int i)
{
  char *pos = BUF + sizeof(BUF) - 1;
  unsigned int u;
  int negative = 0;
  if (i < 0){
    negative = 1;
    u = ((unsigned int)(-(1+i))) + 1;
  } else {
    u = i;
  }
  *pos = 0;
  do {
    *--pos = '0' + (u % 10);
    u /= 10;
  } while (u);
  if (negative) {
    *--pos = '-';
  }
  return pos;
}

void readModeles(const char *prefixfile,int* &nummod, int Wi, int Le){
  FILE* fichier;
  char ns[255];
  int i,j,k,cx,cy,index;

  j=0;
  nummod= (int*) (malloc(sizeof(int)*Wi*Le));
  for (i=0;i<Le;i++){
    for (j=0;j<Wi;j++){
      nummod[i*Wi+j]=0;
    }
  }

  for (k=0;k<NModels;k++){
    strcpy(ns,prefixfile);
    strcat(ns,"_modele");
    strcat(ns,itoa(k+1));
    strcat(ns,".txt");

    fichier=fopen(ns,"r");
    do{
      fscanf(fichier,"%d\t%d\n",&cx,&cy);
      if (cx!=-1){
	assert(cy!=-1);
	nummod[(Le-1-cx)*Wi+cy]=k+1;
	//nummod[cx*Wi+cy]=k+1;
	//nummod[cy*Wi+(Le-cx)]=k+1;
      }else{
	assert(cy==-1);
      }
    }while(cx!=-1);
    fclose(fichier);
  }

  index=1;
  for (i=0;i<Le;i++){
    for (j=0;j<Wi;j++){
      if (nummod[(Le-1-i)*Wi+j]==0){
	nummod[(Le-1-i)*Wi+j]=-index;
	index++;
      }
    }
  }
}

void claqueModeles(regionC** &bm,int* &nummod, int Wi, int Le){
  char* select;
  int i,j,k,l,m,n,taille,refx,refy,index,trouve;

  select= (char*) (malloc(sizeof(char)*Wi*Le));
  for (i=0;i<Le;i++){
    for (j=0;j<Wi;j++){
      select[i*Wi+j]='.';
    }
  }

  for (i=0;i<Le;i++){
    for (j=0;j<Wi;j++){
      if (select[i*Wi+j]=='.'){
	if (nummod[i*Wi+j]<0){
	  bm[i*Wi+j]->meta= (metaRegion*) (malloc(sizeof(metaRegion)));
	  (bm[i*Wi+j]->meta)->nom=nummod[i*Wi+j];
	  (bm[i*Wi+j]->meta)->nMod=1;
	  (bm[i*Wi+j]->meta)->tabMod=&bm[i*Wi+j];

	  (bm[i*Wi+j]->meta)->somR=bm[i*Wi+j]->valoriR;
	  (bm[i*Wi+j]->meta)->somG=bm[i*Wi+j]->valoriG;
	  (bm[i*Wi+j]->meta)->somB=bm[i*Wi+j]->valoriB;
	  (bm[i*Wi+j]->meta)->somAlpha=bm[i*Wi+j]->valoriAlpha;
	  (bm[i*Wi+j]->meta)->nb=1;

	  bm[i*Wi+j]->numMod=-1;

	  select[i*Wi+j]='*';
	}else{
	  assert(nummod[i*Wi+j]>0);
	  taille=0;
	  refx=-1;
	  refy=-1;
	  for (k=0;k<Le;k++){
	    for (l=0;l<Wi;l++){
	      if ( (nummod[k*Wi+l]==nummod[i*Wi+j]) && (refx==-1) ){
		refx=k;
		refy=l;
	      }
	      taille+=( (nummod[k*Wi+l]==nummod[i*Wi+j]) ? 1 : 0 );
	    }
	  }
	  index=0;
	  trouve=0;
	  for (k=0;k<Le;k++){
	    for (l=0;l<Wi;l++){
	      if ( (k==refx) && (l==refy) ){
		bm[k*Wi+l]->meta= (metaRegion*) (malloc(sizeof(metaRegion)));
		(bm[k*Wi+l]->meta)->nom=nummod[refx*Wi+refy];
		(bm[k*Wi+l]->meta)->nMod=taille;
		(bm[k*Wi+l]->meta)->tabMod= (regionC**) (malloc(sizeof(regionC*)*taille));
		(bm[k*Wi+l]->meta)->tabMod[index]=bm[k*Wi+l];

		(bm[k*Wi+l]->meta)->tabI=(int*) (malloc(sizeof(int)*taille));
		(bm[k*Wi+l]->meta)->tabJ=(int*) (malloc(sizeof(int)*taille));
		(bm[k*Wi+l]->meta)->tabI[index]=k;
		(bm[k*Wi+l]->meta)->tabJ[index]=l;

		(bm[k*Wi+l]->meta)->somR=bm[refx*Wi+refy]->valoriR;
		(bm[k*Wi+l]->meta)->somG=bm[refx*Wi+refy]->valoriG;
		(bm[k*Wi+l]->meta)->somB=bm[refx*Wi+refy]->valoriB;
		(bm[k*Wi+l]->meta)->somAlpha=bm[refx*Wi+refy]->valoriAlpha;
		(bm[k*Wi+l]->meta)->nb=1;

		bm[k*Wi+l]->numMod=index;
		select[k*Wi+l]='*';

		for (m=k-(ModelSize-1); m<=k+(ModelSize-1); m++){
		  for (n=l-(ModelSize-1); n<=l+(ModelSize-1); n++){
		    if ( (m>=0) && (m<Le) && (n>=0) && (n<Wi) && (m!=refx) && (n!=refy) ){
		      //printf("%d\n",ModelSize);
		      bm[m*Wi+n]->numMod=index;
		      select[m*Wi+n]='*';
		      FlatUnion(bm[k*Wi+l],bm[m*Wi+n]);
		    }
		  }
		}

		index++;
		trouve=1;
	      }else if (nummod[k*Wi+l]==nummod[refx*Wi+refy]){
		assert(trouve==1);
		bm[k*Wi+l]->meta=bm[refx*Wi+refy]->meta;
		(bm[k*Wi+l]->meta)->tabMod[index]=bm[k*Wi+l];
		(bm[k*Wi+l]->meta)->tabI[index]=k;
		(bm[k*Wi+l]->meta)->tabJ[index]=l;

		(bm[k*Wi+l]->meta)->somR+=bm[refx*Wi+refy]->valoriR;
		(bm[k*Wi+l]->meta)->somG+=bm[refx*Wi+refy]->valoriG;
		(bm[k*Wi+l]->meta)->somB+=bm[refx*Wi+refy]->valoriB;
		(bm[k*Wi+l]->meta)->somAlpha+=bm[refx*Wi+refy]->valoriAlpha;
		(bm[k*Wi+l]->meta)->nb++;

		bm[k*Wi+l]->numMod=index;
		select[k*Wi+l]='*';

		for (m=k-(ModelSize-1); m<=k+(ModelSize-1); m++){
		  for (n=l-(ModelSize-1); n<=l+(ModelSize-1); n++){
		    if ( (m>=0) && (m<Le) && (n>=0) && (n<Wi) && (m!=k) && (n!=l) ){
		      //printf("%d\n",ModelSize);
		      bm[m*Wi+n]->numMod=index;
		      select[m*Wi+n]='*';
		      FlatUnion(bm[k*Wi+l],bm[m*Wi+n]);
		    }
		  }
		}

		index++;
	      }
	    }
	  }
	}
      }
    }
  }

  //char_image(bm,Wi,Le);

  free(select);
}

void char_image(regionC** &bm, int Wi, int Le){
  //procedure ideale pour new11.tif (110x103)
  int i,j;
  for (i = 0; i < Le; i++) 
   {   	for (j = 0; j < Wi; j++) 
	{
	  if ((bm[(Le-i-1)*Wi+j]->meta)->nom>0) printf("0");
	  else if ((bm[(Le-i-1)*Wi+j]->valoriR)>50) printf("*");
	  else printf(".");
	}
   printf("\n");
   }
}

void char_data_image(unsigned int* data, int Wi, int Le){
  //procedure ideale pour new11.tif (110x103)
  int i,j;
  for (i = 0; i < Le; i++) 
   {   	for (j = 0; j < Wi; j++) 
	{
	  if (((data[(Le-i-1)*Wi+j]<<24)>>24)>50) printf("*");
	  else printf(".");
	}
   printf("\n");
   }
}

void char_models(int* &nummod, regionC** &bm, int Wi, int Le){
  //procedure ideale pour new11.tif (110x103)
  int i,j;
  for (i = 0; i < Le; i++) 
   {   	for (j = 0; j < Wi; j++) 
	{
	  //if ( ((bm[(Le-i-1)*Wi+j]->meta)->nom>0) && ((bm[(Le-i-1)*Wi+j]->meta)->tabI[bm[(Le-i-1)*Wi+j]->meta->nom-1]==(Le-i-1))
	  //     && ((bm[(Le-i-1)*Wi+j]->meta)->tabJ[bm[(Le-i-1)*Wi+j]->meta->nom-1]==j) ) printf("X");
	  if (nummod[(Le-1-i)*Wi+j]>0) printf("X");
	  else if (MetaFindCompress(bm[(Le-i-1)*Wi+j])->nom>0) printf("%d",MetaFindCompress(bm[(Le-i-1)*Wi+j])->nom);
	  //else if ((bm[(Le-i-1)*Wi+j]->meta)->nom>0) printf("%d",bm[(Le-i-1)*Wi+j]->meta->nom);
	  else if ((bm[(Le-i-1)*Wi+j]->valoriR)>50) printf("*");
	  else printf(".");
	}
   printf("\n");
   }
  printf("\n");
}

void char_models_part(regionC** &bm, int num, int Wi, int Le){
  //procedure ideale pour new11.tif (110x103)
  int i,j;
  metaRegion* m;
  regionC* c;
  for (i = 0; i < Le; i++) 
   {   	for (j = 0; j < Wi; j++) 
	{
	  c=FindCompress(bm[(Le-i-1)*Wi+j]);
	  m=c->meta;
	  if (m->nom==num){
	    printf("%d",(c->nb)%10);
	    /*for (k=0;k<m->nMod;k++){
	      if ( (m->tabI[k]==i) && (m->tabJ[k]==j) ){
		printf("%d",k);
	      }
	      }*/
	  }else if ((bm[(Le-i-1)*Wi+j]->valoriR)>50) printf("*");
	  else printf(".");
	}
   printf("\n");
   }
  printf("\n");
}

int readBmRGB(const char *filename, regionC** &bm, int &Wi, int &Le)
{
  if (readRGBtiffparam(filename, &Wi, &Le) == 0)
	{	printf ("erreur readRGBtiffparam"); return 0; }

  unsigned int *buffer= (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));

  if (readRGBtiff(filename, buffer) == 0)
	{	printf ("erreur readRGBtiff"); return 0; }
   bm = (regionC**) (malloc(sizeof(regionC*)*Wi*Le));
   for (int i = 0; i < Le; i++) 
   {   	for (int j = 0; j < Wi; j++) 
	{
	  bm[i*Wi+j]              = new region; 
	  bm[i*Wi+j]->meta        = NULL;
	  bm[i*Wi+j]->parent      = bm[i*Wi+j];
	  bm[i*Wi+j]->somR        = TIFFGetR(buffer[i*Wi+j]);
	  bm[i*Wi+j]->somG        = TIFFGetG(buffer[i*Wi+j]);
	  bm[i*Wi+j]->somB        = TIFFGetB(buffer[i*Wi+j]);
	  bm[i*Wi+j]->somAlpha    = TIFFGetA(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriR     = TIFFGetR(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriG     = TIFFGetG(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriB     = TIFFGetB(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriAlpha = TIFFGetA(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriRSauv     = TIFFGetR(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriGSauv     = TIFFGetG(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriBSauv     = TIFFGetB(buffer[i*Wi+j]);
	  bm[i*Wi+j]->valoriAlphaSauv = TIFFGetA(buffer[i*Wi+j]);
	  bm[i*Wi+j]->randR     = rand()%256;
	  bm[i*Wi+j]->randG     = rand()%256;
	  bm[i*Wi+j]->randB     = rand()%256;
	  bm[i*Wi+j]->nb = 1; 
	  bm[i*Wi+j]->no = -1;
	}
   }

   free(buffer);

   return 1;
}


int isTop(regionC* p) 
{ 
   if (p->parent == p) 
	return 1; 
   else	return 0; 
} 
 
regionC* FindCompress(regionC* p) 
{ 
   if (isTop(p)) return p; 
   else{
     p->numMod=(p->parent)->numMod;
     p->meta=(p->parent)->meta;
     p->parent = FindCompress(p->parent); 
     return p->parent;
   }
} 

metaRegion* MetaFindCompress(regionC* p) 
{ 
   if (isTop(p)) return p->meta; 
   else{
     p->parent = FindCompress(p->parent); 
     p->meta=(p->parent)->meta;
     p->numMod=(p->parent)->numMod;
     return p->parent->meta;
   }
} 

int numerote(regionC** &bm, int Wi, int Le) 
{ 
   int cpt = 0;

   regionC* p; 
   for (int i = 0; i < Le; i++) 
   {    for (int j = 0; j < Wi; j++) 
        {       p = FindCompress(bm[i*Wi+j]); 
                if(p->no == -1) 
                {       p->no = cpt; 
                        cpt++; 
                } 
        }
   } 
   return (cpt);
} 

int numeroteModels(regionC** &bm, int Wi, int Le) 
{ 
   int cpt = 0;

   metaRegion* p; 
   for (int i = 0; i < Le; i++) 
   {    for (int j = 0; j < Wi; j++) 
        {       p = topModel(bm[i*Wi+j]); 
                if(p->nom == -1) 
                {       p->nom = cpt; 
                        cpt++; 
                } 
        }
   } 
   //printf("nombre de modeles:%d\n",cpt);
   return (cpt);
} 

int renumeroteModels(regionC** &bm, int Wi, int Le) 
{ 
   for (int i = 0; i < Le; i++){
     for (int j = 0; j < Wi; j++){       
       topModel(bm[i*Wi+j])->nom= -1; 
     }  
   } 
   return numeroteModels(bm,Wi,Le);
} 

int renumerote(regionC** &bm, int Wi, int Le) 
{ 
   for (int i = 0; i < Le; i++){
     for (int j = 0; j < Wi; j++){       
       FindCompress(bm[i*Wi+j])->no= -1; 
     }  
   } 
   return numerote(bm,Wi,Le);
} 


cell* first(cell* &p)
{
  cell* tamp = p;

  if (p->pred != NULL) {
    p->pred->suiv = p->suiv;
  }
  if (p->suiv != NULL) {
    p->suiv->pred = p->pred;
  }
  p = p->suiv;

  tamp->suiv = NULL;
  tamp->pred = NULL;

  return (tamp);
}

void puthead(cell* x, cell* &l, cell* &ptsuppr)
{
  if (ptsuppr == x){
    ptsuppr = x->suiv;
  }
  if(l != x) {
    if (x->pred != NULL) {
      x->pred->suiv = x->suiv;
    }
    if (x->suiv != NULL) {
      x->suiv->pred = x->pred;
    }
    l->pred = x;
    x->pred = NULL;
    x->suiv = l;
    l = x;
  }
}



void Flatten(int i, int Wi, regionC** &bm) 
{ 
   for (int j = 0; j < Wi; j++) 
   { bm[i*Wi+j] = FindCompress(bm[i*Wi+j]); } 
} 
 
void Union (regionC* x, regionC* y) 
{
   if (x != y){
     //printf("\t\t\t\t ra(x)->meta=%p\t rb(y)->meta=%p\n",x->meta,y->meta);
     //printf("\t\t\t\t ra(x)->meta->nom=%d\t rb(y)->meta->nom=%d, checkint=%d\n",x->meta->nom,y->meta->nom,checkint);

     if ((NModels>0) && (topModel(y)->nom >0) && (topModel(x)->nom >0) ){
       assert(topModel(y)==topModel(x));
       printf("Union: error, merging different models from the same region together; checkint=%d, NIT=%d !\n",checkint,NIT);
       exit(-1);
     }
     assert(!(NModels>0) || (topModel(y)->nom <0) );

     if ( (NModels>0) && (topModel(y)->nom <0) && (topModel(x)->nom > 0)){
       y->meta=topModel(x);

       topModel(x)->somR+=y->somR;
       topModel(x)->somG+=y->somG;
       topModel(x)->somB+=y->somB;
       topModel(x)->somAlpha+=y->somAlpha;
       topModel(x)->nb+=y->nb;
     }else{
       y->meta=topModel(x);

       topModel(x)->somR+=y->somR;
       topModel(x)->somG+=y->somG;
       topModel(x)->somB+=y->somB;
       topModel(x)->somAlpha+=y->somAlpha;
       topModel(x)->nb+=y->nb;
     }

     x->somR       = x->somR + y->somR;
     x->somG       = x->somG + y->somG;
     x->somB       = x->somB + y->somB;
     x->somAlpha   = x->somAlpha + y->somAlpha;

     x->nb = y->nb + x->nb;

     y->numMod=x->numMod;
     y->parent = x;

     /*y->somR       = x->somR + y->somR;
       y->somG       = x->somG + y->somG;
       y->somB       = x->somB + y->somB;
       y->somAlpha   = x->somAlpha + y->somAlpha;
       
       y->nb = y->nb + x->nb;
       x->parent = y;*/
   } 
} 

void FlatUnion (regionC* x, regionC* y) 
{
   if (x != y){
     y->meta=topModel(x);

     topModel(x)->somR+=y->somR;
     topModel(x)->somG+=y->somG;
     topModel(x)->somB+=y->somB;
     topModel(x)->somAlpha+=y->somAlpha;
     topModel(x)->nb+=y->nb;

     x->somR       = x->somR + y->somR;
     x->somG       = x->somG + y->somG;
     x->somB       = x->somB + y->somB;
     x->somAlpha   = x->somAlpha + y->somAlpha;

     x->nb = y->nb + x->nb;

     y->numMod=x->numMod;
     y->parent = x;
   }
} 

int nouvelle_region(regionC* x, cell* &ptfree, int i, int &cpfree, int
                    Wi)
{
  if (ptfree == NULL) {
    x->ptliste = new cell;
    if (x->ptliste == NULL) {
      printf("plus de place mémoire...\n");
      return(0);
    }
    x->ptliste->tab = (Case*) (malloc(sizeof(Case)*Wi*2));
    if (x->ptliste->tab == NULL) {
      printf("plus de place mémoire...\n");
      return(0);
    }
    for (int i = 0; i < (Wi*2); i++) {
      x->ptliste->tab[i].contour = 0;
      x->ptliste->tab[i].Hposs = 0;
      x->ptliste->tab[i].Hreg = 0;
      x->ptliste->tab[i].Cont = 0;
      x->ptliste->tab[i].Front = 0;
    }
    x->ptliste->ind = cpfree;
    cpfree++;
    x->ptliste->pred = NULL;
    x->ptliste->suiv = NULL;
  }
  else {
    x->ptliste = first(ptfree);
  }
  x->ptliste->H = i;
  return (1);
}

int metacheckpixel(metaRegion* r1, metaRegion* r2, int color)
{
  return ((r1 != r2) && (TIFFGetR(color) != 0) && (TIFFGetG(color) != 0) && (TIFFGetB(color) != 0));
  //return (r1->valori != r2->valori);
}

int checkpixel(regionC* r1, regionC* r2, int color)
{
  return ((r1 != r2) && (TIFFGetR(color) != 0) && (TIFFGetG(color) != 0) && (TIFFGetB(color) != 0));
  //return (r1->valori != r2->valori);
}

void point(unsigned int* data, int x, int y, int T, int Wi, int Le, int num){
  int i,j;
  double redv, greenv, bluev;
  int pixelR, pixelG, pixelB, pixelA;

  redv= ( (1/(double) (2*NModels)) + ((double) num)/((double) NModels) );
  greenv=( (redv<=0.5) ?  (2*redv) : (2*(1-redv)) );
  bluev=1-redv;

  pixelR=(int) (255*redv);
  pixelG=(int) (255*greenv);
  pixelB=(int) (255*bluev);
  pixelA=0;

  for (i=y-(T/2);i<=y+(T/2);i++) 
    for (j=x-(T/2);j<=x+(T/2);j++){
      if ( (i>=0) && (i<Le) && (j>=0) && (j<Wi) ){
	
	data[i*Wi+j] = pixelR
	  + (pixelG << 8)
	  + (pixelB << 16)
	  + (pixelA << 24);   
      }
    }
}

void cercle(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  int i;
  double alpha;
  assert(num>0);

  for(i=0;i<100;i++){
    alpha=2.0*i*PI_MATH/100.0;
    //if (num==2) printf("(%d,%d)\n",x+(int) (R*cos(alpha)), y+(int) (R*sin(alpha)));
    point(data, x+(int) (R*cos(alpha)), y+(int) (R*sin(alpha)), T, Wi, Le, num);
  }
}

void carre(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  ligne(num, data, y-R, x-R, y+R, x-R, T, Wi, Le);
  ligne(num, data, y+R, x-R, y+R, x+R, T, Wi, Le);
  ligne(num, data, y+R, x+R, y-R, x+R, T, Wi, Le);
  ligne(num, data, y-R, x+R, y-R, x-R, T, Wi, Le);
}

void croix(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  ligne(num, data, y-R, x-R, y+R, x+R, T, Wi, Le);
  ligne(num, data, y+R, x-R, y-R, x+R, T, Wi, Le);
}

void plus(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  ligne(num, data, y-R, x, y+R, x, T, Wi, Le);
  ligne(num, data, y, x-R, y, x+R, T, Wi, Le);
}

void trianglesup(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  int ax, ay, bx, by, cx, cy;
  ay=y-R;
  ax=x;
  by=y+(int) ( (double) R / 2);
  bx=x+(int) ( (double) R * sqrt(3) / 2);
  cy=y+(int) ( (double) R / 2);
  cx=x-(int) ( (double) R * sqrt(3) / 2);

  ligne(num, data, ay, ax, by, bx, T, Wi, Le);
  ligne(num, data, by, bx, cy, cx, T, Wi, Le);
  ligne(num, data, cy, cx, ay, ax, T, Wi, Le);
}

void triangleinf(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  int ax, ay, bx, by, cx, cy;
  ay=y+R;
  ax=x;
  by=y-(int) ( (double) R / 2);
  bx=x+(int) ( (double) R * sqrt(3) / 2);
  cy=y-(int) ( (double) R / 2);
  cx=x-(int) ( (double) R * sqrt(3) / 2);

  ligne(num, data, ay, ax, by, bx, T, Wi, Le);
  ligne(num, data, by, bx, cy, cx, T, Wi, Le);
  ligne(num, data, cy, cx, ay, ax, T, Wi, Le);
}

void losange(int num, unsigned int* data, int y, int x, int R, int T, int Wi, int Le){
  int ax, ay, bx, by, cx, cy, dx, dy;
  ay=y+(3*R/2);
  ax=x;
  by=y;
  bx=x+(int) ( (double) R * sqrt(3) / 2);
  cy=y-(3*R/2);
  cx=x;
  dy=y;
  dx=x-(int) ( (double) R * sqrt(3) / 2);

  ligne(num, data, ay, ax, by, bx, T, Wi, Le);
  ligne(num, data, by, bx, cy, cx, T, Wi, Le);
  ligne(num, data, cy, cx, dy, dx, T, Wi, Le);
  ligne(num, data, dy, dx, ay, ax, T, Wi, Le);
}

void ligne(int num, unsigned int* data, int y0, int x0, int y1, int x1, int T, int Wi, int Le){
  int dy = y1 - y0;
  int dx = x1 - x0;
  int stepx, stepy;
  
  if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
  if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
  dy <<= 1;                                                  
  dx <<= 1;                                                  
  
  point(data, x0, y0, T, Wi, Le, num);
  if (dx > dy) {
    int fraction = dy - (dx >> 1);                         
    while (x0 != x1) {
      if (fraction >= 0) {
	y0 += stepy;
	fraction -= dx;                                
      }
      x0 += stepx;
      fraction += dy;                                    
      point(data, x0, y0, T, Wi, Le, num);
    }
  } else {
    int fraction = dx - (dy >> 1);
    while (y0 != y1) {
      if (fraction >= 0) {
	x0 += stepx;
	fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      point(data, x0, y0, T, Wi, Le, num);
    }
  } 
}

void putModels(regionC** &bm, unsigned int* data, int Wi, int Le){
  int i,j,k,nm,rI,rJ;
  //printf("Putmodels\n");fflush(stdout);
  //rappel: nummod[(Le-1-cx)*Wi+cy]=k+1;

  //char_image(bm,Wi,Le);

  int* picked= (int*) (malloc(sizeof(int)*NModels));
  for (i=0;i<NModels;i++) picked[i]=0;

  for (i = 0; i < Le; i++){
    for (j = 0; j < Wi; j++){   
      nm=bm[i*Wi+j]->meta->nom;
      if ( ( nm > 0 ) && (picked[nm-1]==0) ){
	picked[nm-1]=1;
	for (k=0;k<bm[i*Wi+j]->meta->nMod;k++){
	  rI=bm[i*Wi+j]->meta->tabI[k];
	  rJ=bm[i*Wi+j]->meta->tabJ[k];
	  //printf("(%d,%d)\n",bm[i*Wi+j]->meta->tabI[k], bm[i*Wi+j]->meta->tabJ[k]);
	  //cercle(nm, data, bm[i*Wi+j]->meta->tabI[k], bm[i*Wi+j]->meta->tabJ[k], RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  if ((nm-1)%7==0) losange(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  else if ((nm-1)%7==1) croix(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  else if ((nm-1)%7==2) trianglesup(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  else if ((nm-1)%7==3) carre(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  else if ((nm-1)%7==4) cercle(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  else if ((nm-1)%7==5) plus(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	  else triangleinf(nm, data, rI, rJ, RAYON_CERCLE, TAILLE_POINT, Wi, Le);
	}
      }
    }
  } 
}

void sauveRegion(int numero, int numeroR, regionC** &bm, int Wi, int Le) 
{
  char filename[255];
  strcpy(filename,"fileRegion");
  if (numero<=NB_MAX_REGION) strcat(filename,itoa(numero));
  else strcat(filename,"XX");
  strcat(filename,".tif"); 

  unsigned int *data;
  data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
  regionC* c; 
  int pixelcolorR,pixelcolorG,pixelcolorB,pixelcolorAlpha;
  int white = 255; 
  int black = 0;
  
  //printf ("avant savetiffgl \n");
  
  for (int i = 0; i < Le; i++) 
    {	for (int j = 0; j < Wi; j++) 
      {   
	c = FindCompress(bm[i*Wi+j]); 
	
	if (c->no == numeroR)
	  {
	    pixelcolorR = bm[i*Wi+j]->valoriRSauv;
	    pixelcolorG = bm[i*Wi+j]->valoriGSauv;
	    pixelcolorB = bm[i*Wi+j]->valoriBSauv;
	    pixelcolorAlpha = bm[i*Wi+j]->valoriAlphaSauv;
	  }
	else
	  {
	    pixelcolorR = white;
	    pixelcolorG = white;
	    pixelcolorB = white;
	    pixelcolorAlpha = bm[i*Wi+j]->valoriAlphaSauv;
	  }
	
	data[i*Wi+j] = pixelcolorR
	  + (pixelcolorG << 8)
	  + (pixelcolorB << 16)
	  + (pixelcolorAlpha << 24);   
	
	if ( (i==0) || (i==Le-1) || (j==0) || (j==Wi-1) )
	  {
	    data[i*Wi+j] = black
	      + (black << 8)
	      + (black << 16)
	      + ((bm[i*Wi+j]->valoriAlphaSauv) << 24);
	  }
	
      }
    }
  
  //on renverse l'image
  int tmp;
  for (int i = 0; i < (Le/2); i++)
    {
      for (int j = 0; j < Wi; j++) 
	{
	  tmp=data[(Le-1-i)*Wi+j];
	  data[(Le-1-i)*Wi+j]=data[i*Wi+j];
	  data[i*Wi+j]=tmp;
	}
    }
  
  if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0)
    {	printf ("erreur readtiffgl"); }
  
  //printf ("apres savetiffgl\n");
  
  free(data);
} 

void sauveModel(int numero, int numeroR, regionC** &bm, int Wi, int Le) 
{
  char filename[255];
  strcpy(filename,"fileModel");
  if (numero<=20) strcat(filename,itoa(numero));
  else strcat(filename,"XX");
  strcat(filename,".tif");

  unsigned int *data;
  data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
  metaRegion* c; 
  int pixelcolorR,pixelcolorG,pixelcolorB,pixelcolorAlpha;
  int white = 255; 
  int black = 0;
  
  //printf ("avant savetiffgl \n");
  
  for (int i = 0; i < Le; i++) 
    {	for (int j = 0; j < Wi; j++) 
      {   
	c = topModel(bm[i*Wi+j]); 
	
	if (c->nom == numeroR)
	  {
	    pixelcolorR = bm[i*Wi+j]->valoriRSauv;
	    pixelcolorG = bm[i*Wi+j]->valoriGSauv;
	    pixelcolorB = bm[i*Wi+j]->valoriBSauv;
	    pixelcolorAlpha = bm[i*Wi+j]->valoriAlphaSauv;
	  }
	else
	  {
	    pixelcolorR = white;
	    pixelcolorG = white;
	    pixelcolorB = white;
	    pixelcolorAlpha = bm[i*Wi+j]->valoriAlphaSauv;
	  }
	
	data[i*Wi+j] = pixelcolorR
	  + (pixelcolorG << 8)
	  + (pixelcolorB << 16)
	  + (pixelcolorAlpha << 24);   
	
	if ( (i==0) || (i==Le-1) || (j==0) || (j==Wi-1) )
	  {
	    data[i*Wi+j] = black
	      + (black << 8)
	      + (black << 16)
	      + ((bm[i*Wi+j]->valoriAlphaSauv) << 24);
	  }
	
      }
    }
  
  //on renverse l'image
  int tmp;
  for (int i = 0; i < (Le/2); i++)
    {
      for (int j = 0; j < Wi; j++) 
	{
	  tmp=data[(Le-1-i)*Wi+j];
	  data[(Le-1-i)*Wi+j]=data[i*Wi+j];
	  data[i*Wi+j]=tmp;
	}
    }
  
  if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0)
    {	printf ("erreur readtiffgl"); }
  
  //printf ("apres savetiffgl\n");
  
  free(data);
} 


void sauveRGBimage(const char *filename, regionC** &bm, int Wi, int Le) 
{
   unsigned int *data;
   data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
   regionC* c, *l, *u;; 
   int pixelcolorR, pixelcolorG, pixelcolorB, pixelcolorAlpha, line = 0;
   int white = 255; 
   int black = 0;

   int curblack, curwhite;

   if ( (Sortie==0) || (Sortie==1) || (Sortie==2) || (Sortie==3) )
     {
       for (int i = 0; i < Le; i++) 
	 {	
	   for (int j = 0; j < Wi; j++) 
	     {   
	       c = FindCompress(bm[i*Wi+j]); 
	       
	       pixelcolorR = (int) (c->somR/c->nb);
	       pixelcolorG = (int) (c->somG/c->nb);
	       pixelcolorB = (int) (c->somB/c->nb);
	       pixelcolorAlpha = (int) (c->somAlpha/c->nb);


	       if (Sortie==0){
		 pixelcolorR = bm[i*Wi+j]->valoriR;
		 pixelcolorG = bm[i*Wi+j]->valoriG;
		 pixelcolorB = bm[i*Wi+j]->valoriB;
		 pixelcolorAlpha = bm[i*Wi+j]->valoriAlpha;
	       }
	       else if (Sortie==3){
		 pixelcolorR = c->randR;
		 pixelcolorG = c->randG;
		 pixelcolorB = c->randB;
		 pixelcolorAlpha = bm[i*Wi+j]->valoriAlpha;
	       }

	       data[i*Wi+j] = pixelcolorR
		 + (pixelcolorG << 8)
		 + (pixelcolorB << 16)
		 + (pixelcolorAlpha << 24);
	       
	       curblack = black
		 + (black << 8)
		 + (black << 16)
		 + (bm[i*Wi+j]->valoriAlpha << 24);
	       
	       curwhite = white
		 + (white << 8)
		 + (white << 16)
		 + (bm[i*Wi+j]->valoriAlpha << 24);

	       if (Sortie != 3)
		 {
		   if( i == 0 )  // first line.
		     {   
		       if( line-- )
			 {   
			   l = FindCompress(bm[i*Wi+j-1]); 
			   if( checkpixel(c, l, data[i*Wi+j-1]) ) contour(data, i, j, Wi, Le, curblack, curwhite);
			   else if (Sortie==1)
			     {  
			       data[i*Wi+j] = curwhite; 
			     }
			 }
		       else
			 {   
			   line = Wi;
			   if (Sortie==1) data[i*Wi+j] = curwhite; }
		     }
		   else
		     {  
		       u = FindCompress(bm[(i-1)*Wi+j]);
		       if( line-- )
			 {   
			   l = FindCompress(bm[i*Wi+j-1]); 
			   if( checkpixel(c, l, data[i*Wi+j-1]) ||
			       checkpixel(c, u, data[(i-1)*Wi+j]) )
			     contour(data, i, j, Wi, Le, curblack, curwhite);
			   //data[i*Wi+j] = (Sortie==2) ? curwhite : curblack;
			   else if (Sortie==1)
			     {   
			       data[i*Wi+j] = curwhite; }
			 }
		       else
			 {  
			   line = Wi;
			   if( checkpixel(c, u, data[(i-1)*Wi+j])) contour(data, i, j, Wi, Le, curblack, curwhite);
			   else if (Sortie==1)
			     { 
			       data[i*Wi+j] = curwhite; }
			 }
		     }
		 }
	       if ( (i==0) || (i==Le-1) || (j==0) || (j==Wi-1) )
		 {
		   contour(data, i, j, Wi, Le, curblack, curwhite);
		 }
	     }
	 }
     }
   
   //on renverse l'image
   int tmp;
   for (int i = 0; i < (Le/2); i++)
     {
       for (int j = 0; j < Wi; j++) 
	 {
	   tmp=data[(Le-1-i)*Wi+j];
	   data[(Le-1-i)*Wi+j]=data[i*Wi+j];
	   data[i*Wi+j]=tmp;
	 }
     }

   if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0)
	{	printf ("erreur readtiffgl"); }

   free(data);
}

void sauveRGBimageModels(int* &nummod, const char *filename, regionC** &bm, int Wi, int Le) 
{
   unsigned int *data;
   data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
   metaRegion* c, *l, *u;; 
   int pixelcolorR, pixelcolorG, pixelcolorB, pixelcolorAlpha, line = 0;
   int white = 255; 
   int black = 0;

   int curblack, curwhite;

   if ( (Sortie==0) || (Sortie==1) || (Sortie==2) || (Sortie==3) )
     {
       for (int i = 0; i < Le; i++) 
	 {	
	   for (int j = 0; j < Wi; j++) 
	     {   
	       c = MetaFindCompress(bm[i*Wi+j]); 
	       
	       pixelcolorR = (int) (c->somR/c->nb);
	       pixelcolorG = (int) (c->somG/c->nb);
	       pixelcolorB = (int) (c->somB/c->nb);
	       pixelcolorAlpha = (int) (c->somAlpha/c->nb);


	       if (Sortie==0){
		 pixelcolorR = bm[i*Wi+j]->valoriR;
		 pixelcolorG = bm[i*Wi+j]->valoriG;
		 pixelcolorB = bm[i*Wi+j]->valoriB;
		 pixelcolorAlpha = bm[i*Wi+j]->valoriAlpha;
	       }
	       else if (Sortie==3){
		 pixelcolorR = (c->tabMod[0])->randR;
		 pixelcolorG = (c->tabMod[0])->randG;
		 pixelcolorB = (c->tabMod[0])->randB;
		 pixelcolorAlpha = bm[i*Wi+j]->valoriAlpha;
	       }

	       data[i*Wi+j] = pixelcolorR
		 + (pixelcolorG << 8)
		 + (pixelcolorB << 16)
		 + (pixelcolorAlpha << 24);
	       
	       curblack = black
		 + (black << 8)
		 + (black << 16)
		 + (bm[i*Wi+j]->valoriAlpha << 24);
	       
	       curwhite = white
		 + (white << 8)
		 + (white << 16)
		 + (bm[i*Wi+j]->valoriAlpha << 24);

	       if (Sortie != 3)
		 {
		   if( i == 0 )  // first line.
		     {   
		       if( line-- )
			 {   
			   l = MetaFindCompress(bm[i*Wi+j-1]); 
			   if( metacheckpixel(c, l, data[i*Wi+j-1]) ) contour(data, i, j, Wi, Le, curblack, curwhite);
			   else if (Sortie==1)
			     {  
			       data[i*Wi+j] = curwhite; 
			     }
			 }
		       else
			 {   
			   line = Wi;
			   if (Sortie==1) data[i*Wi+j] = curwhite; }
		     }
		   else
		     {  
		       u = MetaFindCompress(bm[(i-1)*Wi+j]);
		       if( line-- )
			 {   
			   l = MetaFindCompress(bm[i*Wi+j-1]); 
			   if( metacheckpixel(c, l, data[i*Wi+j-1]) ||
			       metacheckpixel(c, u, data[(i-1)*Wi+j]) )
			     contour(data, i, j, Wi, Le, curblack, curwhite);
			   //data[i*Wi+j] = (Sortie==2) ? curwhite : curblack;
			   else if (Sortie==1)
			     {   
			       data[i*Wi+j] = curwhite; }
			 }
		       else
			 {  
			   line = Wi;
			   if( metacheckpixel(c, u, data[(i-1)*Wi+j])) contour(data, i, j, Wi, Le, curblack, curwhite);
			   else if (Sortie==1)
			     { 
			       data[i*Wi+j] = curwhite; }
			 }
		     }
		 }
	       if ( (i==0) || (i==Le-1) || (j==0) || (j==Wi-1) )
		 {
		   contour(data, i, j, Wi, Le, curblack, curwhite);
		 }
	     }
	 }
     }
   
   //char_data_image(data,Wi,Le);
   //char_models(nummod,bm,Wi,Le);
   if (ShowModels==1) putModels(bm, data, Wi, Le);

   //on renverse l'image
   int tmp;
   for (int i = 0; i < (Le/2); i++)
     {
       for (int j = 0; j < Wi; j++) 
	 {
	   tmp=data[(Le-1-i)*Wi+j];
	   data[(Le-1-i)*Wi+j]=data[i*Wi+j];
	   data[i*Wi+j]=tmp;
	 }
     }

   if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0)
	{	printf ("erreur readtiffgl"); }

   free(data);
}


void contour(unsigned int* data, int i, int j, int Wi, int Le, int curblack, int curwhite)
{
  int k,l;
  //Taille de la frontiere=taille*2+1
  int taille=2;
  int mini=((i-taille)>0) ? i-taille : 0 ;
  int maxi=((i+taille)<Le) ? i+taille : Le-1 ;
  int minj=((j-taille)>0) ? j-taille : 0 ;
  int maxj=((j+taille)<Wi) ? j+taille : Wi-1 ;

  for (k=mini; k<=maxi;k++)
    for (l=minj;l<=maxj;l++)
      data[k*Wi+l] = ( (Sortie==0) || (Sortie==2) ) ? ( (BorderColor==0) ? curblack : curwhite ) : curblack ;
}

void sauveRGBimageNoisy(const char *filename, regionC** &bm, int Wi, int Le) 
{
   unsigned int *data;
   data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
   regionC* c; 
   int pixelcolorR, pixelcolorG, pixelcolorB, pixelcolorAlpha;

   for (int i = 0; i < Le; i++) 
     {	
       for (int j = 0; j < Wi; j++) 
	 {   
	   c = FindCompress(bm[i*Wi+j]); 
	 
	   pixelcolorR = bm[i*Wi+j]->valoriR;
	   pixelcolorG = bm[i*Wi+j]->valoriG;
	   pixelcolorB = bm[i*Wi+j]->valoriB;
	   pixelcolorAlpha = bm[i*Wi+j]->valoriAlpha;
	   
	   data[i*Wi+j] = pixelcolorR
	     + (pixelcolorG << 8)
	     + (pixelcolorB << 16)
	     + (pixelcolorAlpha << 24);
	 }
     }

   //on renverse l'image
   int tmp;
   for (int i = 0; i < (Le/2); i++)
     {
       for (int j = 0; j < Wi; j++) 
	 {
	   tmp=data[(Le-1-i)*Wi+j];
	   data[(Le-1-i)*Wi+j]=data[i*Wi+j];
	   data[i*Wi+j]=tmp;
	 }
     }

   if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0)
	{	printf ("erreur readtiffgl"); }

   free(data);
}

int ecartSquare(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2, int delta)
{

  /*****
   * Voisinage (pixels horiZontaux)
   *
   *      <delta>
   * ...........
   * ...........
   * ...........
   * ...........
   * .....O.....
   * -----------
   * .....O.....
   * ...........
   * ...........
   * ...........
   * ...........
   *****/

  int valaret=0;
  int i,j;
  int nouval1R,nouval2R,nouval1G,nouval2G,nouval1B,nouval2B;
  int numb;

  //methodes modifs pour prendre en compte le bruit en 4-connexite
  if (j2>j1)
    {
      assert(i1==i2);
      //ecart horizontal
      numb=0;
      nouval1R=0;
      nouval1G=0;
      nouval1B=0;
      for (i=max( (0), (i1-delta) ); i<min( (Le) , (i1+delta) );i++)
	{
	  for (j=max( (0), (j1-delta) ); j<=j1; j++)
	    {
	      numb++;
	      nouval1R+=bm[i*Wi+j]->valoriR;
	      nouval1G+=bm[i*Wi+j]->valoriG;
	      nouval1B+=bm[i*Wi+j]->valoriB;
	    }
	}
      nouval1R/=numb;
      nouval1G/=numb;
      nouval1B/=numb;

      numb=0;
      nouval2R=0;
      nouval2G=0;
      nouval2B=0;

      for (i=max( (0), (i1-delta) ); i<min( (Le) , (i1+delta) );i++)
	{
	  for (j=j2; j<min ( (Wi) , (j2+delta) ); j++)
	    {
	      numb++;
	      nouval2R+=bm[i*Wi+j]->valoriR;
	      nouval2G+=bm[i*Wi+j]->valoriG;
	      nouval2B+=bm[i*Wi+j]->valoriB;
	    }
	}
      nouval2R/=numb;
      nouval2G/=numb;
      nouval2B/=numb;

      valaret=max( (abs( (nouval1R-nouval2R) )), max( ( abs( (nouval1G-nouval2G) ) ), ( abs( (nouval1B-nouval2B) ) ) ) );
	//(abs( (nouval1R-nouval2R) )+abs( (nouval1G-nouval2G) )+abs( (nouval1B-nouval2B) ))/3;
	//max( (abs( (nouval1R-nouval2R) )), max( ( abs( (nouval1G-nouval2G) ) ), ( abs( (nouval1B-nouval2B) ) ) ) );
    }
  else if (i2>i1)
    {

      assert(j1==j2);
      //ecart horizontal
      numb=0;
      nouval1R=0;
      nouval1G=0;
      nouval1B=0;
      for (i=max( (0), (i1-delta) ); i<=i1;i++)
	{
	  for (j=max( (0), (j1-delta) ); j<min ( (Wi) , (j1+delta) ); j++)
	    {
	      numb++;
	      nouval1R+=bm[i*Wi+j]->valoriR;
	      nouval1G+=bm[i*Wi+j]->valoriG;
	      nouval1B+=bm[i*Wi+j]->valoriB;
	    }
	}

      nouval1R/=numb;
      nouval1G/=numb;
      nouval1B/=numb;

      numb=0;
      nouval2R=0;
      nouval2G=0;
      nouval2B=0;

      for (i=i2; i<min( (Le) , (i2+delta) );i++)
	{
	  for (j=max( (0), (j1-delta) ); j<min ( (Wi) , (j1+delta) ); j++)
	    {
	      numb++;
	      nouval2R+=bm[i*Wi+j]->valoriR;
	      nouval2G+=bm[i*Wi+j]->valoriG;
	      nouval2B+=bm[i*Wi+j]->valoriB;
	    }
	}

      nouval2R/=numb;
      nouval2G/=numb;
      nouval2B/=numb;

      valaret=max( (abs( (nouval1R-nouval2R) )), max( ( abs( (nouval1G-nouval2G) ) ), ( abs( (nouval1B-nouval2B) ) ) ) );
	//(abs( (nouval1R-nouval2R) )+abs( (nouval1G-nouval2G) )+abs( (nouval1B-nouval2B) ))/3;
	//max( (abs( (nouval1R-nouval2R) )), max( ( abs( (nouval1G-nouval2G) ) ), ( abs( (nouval1B-nouval2B) ) ) ) );

      //valaret=ecartOriginal(bm,Wi,Le,i1,j1,i2,j2);
    }
  else
    {
      printf("Erreur !! \n \n Le classement des pixels se fait MAL !!! \n \n");
    }

  return valaret;

}

int ecartOriginal(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2)
{
  //methode originale de BMVC/CVPR-like etc...
  int valaret;
  valaret=max( (abs(bm[i1*Wi+j1]->valoriR-bm[i2*Wi+j2]->valoriR)) 
	       , (max ( (abs(bm[i1*Wi+j1]->valoriG-bm[i2*Wi+j2]->valoriG)) 
			, (abs(bm[i1*Wi+j1]->valoriB-bm[i2*Wi+j2]->valoriB)) )) );
  return valaret;
}

int sobel(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2){
  int valaret, valR, valG, valB;
  if (i1==i2){
    // -1  0  0  1
    // -2  0  0  2
    // -1  0  0  1
    valR=bm[(i1-1)*Wi+j2+1]->valoriR + 
      (2*bm[i1*Wi+j2+1]->valoriR) +
      bm[(i1+1)*Wi+j2+1]->valoriR -
      bm[(i1-1)*Wi+j1-1]->valoriR -
      (2*bm[i1*Wi+j1-1]->valoriR) -
      bm[(i1+1)*Wi+j1-1]->valoriR;

    valG=bm[(i1-1)*Wi+j2+1]->valoriG + 
      (2*bm[i1*Wi+j2+1]->valoriG) +
      bm[(i1+1)*Wi+j2+1]->valoriG -
      bm[(i1-1)*Wi+j1-1]->valoriG -
      (2*bm[i1*Wi+j1-1]->valoriG) -
      bm[(i1+1)*Wi+j1-1]->valoriG;

    valB=bm[(i1-1)*Wi+j2+1]->valoriB + 
      (2*bm[i1*Wi+j2+1]->valoriB) +
      bm[(i1+1)*Wi+j2+1]->valoriB -
      bm[(i1-1)*Wi+j1-1]->valoriB -
      (2*bm[i1*Wi+j1-1]->valoriB) -
      bm[(i1+1)*Wi+j1-1]->valoriB;

    valR/=4;
    valG/=4;
    valB/=4;
  }else{
    assert(j1==j2);
    // -1 -2 -1
    //  0  0  0
    //  0  0  0
    // +1 +2 +1

    valR=bm[(i2+1)*Wi+j1+1]->valoriR + 
      (2*bm[(i2+1)*Wi+j1]->valoriR) +
      bm[(i2+1)*Wi+j1-1]->valoriR -
      bm[(i1-1)*Wi+j1+1]->valoriR -
      (2*bm[(i1-1)*Wi+j1]->valoriR) -
      bm[(i1-1)*Wi+j1-1]->valoriR;

    valG=bm[(i2+1)*Wi+j1+1]->valoriG + 
      (2*bm[(i2+1)*Wi+j1]->valoriG) +
      bm[(i2+1)*Wi+j1-1]->valoriG -
      bm[(i1-1)*Wi+j1+1]->valoriG -
      (2*bm[(i1-1)*Wi+j1]->valoriG) -
      bm[(i1-1)*Wi+j1-1]->valoriG;

    valB=bm[(i2+1)*Wi+j1+1]->valoriB + 
      (2*bm[(i2+1)*Wi+j1]->valoriB) +
      bm[(i2+1)*Wi+j1-1]->valoriB -
      bm[(i1-1)*Wi+j1+1]->valoriB -
      (2*bm[(i1-1)*Wi+j1]->valoriB) -
      bm[(i1-1)*Wi+j1-1]->valoriB;

    valR/=4;
    valG/=4;
    valB/=4;
  }

  valR=abs(valR);
  valG=abs(valG);
  valB=abs(valB);

  valaret=max ( (valR) , max( (valG) , (valB) ) );
  return valaret;
}

int ecart(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2)
{
  int valaret=0;

  if (SOBEL_GRADIENT){
    if ( (i1==0) || (i1==Le-1) || 
	 (j1==0) || (j1==Wi-1) ||
	 (i2==0) || (i2==Le-1) ||
	 (j2==0) || (j2==Wi-1) ) valaret=ecartOriginal(bm,Wi,Le,i1,j1,i2,j2);
    else valaret=sobel(bm,Wi,Le,i1,j1,i2,j2);
  }else if (TypeOfNeighbor==0){
    valaret=ecartOriginal(bm,Wi,Le,i1,j1,i2,j2);
  }else if (TypeOfNeighbor==1){
    valaret=ecartSquare(bm,Wi,Le,i1,j1,i2,j2,10);
  }

  return valaret;
}

metaRegion* topModel(regionC* x){
  if (isTop(x)) return (x->meta);
  else return (topModel(x->parent));
}

int noModel(regionC* x){
  return (topModel(x)->nom<0);
}

void eliminer_petites_regions(regionC** &bm, int Wi, int Le)
{
  regionC *predreg, *smallreg, *rt;

  int tmax=10;
  int remov=0;

  if ( (Wi>128) && (Le>128) )
    {
      tmax*=(Wi/128);
    }

  for (int i = 0; i < Le; i++){ // i -> y cad un numero de ligne

    for (int j = 1; j < Wi; j++){ 
      smallreg = FindCompress(bm[i*Wi+j]);
      predreg = FindCompress(bm[i*Wi+j-1]);
      if (smallreg != predreg) {
        if ( (smallreg->nb <= tmax) && ( noModel(smallreg) || noModel(predreg) ) ) {
	  remov++;
	  if ( (NModels>0) && ( (noModel(smallreg)) && (!noModel(predreg)) ) ){
	    rt=smallreg;
	    smallreg=predreg;
	    predreg=rt;
	  }

          Union(smallreg, predreg);
        }
      }
    }
  }
  
}

double maxdiff(regionC* x, regionC* y){
  double mxR=((double) x->somR)/((double) x->nb);
  double myR=((double) y->somR)/((double) y->nb);

  double mxG=((double) x->somG)/((double) x->nb);
  double myG=((double) y->somG)/((double) y->nb);

  double mxB=((double) x->somB)/((double) x->nb);
  double myB=((double) y->somB)/((double) y->nb);


  double dmR=(mxR-myR)*(mxR-myR);
  double dmG=(mxG-myG)*(mxG-myG);
  double dmB=(mxB-myB)*(mxB-myB);
  
  if ( (dmR>=dmG) && (dmR>=dmB) ){
    return dmR;
  }
  else if (dmG>=dmB) return dmG;
  else return dmB;
}

/*****
 * int OracleProbabiliste(int Wi, int Le, regionC* x, regionC* y)
 * oracle randomise, marchant avec n'importe quelle distance
 * On prend en compte un nombre poly de regions, et un Q plus petit
 * Et c'est la borne la plus simple qu'on utilise.
 *****/
int OracleProbabiliste(int Wi, int Le, regionC* x, regionC* y) 
{
  double Q=32;
  double g=256;

  double dnx=((double) x->nb);
  double dny=((double) y->nb);
  double sizeI=(double) (Wi*Le);

  double mxR=((double) x->somR)/dnx;
  double myR=((double) y->somR)/dny;
  double dmR=(mxR-myR)*(mxR-myR);

  double mxG=((double) x->somG)/dnx;
  double myG=((double) y->somG)/dny;
  double dmG=(mxG-myG)*(mxG-myG);

  double mxB=((double) x->somB)/dnx;
  double myB=((double) y->somB)/dny;
  double dmB=(mxB-myB)*(mxB-myB);

  double logcardX = min( (g) , (dnx) ) * log ( dnx + (double) 1 );
  double logcardY = min( (g) , (dny) ) * log ( dny + (double) 1 );

  double logdelta = log (sizeI*sizeI*((double) 6));

  double devx=((g*g)/(2*Q*dnx))*(logcardX + logdelta);
  double devy=((g*g)/(2*Q*dny))*(logcardY + logdelta);
  double dev=devx+devy;

  return ( (dmR<dev) && (dmG<dev) && (dmB<dev) );


}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void quicksortcroisslinkage(int** refTab, int l, int r){
  //attention, on ne fait pas le sorting exact (premier while ne se fait jamais)

  int i, j, t;
  int *v;

  if (r>l)
    {
      v=refTab[r]; i=l-1; j=r;

      for (;;)
	{
	  while ( (refTab[++i][4]<v[4]) && (i<r) );
	  while ( (refTab[--j][4]>v[4]) && (j>l) );
	  if (i>=j) break;
	  t=refTab[i][0];
	  refTab[i][0]=refTab[j][0];
	  refTab[j][0]=t;

	  t=refTab[i][1];
	  refTab[i][1]=refTab[j][1];
	  refTab[j][1]=t;

	  t=refTab[i][2];
	  refTab[i][2]=refTab[j][2];
	  refTab[j][2]=t;

	  t=refTab[i][3];
	  refTab[i][3]=refTab[j][3];
	  refTab[j][3]=t;

	  t=refTab[i][4];
	  refTab[i][4]=refTab[j][4];
	  refTab[j][4]=t;
	}

      t=refTab[i][0];
      refTab[i][0]=refTab[r][0];
      refTab[r][0]=t;
      
      t=refTab[i][1];
      refTab[i][1]=refTab[r][1];
      refTab[r][1]=t;
      
      t=refTab[i][2];
      refTab[i][2]=refTab[r][2];
      refTab[r][2]=t;
      
      t=refTab[i][3];
      refTab[i][3]=refTab[r][3];
      refTab[r][3]=t;
      
      t=refTab[i][4];
      refTab[i][4]=refTab[r][4];
      refTab[r][4]=t;
      
      quicksortcroisslinkage(refTab,l,i-1);
      quicksortcroisslinkage(refTab,i+1,r);
    }
}

/*****
 * int** sortedlinkage(region** &bm, int Wi, int Le)
 *
 * Methode de sorting, mais ce sont les tests qui sont ordonnes en ordre croissant
 * des differences de niveaux de gris.
 *
 *****/
int** sortedlinkage(region** &bm, int Wi, int Le){

  int timeDep,timeEnd;
  timeDep=time(NULL);

  int** refRet;

  refRet=new int* [((Wi-1)*Le + Wi*(Le-1))];
  for (int i = 0; i <((Wi-1)*Le + Wi*(Le-1)) ; i++) 
    {
      refRet[i]=new int [5];
      refRet[i][0]=-1;
      refRet[i][1]=-1;
      refRet[i][2]=-1;
      refRet[i][3]=-1;
      refRet[i][4]=-1;      
    }

  int dep2=(Wi-1)*Le;

 for (int i = 0; i < Le; i++) 
   {
     for (int j = 0; j < Wi-1; j++) 
       {
	 refRet[i*(Wi-1)+j][0]=i;
	 refRet[i*(Wi-1)+j][1]=j;
	 refRet[i*(Wi-1)+j][2]=i;
	 refRet[i*(Wi-1)+j][3]=j+1;
	 refRet[i*(Wi-1)+j][4]=ecart(bm,Wi,Le,i,j,i,j+1);
       }
   }

 for (int i = 0; i < Le-1; i++) 
   {
     for (int j = 0; j < Wi; j++) 
       {
	 refRet[dep2+i*Wi+j][0]=i;
	 refRet[dep2+i*Wi+j][1]=j;
	 refRet[dep2+i*Wi+j][2]=i+1;
	 refRet[dep2+i*Wi+j][3]=j;
	 refRet[dep2+i*Wi+j][4]=ecart(bm,Wi,Le,i,j,i+1,j);
       }
   }

 quicksortcroisslinkage(refRet,0,((Wi-1)*Le + Wi*(Le-1))-1);

  timeEnd=time(NULL);
  //printf("\n->Time Elapsed for Sorting the Image: %d s\n",timeEnd-timeDep);

  return refRet;
}

void sauveParts(region** &bm, int Wi, int Le){
  int numberInitial=renumerote(bm, Wi, Le);

  //initialisation des parametres

  int**  refRet=new int* [numberInitial];
  for (int i = 0; i <numberInitial ; i++) 
    {
      refRet[i]=new int [5];
      refRet[i][0]=-1;
      refRet[i][1]=-1;
      refRet[i][2]=-1;
      refRet[i][3]=-1;
      refRet[i][4]=-1;      
    }

  //recherche des regions

  int regcour=0;
  int trouve=0;
  int coordI, coordJ;
  regionC* c;

  for (int i=0; i<numberInitial; i++)
    {
      trouve=0;
      coordI=0;
      coordJ=0;
      do
	{
	  c = FindCompress(bm[coordI*Wi+coordJ]);
	  if (c->no == regcour)
	    {
	      trouve=1;
	      refRet[regcour][0]=coordI;
	      refRet[regcour][1]=coordJ;
	      refRet[regcour][2]=regcour;

	      refRet[regcour][4]=c->nb;
	      regcour++;
	    }
	  coordJ++;
	  if ( coordJ==Wi )
	    {
	      coordJ=0;
	      coordI++;
	    }
	}while( (trouve==0) && (coordI<Le) );
      assert(trouve==1);
    }

  // on ordonne

  quicksortcroisslinkage(refRet,0,numberInitial-1);

  //on intervertit la liste

  int temp;
  for (int i=0; i<(numberInitial / 2); i++)
    {
      temp=refRet[i][0];
      refRet[i][0] = refRet[numberInitial-1-i][0];
      refRet[numberInitial-1-i][0]=temp;

      temp=refRet[i][1];
      refRet[i][1] = refRet[numberInitial-1-i][1];
      refRet[numberInitial-1-i][1]=temp;

      temp=refRet[i][2];
      refRet[i][2] = refRet[numberInitial-1-i][2];
      refRet[numberInitial-1-i][2]=temp;

      temp=refRet[i][3];
      refRet[i][3] = refRet[numberInitial-1-i][3];
      refRet[numberInitial-1-i][3]=temp;

      temp=refRet[i][4];
      refRet[i][4] = refRet[numberInitial-1-i][4];
      refRet[numberInitial-1-i][4]=temp;
    }

  //on sauvegarde

  int nbc=(numberInitial<15) ? numberInitial : 15;
  for (int i=0; i<nbc; i++)
    {
      sauveRegion(i, refRet[i][2], bm, Wi, Le); 
    }

}

void sauvePartsModels(region** &bm, int Wi, int Le){
  int numberInitial=renumeroteModels(bm, Wi, Le);

  //initialisation des parametres

  int**  refRet=new int* [numberInitial];
  for (int i = 0; i <numberInitial ; i++) 
    {
      refRet[i]=new int [5];
      refRet[i][0]=-1;
      refRet[i][1]=-1;
      refRet[i][2]=-1;
      refRet[i][3]=-1;
      refRet[i][4]=-1;      
    }

  //recherche des regions

  int regcour=0;
  int trouve=0;
  int coordI, coordJ;
  metaRegion* c;

  for (int i=0; i<numberInitial; i++)
    {
      trouve=0;
      coordI=0;
      coordJ=0;
      do
	{
	  c = topModel(bm[coordI*Wi+coordJ]);
	  if (c->nom == regcour)
	    {
	      trouve=1;
	      refRet[regcour][0]=coordI;
	      refRet[regcour][1]=coordJ;
	      refRet[regcour][2]=regcour;

	      refRet[regcour][4]=c->nb;
	      regcour++;
	    }
	  coordJ++;
	  if ( coordJ==Wi )
	    {
	      coordJ=0;
	      coordI++;
	    }
	}while( (trouve==0) && (coordI<Le) );
      assert(trouve==1);
    }

  // on ordonne

  quicksortcroisslinkage(refRet,0,numberInitial-1);

  //on intervertit la liste

  int temp;
  for (int i=0; i<(numberInitial / 2); i++)
    {
      temp=refRet[i][0];
      refRet[i][0] = refRet[numberInitial-1-i][0];
      refRet[numberInitial-1-i][0]=temp;

      temp=refRet[i][1];
      refRet[i][1] = refRet[numberInitial-1-i][1];
      refRet[numberInitial-1-i][1]=temp;

      temp=refRet[i][2];
      refRet[i][2] = refRet[numberInitial-1-i][2];
      refRet[numberInitial-1-i][2]=temp;

      temp=refRet[i][3];
      refRet[i][3] = refRet[numberInitial-1-i][3];
      refRet[numberInitial-1-i][3]=temp;

      temp=refRet[i][4];
      refRet[i][4] = refRet[numberInitial-1-i][4];
      refRet[numberInitial-1-i][4]=temp;
    }

  //on sauvegarde

  int nbc=(numberInitial<NB_MAX_REGION) ? numberInitial : NB_MAX_REGION;
  for (int i=0; i<nbc; i++)
    {
      //printf("saving model %d (numberinitial=%d, nbc=%d)\n",i,numberInitial,nbc);
      sauveModel(i, refRet[i][2], bm, Wi, Le); 
    }
  //printf("End save !\n");
}

int AuthorizedModels(regionC* x, regionC* y){
  return ( (topModel(x)->nom < 0) ||
	   (topModel(y)->nom < 0) );
}

int ForcedMerging(regionC* x, regionC* y){
  assert(topModel(x)->nom >0);
  assert(topModel(y)->nom >0);
  return (topModel(y)->nom == topModel(x)->nom);
}

void PSIS(int* &nummod, region** &bm, int Wi, int Le){

  printf("Probabilistic Sorted Image Segmentation on a %dx%d image...",Wi,Le);
  int OKMerge;
  int i,k,kopt;
  double minmaxdiff;

  int** refSorted;
  refSorted=sortedlinkage(bm, Wi, Le);

  int timeDep,timeEnd;
  timeDep=time(NULL);

  regionC *ra, *rb, *rt; 
  int currentMerges=0;
  int stop=0;

  do{

    currentMerges=0;

    for (i = 1; i < ((Wi-1)*Le + Wi*(Le-1)); i++){ 

      ra= FindCompress(bm[refSorted[i][0]*Wi+refSorted[i][1]]);
      rb= FindCompress(bm[refSorted[i][2]*Wi+refSorted[i][3]]);

      //printf("\t\t\t\t\t ra->meta=%p\t rb->meta=%p\n",ra->meta,rb->meta);
      //printf("\t\t\t\t\t noModel(ra)=%d\t noModel(rb)=%d\n",noModel(ra),noModel(rb));

      OKMerge=0;
      if ( (ra!=rb) && ( (noModel(ra)) || (noModel(rb)) || (topModel(ra)!=topModel(rb)) ) ){
	//printf("%d %d\n",noModel(ra),noModel(rb));

	if (noModel(ra) && noModel(rb)){
	  OKMerge=OracleProbabiliste(Wi, Le, ra, rb);
	  /*if (OKMerge){
	    printf("ra=%p,\t rb=%p\n",ra,rb);
	    printf(" \t ra->meta=%p\t rb->meta=%p\n",ra->meta,rb->meta);
	    }*/
	  if (OKMerge) checkint=1;
	}else if (noModel(ra) || noModel(rb)){
	  if (noModel(ra)){
	    rt=ra;
	    ra=rb;
	    rb=rt;
	  }
	  assert(!noModel(ra));
	  assert(noModel(rb));
	  rt=NULL;
	  kopt=-1;
	  minmaxdiff=0;

	  rt=ra;
	  OKMerge=OracleProbabiliste(Wi, Le, rt, rb);
	  if (!OKMerge){
	    /*
	    //Methode ou on prend le premier modele qui matche
	    for (k=0;k<topModel(ra)->nMod;k++){ 
	    rt=topModel(ra)->tabMod[k];
	    OKMerge=OracleProbabiliste(Wi, Le, rt, rb);
	    if (OKMerge) break;
	    }*/

	    
	    //Methode ou on choisit le modele le plus proche
	    for (k=0;k<topModel(ra)->nMod;k++){
	      rt=topModel(ra)->tabMod[k]; 
	      if ( (k==0) || (maxdiff(rt,rb)<minmaxdiff) ){ 
		kopt=k;
		minmaxdiff=maxdiff(rt,rb);
	      }
	    }
	    rt=topModel(ra)->tabMod[kopt];
	    OKMerge=OracleProbabiliste(Wi, Le, rt, rb);

	  }

	  if (OKMerge){
	    assert(rt!=NULL);
	    ra=rt;
	    //printf("\t\t ra->meta=%p\t rb->meta=%p\n",ra->meta,rb->meta);
	  }
	  if (OKMerge) checkint=3;
	}
      }

      if (OKMerge){ 
	assert(topModel(ra)!=topModel(rb));
	//printf("\t\t\t topModel(ra)->meta=%p\t rb->meta=%p\n",ra->meta,rb->meta);
	Union(ra, rb); 
	mergesMade++;   
	currentMerges++;
      } 
      NIT++;
      
      if ( (MONTRE_TXT) && (NIT%1000==0) ){
	printf("\n NIT=%d:\n",NIT);
	//char_models(nummod,bm,Wi,Le);
	char_models_part(bm, 1, Wi, Le);
      }

      if ( (PICK_MOD) && (i%2000==0) ){
	sauvegardeRect(bm, Wi, Le, LUX, LUY, RDX, RDY, NUMERO_MOD, i/2000);
      }
    }
    stop=1;
 
  }while( stop==0 );
 
  timeEnd=time(NULL);
  printf("done\n");
  printf("\n->Time Elapsed: %d s\n",timeEnd-timeDep);
}

/////////////////////////////////////////Functions proper to Occlusions management

/*
void OcclusionMerge(region** &bm, int Wi, int Le){
  int i,j;
  int currentMerges=0;
  int* vecteurI= (int*) malloc(sizeof(int)*Wi*Le);
  int* vecteurJ= (int*) malloc(sizeof(int)*Wi*Le);
  int index=0;
  regionC *ri, *rj; 

  for (i=0;i<Wi*Le;i++){
    vecteurI[i]=0;
    vecteurJ[i]=0;
  }

  printf("Occlusions merging on the %dx%d image\n\n...",Wi,Le);
  //Flattening
  for (i=0;i<Le;i++) Flatten(i, Wi, bm);
 
  //Calcul des régions à tester
  for (i=0;i<Wi;i++){
    for (j=0;j<Le;j++){
      if (isTop(bm[i*Wi+j])){
	vecteurI[index]=i;
	vecteurJ[index]=j;
	index++;
      }
    }
  }
  printf("Number of candidate regions: %d\n",index);
  fflush(stdout);

  assert(index>=1);
  //Et hop ! Let's go !!
  if (index>1){
    for (i=0;i<index-1;i++){
      if (isTop(bm[vecteurI[i]*Wi+vecteurJ[i]])){
	for (j=i+1;j<index;j++){
	  if (isTop(bm[vecteurI[j]*Wi+vecteurJ[j]])){
	    ri= FindCompress(bm[vecteurI[i]*Wi+vecteurJ[i]]);
	    rj= FindCompress(bm[vecteurI[j]*Wi+vecteurJ[j]]);
	    if (OracleProbabiliste(Wi, Le, ri, rj)){
	      Union(ri,rj);
	      mergesMade++;   
	      currentMerges++;
	    }
	  }
	}
      }
    }
  }

  free(vecteurI);
  free(vecteurJ);
}
*/

void OcclusionMerge(region** &bm, int Wi, int Le){
  printf("Executing region merging between nonadjacent regions\n\n");

  int numberInitial=renumerote(bm, Wi, Le);

  printf("the number of initial regions is %d\n",numberInitial);

  //initialisation des parametres

  int**  refRet=new int* [numberInitial];
  for (int i = 0; i <numberInitial ; i++) 
    {
      refRet[i]=new int [5];
      refRet[i][0]=-1;
      refRet[i][1]=-1;
      refRet[i][2]=-1;
      refRet[i][3]=-1;
      refRet[i][4]=-1;      
    }

  //recherche des regions

  int regcour=0;
  int trouve=0;
  int coordI, coordJ;
  regionC* c;

  for (int i=0; i<numberInitial; i++)
    {
      trouve=0;
      coordI=0;
      coordJ=0;
      do
	{
	  c = FindCompress(bm[coordI*Wi+coordJ]);
	  if (c->no == regcour)
	    {
	      trouve=1;
	      refRet[regcour][0]=coordI;
	      refRet[regcour][1]=coordJ;
	      refRet[regcour][2]=regcour;

	      refRet[regcour][4]=c->nb;
	      regcour++;
	    }
	  coordJ++;
	  if ( coordJ==Wi )
	    {
	      coordJ=0;
	      coordI++;
	    }
	}while( (trouve==0) && (coordI<Le) );
      assert(trouve==1);
    }

  finalMerge(bm, refRet, numberInitial, Wi, Le);
}

void OcclusionMergeSort(region** &bm, int Wi, int Le){
  printf("Executing region merging between nonadjacent regions, with preliminary sorting of couples\n\n");

  int numberInitial=renumerote(bm, Wi, Le);

  printf("the number of initial regions is %d\n",numberInitial);

  //initialisation des parametres

  int**  refRet=new int* [numberInitial];
  int* tabI, *tabJ;
  int index;

  for (int i = 0; i <numberInitial ; i++) 
    {
      refRet[i]=new int [5];
      refRet[i][0]=-1;
      refRet[i][1]=-1;
      refRet[i][2]=-1;
      refRet[i][3]=-1;
      refRet[i][4]=-1;      
    }

  //recherche des regions

  int regcour=0;
  int trouve=0;
  int coordI, coordJ;
  regionC* c;
  regionC *ra, *rb, *rc, *rd; 
  int valdifi, valdifj, temp, npairs;

  for (int i=0; i<numberInitial; i++)
    {
      trouve=0;
      coordI=0;
      coordJ=0;
      do
	{
	  c = FindCompress(bm[coordI*Wi+coordJ]);
	  if (c->no == regcour)
	    {
	      trouve=1;
	      refRet[regcour][0]=coordI;
	      refRet[regcour][1]=coordJ;
	      refRet[regcour][2]=regcour;

	      refRet[regcour][4]=c->nb;
	      regcour++;
	    }
	  coordJ++;
	  if ( coordJ==Wi )
	    {
	      coordJ=0;
	      coordI++;
	    }
	}while( (trouve==0) && (coordI<Le) );
      assert(trouve==1);
    }

  //sorting - not optimized

  assert(regcour>1);
  npairs=numberInitial*(numberInitial-1)/2;

  tabI=new int [npairs];
  tabJ=new int [npairs];
  index=0;
  for (int i=0;i<numberInitial-1;i++)
    {
      for (int j=i+1; j<numberInitial; j++)
	{
	  tabI[index]=i;
	  tabJ[index]=j;
	  index++;
	}
    }

  for (int i=0;i<npairs-1;i++)
    {
      for (int j=i+1; j<npairs; j++)
	{
	  //paire i
	  ra = FindCompress(bm[refRet[tabI[i]][0]*Wi+refRet[tabI[i]][1]]);
	  rb = FindCompress(bm[refRet[tabJ[i]][0]*Wi+refRet[tabJ[i]][1]]);

	  //paire j
	  rc = FindCompress(bm[refRet[tabI[j]][0]*Wi+refRet[tabI[j]][1]]);
	  rd = FindCompress(bm[refRet[tabJ[j]][0]*Wi+refRet[tabJ[j]][1]]);

	  valdifi = max( (abs(ra->somR-rb->somR)) 
			 , ( max ( (abs(ra->somG-rb->somG)) 
			    , (abs(ra->somB-rb->somB)) ) )  );

	  valdifj = max( (abs(rc->somR-rd->somR)) 
			 , ( max ( (abs(rc->somG-rd->somG)) 
			    , (abs(rc->somB-rd->somB)) ) )  );

	  if (valdifj < valdifi){
	    temp = tabI[j];
	    tabI[j] = tabI[i];
	    tabI[i] = temp;
	    
	    temp = tabJ[j];
	    tabJ[j] = tabJ[i];
	    tabJ[i] = temp;
	  }
	}
    }

  finalMergeSort(bm, refRet, tabI, tabJ, numberInitial, Wi, Le);
}

void finalMergeSort(region** &bm, int** refCour, int* tabI, int* tabJ, int numTot, int Wi, int Le){
  printf("Beware, this is the beginning of non-adjacent region merging  !\n");
  regionC *ra, *rb; 
  int currentMerges=0;
  int index=0;

  for (int i=0;i<numTot-1;i++)
    {
      for (int j=i+1; j<numTot; j++)
	{
	  ra= FindCompress(bm[refCour[tabI[index]][0]*Wi+refCour[tabI[index]][1]]);
	  rb= FindCompress(bm[refCour[tabJ[index]][0]*Wi+refCour[tabJ[index]][1]]);

	  if ( (ra!=rb) && OracleProbabiliste(Wi, Le, ra, rb)  ){ 
	    Union(ra, rb);
	    currentMerges++;
	  }
	  index++;
	}
    }
  printf("Number of merges Done %d\n",currentMerges);

}

void finalMerge(region** &bm, int** refCour, int numTot, int Wi, int Le){
  printf("Beware, this is the beginning of non-adjacent region merging  !\n");
  regionC *ra, *rb; 
  int currentMerges=0;

  for (int i=0;i<numTot-1;i++)
    {
      for (int j=i+1; j<numTot; j++)
	{
	  ra= FindCompress(bm[refCour[i][0]*Wi+refCour[i][1]]);
	  rb= FindCompress(bm[refCour[j][0]*Wi+refCour[j][1]]);

	  if ( (ra!=rb) && OracleProbabiliste(Wi, Le, ra, rb)  ){ 
	    Union(ra, rb);
	    currentMerges++;
	  }
	}
    }
  printf("Number of merges Done %d\n",currentMerges);

}
