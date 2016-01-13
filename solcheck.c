/*
** solcheck.c
**
** This file contains the routines for checking the coverability of the
** current solution. Only for debugging purposes.
**
*/


#include "cover.h"
#include "setoper.h"
#include "tables.h"
#include "bincoef.h"


/*
** commonElements() computes the intersection of two sets.
**
*/

static int commonElements(rankType mRank, rankType kRank)
{
  varietyType mSet[maxv + 1], kSet[maxv + 1];
  varietyType *mPtr, *kPtr;
  int matchCount = 0, i;

  unrankSubset(mRank, mSet, m);
  unrankSubset(kRank, kSet, k);
  mSet[m] = kSet[k] = maxv; /* sentinel */
  mPtr = mSet;
  kPtr = kSet;
  for(i = 0; i < k + m; i++)
    if(*mPtr == *kPtr) {
      matchCount++;
      mPtr++;
      kPtr++;
      i++;
    }
    else if(*mPtr > *kPtr)
      kPtr++;
    else
      mPtr++; /* *mPtr < *kPtr */
  return matchCount;
}


/*
** checkSolution() runs through all the m-sets and calculates the cost
** directly. This cost value can be used in debugging purposes to check
** that the initial cost and the cost change calculation have worked
** properly.
**
*/

costType checkSolution(void)
{
  rankType mRank;
  int i, found, foundAll = 1;
  int setCount = 0;
  costType totalCost = 0;

  for(mRank = 0; mRank < binCoef[v][m]; mRank++) {
    found = 0;
    for(i = 0; i < b; i++)
      if(commonElements(mRank, kset[i]) >= t)
	found++;
    totalCost += costs[found];
    if(verbose >= 2 && costs[found]) {
      printf("Nonzero cost: ");
      printSubset(stdout, mRank, m);
      printf(" (cost %d)\n", costs[found]);
    }
  }
  return(totalCost);
}
