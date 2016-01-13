/*
** exp.c
**
** Use the function calculate_exps() to initialize the exponentiation
** table.
**
*/

#include <math.h>
#include "cover.h"
#include "exp.h"

double exps[EXPSIZE + 1];
double neglibleExp;

void calculate_exps(void)
{
  double neglibleProb;
  int i;

  neglibleProb = 1.0 / EXPSIZE;
  neglibleExp = -log(neglibleProb);
  for(i = 0; i <= EXPSIZE; i++)
    exps[i] = exp((double) -i / EXPSIZE * neglibleExp);
}
