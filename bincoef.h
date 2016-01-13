/*   bincoef.h
**
**   This file contains definitions needed when calculating the binomial
**   coefficient table and when using that table.
**
*/


#ifndef _bincoef_
#define _bincoef_


extern binCoefType binCoef[maxv + 1][maxv + 2];
void calculateBinCoefs(void);

#define overflowBinCoef(v,k) (binCoef[(v)][(k)] == 0)

#endif
