//Generation de bruit

#include "var_commune.hh"
#include "srmBproc.hh"
#include "limits.h"
#include "float.h"
#include "assert.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define abs(x) ( (x) < 0 ? (-x) : (x) )
#define min(x,y) ( (x) < (y) ? (x) : (y) )

float ranf(void)
{
  return ((float) rand())/((float) RAND_MAX);
}

float box_muller(float m, float s)      /* normal random variate generator */
{                                       /* mean m, standard deviation s */
        float x1, x2, w, y1;

        static float y2;
        static int use_last = 0;

        if (use_last)                   /* use value from previous call */
        {
                y1 = y2;
                use_last = 0;
        }
        else
        {
                do {
                        x1 = 2.0 * ranf() - 1.0;
                        x2 = 2.0 * ranf() - 1.0;
                        w = x1 * x1 + x2 * x2;
                } while ( w >= 1.0 );

                w = sqrt( (-2.0 * log( w ) ) / w );
                y1 = x1 * w;
                y2 = x2 * w;
                use_last = 1;
        }

        return( m + y1 * s );
}

int uniform(int vmax)
{
  return (rand()%vmax);
}

void gen_occlusion(regionC** &bm, int Wi, int Le)
{
  int nocc=30;
  int wi=40;
  int col=255;
  int numo=0;
  printf("Generating %d occlusions with width %d, and color %d\n",nocc,wi, col);
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
    for (i = 0; i < Le; i++) 
      {
	for (j=0; j<wi; j++)
	  {
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
  printf("percentage of pixels occluded in the image: %f\n",(float) numo/(Wi*Le));
}

void bigcircle(regionC** &bm, int Wi, int Le, int Qiou)
{
  float vcou,vv;
  float vmocou,vmacou;
  int i,j,k;

  float valc=128; //interieur du concept
  float valout=126; //exterieur du concept
  
  printf("Concept cible: cercle centre sur l'image, de parametres ");
  float rayon=4* (min ( ((float) Wi) , ((float) Le) ))/(float) 10;
  
  float cx=(float) Wi/2;
  float cy=(float) Le/2;
  
  printf("(%d,%d),r=%d\n\n",(int) cx,(int) cy,(int) rayon);
  
  vmacou=(float) 256/Qiou;
  
  for (i = 0; i < Le; i++) 
    {	
      for (j = 0; j < Wi; j++) 
	{ 
	  vcou=0;
	  if ( (float) ( (((float) i-cy)*((float) i-cy)) + (((float) j-cx)*((float) j-cx)) ) <= (rayon*rayon) )
	    {
	      vmocou=(float) valc/Qiou;
	    }
	  else
	    {
	      vmocou=(float) valout/Qiou;
	    }
	  
	  for (k=0;k<Qiou;k++)
	    {
	      if (rand()%2==0)
		{
		  vcou+=(float) uniform((int) (2*vmocou));
		}
	      else
		{
		  vv=box_muller(vmocou,5);
		  if (vv>vmacou)
		    {
		      vv=vmacou;
		    }
		  else if (vv<0)
		    {
		      vv=0;
		    }
		  vcou+=vv;
		}
	    }
	  bm[i*Wi+j]->valoriR=(int) vcou;
	  bm[i*Wi+j]->valoriG=(int) vcou;
	  bm[i*Wi+j]->valoriB=(int) vcou; 
	  
	  bm[i*Wi+j]->somR=bm[i*Wi+j]->valoriR;
	  bm[i*Wi+j]->somG=bm[i*Wi+j]->valoriG;
	  bm[i*Wi+j]->somB=bm[i*Wi+j]->valoriB;   
	}
    }
}

void troisRectangles(regionC** &bm, int Wi, int Le, int Qiou)
{
  float vcou,vv;
  float vmocou,vmacou;
  int i,j,k;

  float val1=200; //interieur du rectangle 1
  float val2=120; //interieur du rectangle 2
  float val3=80; //interieur du rectangle 3

  printf("Concept cible: Trois rectangles verticaux limités en potisions");
  float lim1=((float) Wi)/3;
  float lim2=2*((float) Wi)/3;

  printf(" %d,%d \n\n",(int) lim1, (int) lim2);
  
  vmacou=(float) 256/Qiou;
  
  for (i = 0; i < Le; i++) 
    {	
      for (j = 0; j < Wi; j++) 
	{ 
	  vcou=0;
	  if ( j < lim1 )
	    {
	      vmocou=(float) val1/Qiou;
	    }
	  else if ( j < lim2 )
	    {
	      vmocou=(float) val2/Qiou;
	    }
	  else 
	    {
	      vmocou=(float) val3/Qiou;
	    }
	  
	  for (k=0;k<Qiou;k++)
	    {
	      if (rand()%2==0)
		{
		  vcou+=(float) uniform((int) (2*vmocou));
		}
	      else
		{
		  vv=box_muller(vmocou,5);
		  if (vv>vmacou)
		    {
		      vv=vmacou;
		    }
		  else if (vv<0)
		    {
		      vv=0;
		    }
		  vcou+=vv;
		}
	    }
	  bm[i*Wi+j]->valoriR=(int) vcou;
	  bm[i*Wi+j]->valoriG=(int) vcou;
	  bm[i*Wi+j]->valoriB=(int) vcou; 
	  
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

   printf ("\n\n\n ->Noisification: Generation de salt and pepper noise sur %d percents \n\n\n",noise);

   for (i = 0; i < Le; i++) 
     {	
       for (j = 0; j < Wi; j++) 
	 {   
	   if (rand()%100<nm)
	     {
	       if (rand()%2==0)
		 {
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

void randomnoise(regionC** &bm, int Wi, int Le, int noise) 
{
   int i,j;
   int nm=noise;
   int vam;

   printf ("\n\n\n -> Noisification: Generation de (random) transmission noise sur %d percents base sur paper de Xu\n\n\n",noise);

   for (i = 0; i < Le; i++) 
     {	
       for (j = 0; j < Wi; j++) 
	 {   
	   if (rand()%100<nm)
	     {
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


void transformImage(regionC** &bm, int Wi, int Le, int Qiou, int numConcept)
{
  //numConcept=numero d'image generee
  if (numConcept==0)
    {
      bigcircle(bm,Wi,Le,Qiou);
    }
  else if (numConcept==1)
    {
      troisRectangles(bm,Wi,Le,Qiou);
    }
}


void normalnoise(regionC** &bm, int Wi, int Le, float m, float s) 
{
   int i,j;
   int addval;

   printf ("\n\n\n -> Noisification: Generation de additive Gaussian noise sur chaque pixel partant du paper de Xu, N(%f,%f) \n\n\n",m,s);

   for (i = 0; i < Le; i++) 
     {	
       for (j = 0; j < Wi; j++) 
	 {   
	   addval=(int) box_muller(m,s);

	   if (addval<-128)
	     {
	       addval=-128;
	     }
	   if (addval>128)
	     {
	       addval=128;
	     }

	   bm[i*Wi+j]->valoriR+=addval;
	   if (bm[i*Wi+j]->valoriR<0)
	     {
	       bm[i*Wi+j]->valoriR=0;
	     }

	   if (bm[i*Wi+j]->valoriR>255)
	     {
	       bm[i*Wi+j]->valoriR=255;
	     }

	   addval=(int) box_muller(m,s);
	   if (addval<-128)
	     {
	       addval=-128;
	     }
	   if (addval>128)
	     {
	       addval=128;
	     }

	   bm[i*Wi+j]->valoriG+=addval;
	   if (bm[i*Wi+j]->valoriG<0)
	     {
	       bm[i*Wi+j]->valoriG=0;
	     }

	   if (bm[i*Wi+j]->valoriG>255)
	     {
	       bm[i*Wi+j]->valoriG=255;
	     }


	   addval=(int) box_muller(m,s);
	   if (addval<-128)
	     {
	       addval=-128;
		 }
	   if (addval>128)
	     {
	       addval=128;
	     }

	   bm[i*Wi+j]->valoriB+=addval;
	   if (bm[i*Wi+j]->valoriB<0)
	     {
	       bm[i*Wi+j]->valoriB=0;
	     }

	   if (bm[i*Wi+j]->valoriB>255)
	     {
	       bm[i*Wi+j]->valoriB=255;
	     }

	   bm[i*Wi+j]->somR=bm[i*Wi+j]->valoriR;
	   bm[i*Wi+j]->somG=bm[i*Wi+j]->valoriG;
	   bm[i*Wi+j]->somB=bm[i*Wi+j]->valoriB;   	  
	 }
     }
} 
