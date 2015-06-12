#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "var_commune.hh"
#include "srmproc.hh"
#include "limits.h"
#include "float.h"
#include "assert.h"

#define NB_MAX_REGION 15
#define Delta 10

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

int mergesMade=0;
static char BUF[12];

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of loading functions

int readBmRGB(const char *filename, regionC** &bm, int &Wi, int &Le)
{
  printf("Reading image...");
  fflush(stdout);

  if (readRGBtiffparam(filename, &Wi, &Le) == 0){
    printf ("erreur readRGBtiffparam"); 
    return 0; 
  }

  unsigned int *buffer= (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));

  if (readRGBtiff(filename, buffer) == 0){
    printf ("erreur readRGBtiff"); 
    return 0; 
  }

  bm = (regionC**) (malloc(sizeof(regionC*)*Wi*Le));
  for (int i = 0; i < Le; i++){
    for (int j = 0; j < Wi; j++){
      bm[i*Wi+j]              = new region; 
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
  printf("done\n");
  return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of misc. functions

char* itoa(int i)
{
  char *pos = BUF + sizeof(BUF) - 1;
  unsigned int u;
  int negative = 0;
  if (i < 0){
    negative = 1;
    u = ((unsigned int)(-(1+i))) + 1;
  }else{
    u = i;
  }
  *pos = 0;
  do{
    *--pos = '0' + (u % 10);
    u /= 10;
  }while (u);
  if (negative){
    *--pos = '-';
  }
  return pos;
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
     p->parent = FindCompress(p->parent); 
     return p->parent;
   }
} 

void Union (regionC* x, regionC* y) 
{ 
   if (x != y){
     y->somR       = x->somR + y->somR;
     y->somG       = x->somG + y->somG;
     y->somB       = x->somB + y->somB;
     y->somAlpha   = x->somAlpha + y->somAlpha;
     
     y->nb = y->nb + x->nb;
     x->parent = y;
   } 
} 

int checkpixel(regionC* r1, regionC* r2, int color)
{
  return ((r1 != r2) && (TIFFGetR(color) != 0) && (TIFFGetG(color) != 0) && (TIFFGetB(color) != 0));
}

int numerote(regionC** &bm, int Wi, int Le) 
{ 
   int cpt = 0;

   regionC* p; 
   for (int i = 0; i < Le; i++){
     for (int j = 0; j < Wi; j++){       
       p = FindCompress(bm[i*Wi+j]); 
       if(p->no == -1){       
	 p->no = cpt; 
	 cpt++; 
       } 
     }
   } 
   return (cpt);
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of noise generating functions

void randomnoise(regionC** &bm, int Wi, int Le, int noise) 
{
   int i,j;
   int nm=noise;
   int vam;
   assert( (noise>=0) && (noise<=100) );
   for (i = 0; i < Le; i++){	
     for (j = 0; j < Wi; j++){   
       if (rand()%100<nm){
	 vam=rand()%256;
	 
	       bm[i*Wi+j]->valoriR=vam;
	       bm[i*Wi+j]->valoriG=vam;
	       bm[i*Wi+j]->valoriB=vam;
       }   	   
       bm[i*Wi+j]->somR=bm[i*Wi+j]->valoriR;
       bm[i*Wi+j]->somG=bm[i*Wi+j]->valoriG;
       bm[i*Wi+j]->somB=bm[i*Wi+j]->valoriB;   	   
     }
   }
} 

void saltandpepper(regionC** &bm, int Wi, int Le, int noise) 
{
   int i,j;
   int nm=noise;

   assert( (noise>=0) && (noise<=100) );
   for (i = 0; i < Le; i++){	
     for (j = 0; j < Wi; j++){   
       if (rand()%100<nm){
	 if (rand()%2==0){
	   bm[i*Wi+j]->valoriR=0;
	   bm[i*Wi+j]->valoriG=0;
	   bm[i*Wi+j]->valoriB=0;		   
	 }
	 else
	   {
	     bm[i*Wi+j]->valoriR=255;
	     bm[i*Wi+j]->valoriG=255;
	     bm[i*Wi+j]->valoriB=255;
	   }
       }          
       bm[i*Wi+j]->somR=bm[i*Wi+j]->valoriR;
       bm[i*Wi+j]->somG=bm[i*Wi+j]->valoriG;
       bm[i*Wi+j]->somB=bm[i*Wi+j]->valoriB;   	   
     }
   }
} 

float ranf(void)
{
  return ((float) rand())/((float) RAND_MAX);
}

float box_muller(float m, float s)      
{                                       
  float x1, x2, w, y1;
  
  static float y2;
  static int use_last = 0;
  
  if (use_last){
    y1 = y2;
    use_last = 0;
  }else{
    do{
      x1 = 2.0 * ranf() - 1.0;
      x2 = 2.0 * ranf() - 1.0;
      w = x1 * x1 + x2 * x2;
    }while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    use_last = 1;
  }
  return( m + y1 * s );
}

void normalnoise(regionC** &bm, int Wi, int Le, float m, float s) 
{
   int i,j;
   int addval;

   assert( (s>0) );
   for (i = 0; i < Le; i++){	
     for (j = 0; j < Wi; j++){   
       addval=(int) box_muller(m,s);
       if (addval<-128) addval=-128;
       if (addval>128) addval=128;
       bm[i*Wi+j]->valoriR+=addval;
       if (bm[i*Wi+j]->valoriR<0) bm[i*Wi+j]->valoriR=0;
       if (bm[i*Wi+j]->valoriR>255) bm[i*Wi+j]->valoriR=255;
       
       addval=(int) box_muller(m,s);
       if (addval<-128) addval=-128;
       if (addval>128) addval=128;
       bm[i*Wi+j]->valoriG+=addval;
       if (bm[i*Wi+j]->valoriG<0) bm[i*Wi+j]->valoriG=0;
       if (bm[i*Wi+j]->valoriG>255) bm[i*Wi+j]->valoriG=255;
       
       addval=(int) box_muller(m,s);
       if (addval<-128) addval=-128;
       if (addval>128) addval=128;
       bm[i*Wi+j]->valoriB+=addval;
       if (bm[i*Wi+j]->valoriB<0) bm[i*Wi+j]->valoriB=0;
       if (bm[i*Wi+j]->valoriB>255) bm[i*Wi+j]->valoriB=255;
       
       bm[i*Wi+j]->somR=bm[i*Wi+j]->valoriR;
       bm[i*Wi+j]->somG=bm[i*Wi+j]->valoriG;
       bm[i*Wi+j]->somB=bm[i*Wi+j]->valoriB;   	  
     }
   }
} 

void gen_occlusion(regionC** &bm, int Wi, int Le, int nocc)
{
  assert( (nocc>0) );

  int wi=40;
  int col=255;
  int numo=0;
  int px, py;
  int a, i, j, k, pi, pj;
  int tabdone[Le][Wi];
  for (i=0;i<Le;i++){
    for (j=0;j<Wi;j++){
      tabdone[i][j]=0;
    }
  }

  for (k=0;k<nocc;k++){
    a=(rand()%21)-10;
    py=(rand()%Wi);
    px=(rand()%Le);
    for (i = 0; i < Le; i++){
      for (j=0; j<wi; j++){
	pi=i;
	pj=py+a*(px-i)+j;
	if ( (pj>=0) && (pj<Wi) ){
	  if (tabdone[pi][pj]==0){
	    numo++;
	    tabdone[pi][pj]=1;
	  }
	  bm[pi*Wi+pj]->valoriR=col;
	  bm[pi*Wi+pj]->valoriG=col;
	  bm[pi*Wi+pj]->valoriB=col; 
	  
	  bm[pi*Wi+pj]->valoriRSauv=col;
	  bm[pi*Wi+pj]->valoriGSauv=col;
	  bm[pi*Wi+pj]->valoriBSauv=col; 
	  
	  bm[pi*Wi+pj]->somR=col;
	  bm[pi*Wi+pj]->somG=col;
	  bm[pi*Wi+pj]->somB=col; 
	}
      }
    }
  }
  printf("(ratio #occluded_pixels/#pixels = %f)...",(float) numo/(Wi*Le));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of ${\cal P}(R,R')$

int mergingOracle(int Wi, int Le, regionC* x, regionC* y) 
{
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of function $f(R, R')$ prior to sorting

int ecart(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2)
{
  int valaret=0;

  if ( (TypeOfNeighbor==1) && (SobelGradient>=1) ){
    printf("\n \n* Please do not choose -U 1 and -G 1 altogether *\n");
    exit(0);
  }

  if (SobelGradient>=1){
    if ( (i1==0) || (i1==Le-1) || 
	 (j1==0) || (j1==Wi-1) ||
	 (i2==0) || (i2==Le-1) ||
	 (j2==0) || (j2==Wi-1) ) valaret=ecartNonBruit(bm,Wi,Le,i1,j1,i2,j2);
    else valaret=ecartSobelGeneralized(bm,Wi,Le,i1,j1,i2,j2);
  }else if (TypeOfNeighbor==0) valaret=ecartNonBruit(bm,Wi,Le,i1,j1,i2,j2);
  else if (TypeOfNeighbor==1) valaret=ecartSquareBruit(bm,Wi,Le,i1,j1,i2,j2,Delta);

  return valaret;
}

int ecartSobelGeneralized(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2){
  //Extension of Sobel filter to arbitrary size
  int valaret, valR=0, valG=0, valB=0, i, j;
  int truesize;
  int* derR, * derG, * derB, *smo;
  int ponder;

  //Smoothing is performed by c.mask : [1 2 ... s+1 s ... 1] with (here) s=truesize
  //Derivative filter performed by a local linear regression

  if (i1==i2){    
    truesize=min( (fSize) , 
		 (min( (i1) , 
		       (min( (Le-1-i1), 
			     (min( (j1) , 
				   (min( (Wi-1-j1) , 
					 (min( (j2) , 
					       (Wi-1-j2))))))))))) );
    derR = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    derG = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    derB = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    smo = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    for (i=0;i<(2*truesize)+1;i++){
      smo[i]=(i<truesize+1) ? i+1 : (2*truesize)+1-i;
    }

    for (i=i1-truesize;i<=i1+truesize;i++){
      for (j=0;j<(2*truesize) + 1;j++){
	derR[j]=(j<truesize) ? bm[i*Wi+(j1-truesize+j)]->valoriR : 
	  ((j>truesize) ? bm[i*Wi+(j2+(j-truesize))]->valoriR : 0) ;

	derG[j]=(j<truesize) ? bm[i*Wi+(j1-truesize+j)]->valoriG : 
	  ((j>truesize) ? bm[i*Wi+(j2+(j-truesize))]->valoriG : 0) ;

	derB[j]=(j<truesize) ? bm[i*Wi+(j1-truesize+j)]->valoriB : 
	  ((j>truesize) ? bm[i*Wi+(j2+(j-truesize))]->valoriB : 0) ;
      }

      for (j=0;j<(2*truesize) + 1;j++){
	valR+=smo[i-(i1-truesize)]*(j-truesize)*derR[j];
	valG+=smo[i-(i1-truesize)]*(j-truesize)*derG[j];
	valB+=smo[i-(i1-truesize)]*(j-truesize)*derB[j];
      }
    }

    ponder=(truesize==1) ? (truesize+1)*(truesize+1) : (truesize*(truesize+1)*(2*truesize+1)/3)+(truesize+1)*(truesize+1);

    free(derR);
    free(derG);
    free(derB);
  }else{
    assert(j1==j2);

    truesize=min( (fSize) , 
		 (min( (j1) , 
		       (min( (Wi-1-j1), 
			     (min( (i1) , 
				   (min( (Le-1-i1) , 
					 (min( (i2) , 
					       (Le-1-i2))))))))))) );
    derR = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    derG = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    derB = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    smo = (int*) malloc( ((2*truesize) + 1)*sizeof(int) );
    for (i=0;i<(2*truesize)+1;i++) smo[i]=(i<truesize+1) ? i+1 : (2*truesize)+1-i;

    for (j=j1-truesize;j<=j1+truesize;j++){   
      for (i=0;i<(2*truesize) + 1;i++){
	derR[i]=(i<truesize) ? bm[(i1-truesize+i)*Wi+j]->valoriR : 
	  ((i>truesize) ? bm[(i2+(i-truesize))*Wi+j]->valoriR : 0) ;

	derG[i]=(i<truesize) ? bm[(i1-truesize+i)*Wi+j]->valoriG : 
	  ((i>truesize) ? bm[(i2+(i-truesize))*Wi+j]->valoriG : 0) ;

	derB[i]=(i<truesize) ? bm[(i1-truesize+i)*Wi+j]->valoriB : 
	  ((i>truesize) ? bm[(i2+(i-truesize))*Wi+j]->valoriB : 0) ;
      }
      for (i=0;i<(2*truesize) + 1;i++){
	valR+=(smo[j-(j1-truesize)]*(truesize-i)*derR[i]);
	valG+=(smo[j-(j1-truesize)]*(truesize-i)*derG[i]);
	valB+=(smo[j-(j1-truesize)]*(truesize-i)*derB[i]);
      }
    }

    ponder=(truesize==1) ? (truesize+1)*(truesize+1) : (truesize*(truesize+1)*(2*truesize+1)/3)+(truesize+1)*(truesize+1);

    free(derR);
    free(derG);
    free(derB);
  }


  valR/=ponder;
  valG/=ponder;
  valB/=ponder;

  valR=abs(valR);
  valG=abs(valG);
  valB=abs(valB);

  valaret=max ( (valR) , max( (valG) , (valB) ) );
  return valaret;
}

int ecartSquareBruit(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2, int delta)
{
  int valaret=0;
  int i,j;
  int nouval1R,nouval2R,nouval1G,nouval2G,nouval1B,nouval2B;
  int numb;

  if (j2>j1){
    assert(i1==i2);
    numb=0;
    nouval1R=0;
    nouval1G=0;
    nouval1B=0;
    for (i=max( (0), (i1-delta) ); i<min( (Le) , (i1+delta) );i++){
      for (j=max( (0), (j1-delta) ); j<=j1; j++){
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
    
    for (i=max( (0), (i1-delta) ); i<min( (Le) , (i1+delta) );i++){
      for (j=j2; j<min ( (Wi) , (j2+delta) ); j++){
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
  }else if (i2>i1){
    
    assert(j1==j2);
    numb=0;
    nouval1R=0;
    nouval1G=0;
    nouval1B=0;
    for (i=max( (0), (i1-delta) ); i<=i1;i++){
      for (j=max( (0), (j1-delta) ); j<min ( (Wi) , (j1+delta) ); j++){
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
    
    for (i=i2; i<min( (Le) , (i2+delta) );i++){
      for (j=max( (0), (j1-delta) ); j<min ( (Wi) , (j1+delta) ); j++){
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
  }else{
      printf("Error !! Check pixel classification\n \n");
  }
  return valaret;
}

int ecartNonBruit(region** &bm, int Wi, int Le, int i1, int j1, int i2, int j2)
{
  int valaret;
  valaret=max( (abs(bm[i1*Wi+j1]->valoriR-bm[i2*Wi+j2]->valoriR)) 
	       , (max ( (abs(bm[i1*Wi+j1]->valoriG-bm[i2*Wi+j2]->valoriG)) 
			, (abs(bm[i1*Wi+j1]->valoriB-bm[i2*Wi+j2]->valoriB)) )) );
  return valaret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of \texttt{sort$\_$increasing}

void quicksort(int** refTab, int l, int r){
  int i, j, t;
  int *v;

  if (r>l){
    v=refTab[r]; i=l-1; j=r;
    
    for (;;){
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
    
    quicksort(refTab,l,i-1);
    quicksort(refTab,i+1,r);
  }
}

int** sorting(region** &bm, int Wi, int Le){
  int** refRet;

  refRet=new int* [((Wi-1)*Le + Wi*(Le-1))];
  for (int i = 0; i <((Wi-1)*Le + Wi*(Le-1)) ; i++){
    refRet[i]=new int [5];
    refRet[i][0]=-1;
    refRet[i][1]=-1;
    refRet[i][2]=-1;
    refRet[i][3]=-1;
    refRet[i][4]=-1;      
  }

  int dep2=(Wi-1)*Le;

  for (int i = 0; i < Le; i++){
    for (int j = 0; j < Wi-1; j++){
      refRet[i*(Wi-1)+j][0]=i;
      refRet[i*(Wi-1)+j][1]=j;
      refRet[i*(Wi-1)+j][2]=i;
      refRet[i*(Wi-1)+j][3]=j+1;
      refRet[i*(Wi-1)+j][4]=ecart(bm,Wi,Le,i,j,i,j+1);
    }
  }

  for (int i = 0; i < Le-1; i++){
    for (int j = 0; j < Wi; j++){
      refRet[dep2+i*Wi+j][0]=i;
      refRet[dep2+i*Wi+j][1]=j;
      refRet[dep2+i*Wi+j][2]=i+1;
      refRet[dep2+i*Wi+j][3]=j;
      refRet[dep2+i*Wi+j][4]=ecart(bm,Wi,Le,i,j,i+1,j);
    }
  }
  quicksort(refRet,0,((Wi-1)*Le + Wi*(Le-1))-1);
  return refRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// SRM

void SRM(region** &bm, int Wi, int Le){
  printf("Executing Statistical Region Merging on a %dx%d image... Step 1: sorting...",Wi,Le);
  fflush(stdout);

  int** refSorted;

  if (TypeOfNeighbor==1) printf("with noise handling (Delta=%d)...",Delta);
  else if (SobelGradient==1) printf("with a Sobel-type convolution filter of size %d...",((2*fSize)+1));
  fflush(stdout);

  refSorted=sorting(bm, Wi, Le);

  printf("done\n");
  fflush(stdout);

  printf("Executing Statistical Region Merging on a %dx%d image... Step 2: merging...",Wi,Le);
  fflush(stdout);

  regionC *ra, *rb; 
  int currentMerges=0;

  for (int i = 1; i < ((Wi-1)*Le + Wi*(Le-1)); i++){ 
    ra= FindCompress(bm[refSorted[i][0]*Wi+refSorted[i][1]]);
    rb= FindCompress(bm[refSorted[i][2]*Wi+refSorted[i][3]]);
    
    if ( (ra!=rb) && mergingOracle(Wi, Le, ra, rb)  ){ 
      Union(ra, rb); 
      mergesMade++;   
      currentMerges++; 
    } 
  }
  printf("done\n");
}

void eliminer_petites_regions(regionC** &bm, int Wi, int Le)
{
  regionC *predreg, *smallreg;

  int tmax=10;
  int remov=0;

  if ( (Wi>128) && (Le>128) ) tmax*=(Wi/128);

  for (int i = 0; i < Le; i++){
    for (int j = 1; j < Wi; j++){ 
      smallreg = FindCompress(bm[i*Wi+j]);
      predreg = FindCompress(bm[i*Wi+j-1]);
      if (smallreg != predreg) {
        if (smallreg->nb <= tmax) {
	  remov++;
          Union(smallreg, predreg);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of functions for sorting/merging for occlusion merging

void OcclusionSortSRM(region** &bm, int Wi, int Le){
  printf("Executing occlusion merging...");
  fflush(stdout);

  int numberInitial=renumerote(bm, Wi, Le);

  int**  refRet=new int* [numberInitial];
  int* tabI, *tabJ;
  int index;

  for (int i = 0; i <numberInitial ; i++){
    refRet[i]=new int [5];
    refRet[i][0]=-1;
    refRet[i][1]=-1;
    refRet[i][2]=-1;
    refRet[i][3]=-1;
    refRet[i][4]=-1;      
  }
  
  int regcour=0;
  int trouve=0;
  int coordI, coordJ;
  regionC* c;
  regionC *ra, *rb, *rc, *rd; 
  int valdifi, valdifj, temp, npairs;

  for (int i=0; i<numberInitial; i++){
    trouve=0;
    coordI=0;
    coordJ=0;
    do{
      c = FindCompress(bm[coordI*Wi+coordJ]);
      if (c->no == regcour){
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
  assert(regcour>1);
  npairs=numberInitial*(numberInitial-1)/2;

  tabI=new int [npairs];
  tabJ=new int [npairs];
  index=0;
  for (int i=0;i<numberInitial-1;i++){
    for (int j=i+1; j<numberInitial; j++){
      tabI[index]=i;
      tabJ[index]=j;
      index++;
    }
  }
  
  for (int i=0;i<npairs-1;i++){
    for (int j=i+1; j<npairs; j++){
      ra = FindCompress(bm[refRet[tabI[i]][0]*Wi+refRet[tabI[i]][1]]);
      rb = FindCompress(bm[refRet[tabJ[i]][0]*Wi+refRet[tabJ[i]][1]]);
      
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
  OcclusionSRM(bm, refRet, tabI, tabJ, numberInitial, Wi, Le);
}

void OcclusionSRM(region** &bm, int** refCour, int* tabI, int* tabJ, int numTot, int Wi, int Le){
  regionC *ra, *rb; 
  int currentMerges=0;
  int index=0;

  for (int i=0;i<numTot-1;i++){
    for (int j=i+1; j<numTot; j++){
      ra= FindCompress(bm[refCour[tabI[index]][0]*Wi+refCour[tabI[index]][1]]);
      rb= FindCompress(bm[refCour[tabJ[index]][0]*Wi+refCour[tabJ[index]][1]]);
      
      if ( (ra!=rb) && mergingOracle(Wi, Le, ra, rb)  ){ 
	Union(ra, rb);
	currentMerges++;
      }
      index++;
    }
  }
  printf("...done\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of saving functions

void contour(unsigned int* data, int i, int j, int Wi, int Le, int curblack, int curwhite)
{
  int k,l;
  //Taille de la frontiere=taille*2+1
  int taille=2;
  int mini=((i-taille)>0) ? i-taille : 0 ;
  int maxi=((i+taille)<Le) ? i+taille : Le-1 ;
  int minj=((j-taille)>0) ? j-taille : 0 ;
  int maxj=((j+taille)<Wi) ? j+taille : Wi-1 ;
  int drawborder=( (BorderColor != 2) || ( (Sortie!=0) && (Sortie!=2) ) );

  if (drawborder)
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

   for (int i = 0; i < Le; i++){	
     for (int j = 0; j < Wi; j++){   
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
   for (int i = 0; i < (Le/2); i++){
     for (int j = 0; j < Wi; j++){
       tmp=data[(Le-1-i)*Wi+j];
       data[(Le-1-i)*Wi+j]=data[i*Wi+j];
       data[i*Wi+j]=tmp;
     }
   }
   
   if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0) printf ("erreur readtiffgl");

   free(data);
}

void sauveRGBimage(const char *filename, regionC** &bm, int Wi, int Le) 
{
  printf("Saving output image...");
  fflush(stdout);

  unsigned int *data;
   data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
   regionC* c, *l, *u;; 
   int pixelcolorR, pixelcolorG, pixelcolorB, pixelcolorAlpha, line = 0;
   int white = 255; 
   int black = 0;

   int curblack, curwhite;

   if ( (Sortie==0) || (Sortie==1) || (Sortie==2) || (Sortie==3) ){
     for (int i = 0; i < Le; i++){	
       for (int j = 0; j < Wi; j++){   
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
	 
	 if (Sortie != 3){
	   if( i == 0 ){   
	     if( line-- ){   
	       l = FindCompress(bm[i*Wi+j-1]); 
	       if( checkpixel(c, l, data[i*Wi+j-1]) ) contour(data, i, j, Wi, Le, curblack, curwhite);
	       else if (Sortie==1){  
		 data[i*Wi+j] = curwhite; 
	       }
	     }else{   
	       line = Wi;
	       if (Sortie==1) data[i*Wi+j] = curwhite; }
	   }else{  
	     u = FindCompress(bm[(i-1)*Wi+j]);
	     if( line-- ){   
	       l = FindCompress(bm[i*Wi+j-1]); 
	       if( checkpixel(c, l, data[i*Wi+j-1]) ||
		   checkpixel(c, u, data[(i-1)*Wi+j]) )
		 contour(data, i, j, Wi, Le, curblack, curwhite);
	       else if (Sortie==1){   
		 data[i*Wi+j] = curwhite; 
	       }
	     }else{  
	       line = Wi;
	       if( checkpixel(c, u, data[(i-1)*Wi+j])) contour(data, i, j, Wi, Le, curblack, curwhite);
	       else if (Sortie==1){ 
		 data[i*Wi+j] = curwhite; }
	     }
	   }
	 }
	 if ( (i==0) || (i==Le-1) || (j==0) || (j==Wi-1) ){
	   contour(data, i, j, Wi, Le, curblack, curwhite);
	 }
       }
     }
   }
   

   //on renverse l'image
   int tmp;
   for (int i = 0; i < (Le/2); i++){
     for (int j = 0; j < Wi; j++){
       tmp=data[(Le-1-i)*Wi+j];
       data[(Le-1-i)*Wi+j]=data[i*Wi+j];
       data[i*Wi+j]=tmp;
     }
   }
   
   if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0) printf ("erreur readtiffgl");

   free(data);
   printf("done\n");
}

void sauveRegion(int numero, int numeroR, regionC** &bm, int Wi, int Le) 
{
  char filename[255];
  strcpy(filename,outputfile);
  strcat(filename,"_Region");
  strcat(filename,itoa(numero));
  strcat(filename,".tif");

   unsigned int *data;
   data = (unsigned int*) (malloc(sizeof(unsigned int)*Wi*Le));
   regionC* c; 
   int pixelcolorR,pixelcolorG,pixelcolorB,pixelcolorAlpha;
   int white = 255; 
   int black = 0;

   //printf ("avant savetiffgl \n");

   for (int i = 0; i < Le; i++){	
     for (int j = 0; j < Wi; j++){   
       c = FindCompress(bm[i*Wi+j]); 
       
       if (c->no == numeroR){
	 pixelcolorR = bm[i*Wi+j]->valoriRSauv;
	 pixelcolorG = bm[i*Wi+j]->valoriGSauv;
	 pixelcolorB = bm[i*Wi+j]->valoriBSauv;
	 pixelcolorAlpha = bm[i*Wi+j]->valoriAlphaSauv;
       }else{
	 pixelcolorR = white;
	 pixelcolorG = white;
	 pixelcolorB = white;
	 pixelcolorAlpha = bm[i*Wi+j]->valoriAlphaSauv;
       }
       
       data[i*Wi+j] = pixelcolorR
	 + (pixelcolorG << 8)
	 + (pixelcolorB << 16)
	 + (pixelcolorAlpha << 24);   
       
       if ( (i==0) || (i==Le-1) || (j==0) || (j==Wi-1) ){
	 data[i*Wi+j] = black
	   + (black << 8)
	   + (black << 16)
	   + ((bm[i*Wi+j]->valoriAlphaSauv) << 24);
       }
     }
   }
   
   int tmp;
   for (int i = 0; i < (Le/2); i++){
     for (int j = 0; j < Wi; j++){
       tmp=data[(Le-1-i)*Wi+j];
       data[(Le-1-i)*Wi+j]=data[i*Wi+j];
       data[i*Wi+j]=tmp;
     }
   }
   
   if (saveRGBtiff(filename, "", "", data, PHOTOMETRIC_RGB, Wi, Le) == 0) printf ("erreur readtiffgl");

   free(data);
} 

void sauveParts(region** &bm, int Wi, int Le){
  printf("Saving regions...");
  fflush(stdout);

  int numberInitial=renumerote(bm, Wi, Le);

  //initialisation des parametres

  int**  refRet=new int* [numberInitial];
  for (int i = 0; i <numberInitial ; i++){
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

  for (int i=0; i<numberInitial; i++){
    trouve=0;
    coordI=0;
    coordJ=0;
    do{
      c = FindCompress(bm[coordI*Wi+coordJ]);
      if (c->no == regcour){
	trouve=1;
	refRet[regcour][0]=coordI;
	refRet[regcour][1]=coordJ;
	refRet[regcour][2]=regcour;
	
	refRet[regcour][4]=c->nb;
	regcour++;
      }
      coordJ++;
      if ( coordJ==Wi ){
	coordJ=0;
	coordI++;
      }
    }while( (trouve==0) && (coordI<Le) );
    assert(trouve==1);
  }
  
  // on ordonne

  quicksort(refRet,0,numberInitial-1);

  //on intervertit la liste

  int temp;
  for (int i=0; i<(numberInitial / 2); i++){
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
  for (int i=0; i<nbc; i++){
    sauveRegion(i, refRet[i][2], bm, Wi, Le); 
  }
  
  printf("done\n");
}
