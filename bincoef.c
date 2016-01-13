/*   bincoef.c
**
**   This file contains the binomial coefficient table and needed routines
**   to calculate it and to use it.
**
*/

#include "cover.h"
#include "bincoef.h"

binCoefType binCoef[maxv + 1][maxv + 2];  /* the binomial coefficient table */


/*
** The binomial coefficient table is calculated with the well know method by
** summing 2 numbers on the previous line. Note that binCoef[x][x + 1] is
** set to 0. In case of overflow the entry in the table is set to 0. We use
** 0 instead of, say -1, in order to permit unsigned binCoefType, even
** though it brings some labour.
** 
*/

void calculateBinCoefs(void)
{
  int v,k;

  for(v = 0; v <= maxv; v++) {
    binCoef[v][0] = binCoef[v][v] = 1;
    binCoef[v][v+1] = 0;
    for(k = 1; k <= v - 1; k++) {
      binCoef[v][k] = binCoef[v - 1][k - 1] + binCoef[v - 1][k];
      if(binCoef[v][k] < binCoef[v - 1][k - 1] ||
	 binCoef[v][k] < binCoef[v - 1][k] ||
	 binCoef[v - 1][k - 1] == 0 ||
	 (binCoef[v - 1][k] == 0 && k < v))
	binCoef[v][k] = 0; /* there was an overflow */
    }
  }
}
