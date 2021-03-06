/*
 * udpoint.c
 *
 * An archive of methods to generate uniformly distributed points on
 * sphere. 
 *
 * Copyrighted by Tien-Tsin Wong, 1996.
 * 
 * 19 October 1996.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <values.h>
#include "libcommon/common.h"

/*
 * This function cannot generate "uniformly" distributed random 
 * vector.
 */
Vector OnCube()
{
  Vector randomdir;
  /*
   * Notice that the following way of generation of random direction
   * cannot generate random points statistically evenly distribute on
   * a sphere. Instead it generates random points statistically evenly
   * distribute on a cube. A more suitable generation method sure be
   * used later.
   */
  randomdir.x = (Float)random()/(Float)MAXLONG - 0.5;
  randomdir.y = (Float)random()/(Float)MAXLONG - 0.5;
  randomdir.z = (Float)random()/(Float)MAXLONG - 0.5;
  VecNormalize(&randomdir);
  return randomdir;
}



/*
 * Generate a uniformly distributed random ray 
 * See paper for detail description of the technique
 */
Vector SphereStrip()
{
  Vector randomdir;
  /*
   * Generate random point on the surface of a sphere 
   * See diary how I generate random point evenly on a sphere 
   */
  Float theta, angle2;
  theta = acos(1 - 2*(Float)random()/(Float)MAXLONG); 
  angle2 = 2*M_PI*(Float)random()/(Float)MAXLONG;
  randomdir.x = sin(theta)*cos(angle2);
  randomdir.y = sin(theta)*sin(angle2);
  randomdir.z = cos(theta);

  VecNormalize(&randomdir);
  return randomdir;
}



/* 
 * The following function generate non-uniform distributed pointset.
 * The point will be dense at the two poles and sparse at the equator.
 */
Vector TwoPole()
{
  Vector randomdir;
  Float t, phine, tmp;
  
  t = 2.0*(Float)random()/(Float)MAXLONG - 1;
  phine = 2.0*M_PI*(Float)random()/(Float)MAXLONG;
  tmp = sqrt(1.0-t*t);
  
  randomdir.x = tmp*cos(phine);
  randomdir.y = tmp*sin(phine);
  randomdir.z = t;

  /* There is no need to normalize the vector, it is normalized inborn */
  VecNormalize(&randomdir);
  return randomdir;
}




/*
 * Hammersley point sets. Deterministic and look random.
 * Base p = 2, which is especially fast for computation.
 * Input parameter is total number of samples
 */
Vector SphereHammersley(int n) 
{
  float p, t, st, phi, phirad;
  int kk;
  static int k=1, prevn=-1, realn;
  static float perturbpercent=0.3;
  Vector result;
  
  if (prevn!=n)
  {
    prevn = n;
    /* Introduce some randomness into n, so the sample pattern is not always the same */
    realn = prevn + (int)(n*perturbpercent*(random()/(Float)MAXLONG - 0.5)); 
    k = 1;
  }
  if (k>=realn)
    k = 1;
    
  t = 0;
  for (p=0.5, kk=k ; kk ; p*=0.5, kk>>=1)
    if (kk & 1)                           /* kk mod 2 == 1 */
      t += p;
  t = 2.0 * t  - 1.0;                     /* map from [0,1] to [-1,1] */

  phi = (k + 0.5) / realn;                /* a slight shift */
  phirad =  phi * 2.0 * M_PI;             /* map to [0, 2 pi) */

  st = sqrt(1.0-t*t);
  result.x = st * cos(phirad);
  result.y = st * sin(phirad);
  result.z = t;
  k++; /* incremental the counter */
  return result;
}



/*
 * Halton point set generation
 * two p-adic Van der Corport sequences
 * Useful for incremental approach.
 * p1 = 2(default), p2 = 3(default)
 */
Vector SphereHalton2(int n, int p1, int p2)
{
  float p, t, st, phi, phirad, ip;
  int kk, a;
  static int prevn=-1, k=1;
  Vector result;

  if (prevn!=n)
  {
    prevn = n;
    k = 1;
  }
  if (k>=prevn)
    k = 1;
  
  t = 0;
  ip = 1.0/p1;                           /* recipical of p1 */
  for (p=ip, kk=k ; kk ; p*=ip, kk/=p1)  /* kk = (int)(kk/p1) */
    if ((a = kk % p1))
      t += a * p;
  t = 2.0 * t  - 1.0;                    /* map from [0,1] to [-1,1] */
  st = sqrt(1.0-t*t);

  phi = 0;
  ip = 1.0/p2;                           /* recipical of p2 */
  for (p=ip, kk=k ; kk ; p*=ip, kk/=p2)  /* kk = (int)(kk/p2) */
    if ((a = kk % p2))
      phi += a * p;
  phirad =  phi * 4.0 * M_PI;            /* map from [0,0.5] to [0, 2 pi) */

  result.x = st * cos(phirad);
  result.y = st * sin(phirad);
  result.z = t;
  k++;
  return result;
}
