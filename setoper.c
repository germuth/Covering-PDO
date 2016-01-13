/*   setoper.c
**
**   This file contains functions needed for subset ranking and unranking
**   plus making a complement of a given subset.
**
*/


#include "cover.h"
#include "bincoef.h"
#include "setoper.h"


/*
** `rankSubset' calculates a unique rank for a given subset with a given
** cardinality. Varieties in the subset are numbered beginning from 0.
**
*/

rankType rankSubset(varietyType *subset, int card)
{
  int i;
  rankType rank = 0;

  for(i = 0; i < card; i++)
    rank += binCoef[*subset++][i + 1];
  return rank;
}


/*
** `getFirstSubset' gets the subset with rank 0 and the given cardinality.
**
*/

void getFirstSubset(varietyType *subset, int card)
{
  int i;

  for(i = 0; i < card; i++)
    *subset++ = i;
  *subset = maxv + 1; /* sentinel */
}


/*
** `getNextSubset' gets the subset that has rank one bigger than the rank
** of the given subset. `v' is the number of varieties. Varieties range
** from 0 to v - 1. It returns 0, if this was the greatest possible rank
** for this type subset, otherwise the return value is 1.
*/

int getNextSubset(varietyType *subset, int card, int v)
{
  int i,j;

  if(subset[0] >= v - card)
    return 0;
  else {
    j = 0;
    while(subset[j + 1] <= subset[j] + 1)
      j++;
    subset[j]++;
    for(i = 0; i < j; i++)
      subset[i] = i;
    return 1;
  }
}


/*
** `unrankSubset' makes a subset out of a rank.
**
*/

void unrankSubset(rankType rank, varietyType *subset, int card)
{
  int p, m, i;

  m = rank;
  for(i = card - 1; i >= 0; i--) {
    p = i;
    while(binCoef[p + 1][i + 1] <= m)
      p++;
    m -= binCoef[p][i + 1];
    subset[i] = p;
  }
}


/*
** `makeComplement' forms the complement of a given subset `s' with a
** cardinality `c'. The number of varieties is `v'.
**
*/

void makeComplement(varietyType *s, varietyType *c, int v)
{
  int i;

  for(i = 0; i < v; i++)
    if(*s == (varietyType) i)
      s++;
    else
      *c++ = (varietyType) i;
  *c = maxv + 1; /* sentinel */
}


/*
** printSubset prints a subset of a given cardinality in a format given by
** the command-line parameters.
*/

void printSubset(FILE *fp, rankType r, int card)
{
  varietyType set[maxv + 1], *vptr;
  int i;

  unrankSubset(r, set, card);
  if(solX)
    for(i = 0, vptr = set; i < v; i++)
      if(*vptr == i) {
	fprintf(fp, "X");
	vptr++;
      }
      else
	fprintf(fp, "-");
  else
    for(i = 0; i < card; i++)
      fprintf(fp, "%d ", set[i]);
}
