/*   anneal.c
**
**   This file contains the functions in simulated annealing process.
**
*/


#include <math.h>
#include <sys/time.h>
#include <string.h>
#include "cover.h"
#include "bincoef.h"
#include "tables.h"
#include "exp.h"


int iterCounter = 0;
float endT;

/*
** `setNumber' is the index to the table `kset'. `setNumber' indicates
** the index of the k-set in the proposed change. The current solution
** can be changed to the proposed next solution by assigning
** kset[setNumber] = nextS. `stored', `storedPtr', `currSto' and
**`nextSto' are for on-the-fly annealing. `costs[x]' holds the difference
** of costs associated with a m-set covered x times and x+1 times.
**
*/

static setNumber;
static rankType nextS;
static rankType stored[2];
static int currSto, nextSto;
rankType *storedPtr[2];


/*
** TODO This shouldn't be in anneal, this is used in all methods
** `calculateCosts()' calculates the costs and cost differences in the
** tables `costs' and `costds' to be used when calculating the costs of
** the solutions.
**
** Populates costs array
** Contains the punishment for how many times it was covered
** For example costs[0] is how much cost is added to a solution if you cover
** a single m-set 0 times
** costs[3] is if you cover it 3 times
** if lambda is 1 we would expect [1,0,0,0,0,0,0,0]
**  for a covering design (there is no cost for over-covering)
*/

void calculateCosts(void) {
  int i;

  if(pack) /* packing design */
    for(i = 0; i <= b; i++)
      if(i < coverNumber)
	costs[i] = (costType) 0;
      else
	costs[i] = (costDType) (i - coverNumber);
  else     /* covering design */
    for(i = 0; i <= b; i++)
      if(i < coverNumber)
	costs[i] = (costType) (coverNumber - i);
      else
	costs[i] = 0;
  for(i = 0; i < b; i++)
    costds[i] = costs[i] - costs[i + 1];
}



/*
** `initSolution' makes an initial solution by selecting `b' random k-sets.
** The table `covered' is initialized to zero for computing the initial
** solution. It then initiates the table `covered' to this initial solution
** and calculates the initial value of the cost function (zeros in `covered').
*/

static costType initSolution(void) {
  int i, j;
  costType initCost;
  costType P2plus = (costType) 0;
  coveredType *ptr;
  coveredType *cptr;
  rankType *coveringsPtr;

  if(restrictedNeighbors)
    setNumber = 0;
  for(i = 0; i < 2; i++)
    stored[i] = binCoef[v][k];
  nextSto = currSto = -1;
  for(i = binCoef[v][m], cptr = covered; i > 0; i--)
    *cptr++ = (coveredType) 0;
  for(i = 0; i < b; i++) {
    kset[i] = rnd(binCoef[v][k]);
    if(onTheFly) {
      calculateOneCovering(kset[i], coverings);
      coveringsPtr = coverings;
    } else {
      coveringsPtr = coverings + (int) kset[i] * coverLen;
    }
    for(j = 0; j < coverLen - 1; j++) {
      covered[coveringsPtr[j]]++;
    }
  }
  for(i = 0, initCost = (costType) 0, ptr = covered; i < coveredLen;
      i++, ptr++)
    initCost += costs[*ptr];

  if(verbose) {
    printf("initCost      = %d\n", initCost);
  }
  return initCost;
}

/*
** compareVarieties is needed for qsort int randomNeighbor()
**
*/

int compareVarieties(varietyType *a, varietyType *b) {
  if(*a < *b)
    return -1;
  else {
    if(*a > *b)
      return 1;
    else
      return 0;
  }
}


/*
** randomNeighbor() computes the rank of a random neigbor of the k-set with
** rank `curr'.
*/

static rankType randomNeighbor(rankType curr)
{
  varietyType subset[maxv + 1];
  varietyType csubset[maxv + 1];

  unrankSubset(curr, subset, k);
  makeComplement(subset, csubset, v);
  subset[rnd(k)] = csubset[rnd(v-k)];
  qsort((char *) subset, k, sizeof(varietyType), compareVarieties);
  return rankSubset(subset, k);
}


/*
** computeNeighbor() calculates the cost difference between the current
** solution and a random neighbor of the current solution. It employs
** the sentinels at the end of the covered sets of each k-set. (That's
** why they were put there in calculateNeighbors().)
**
*/

costType computeNeighbor(void)
{
  costType costDelta = 0;

  int i;
  rankType currS;
  rankType *currPtr, *nextPtr;

  if(restrictedNeighbors) {
    if(++setNumber == b)
      setNumber = 0;
  }
  else
    setNumber = rnd(b);

  currS = kset[setNumber];
  nextS = onTheFly ? randomNeighbor(currS) :
    neighbors[currS * neighborLen + rnd(neighborLen)];

  if(onTheFly) {
    currPtr = NULL;
    for(i = 0; i < 2; i++)
      if(currS == stored[i]) {
          currPtr = storedPtr[i];
          currSto = i;
      }
    nextPtr = NULL;
    for(i = 0; i < 2; i++)
      if(nextS == stored[i]) {
          nextPtr = storedPtr[i];
          nextSto = i;
      }
    for(i = 0; !currPtr; i++)
      if(nextSto != i) {
          storedPtr[i] = currPtr = coverings + i * coverLen;
          currSto = i;
          stored[i] = currS;
          calculateOneCovering(currS, currPtr);
      }
    for(i = 0; !nextPtr; i++)
      if(currSto != i) {
          storedPtr[i] = nextPtr = coverings + i * coverLen;
          nextSto = i;
          stored[i] = nextS;
          calculateOneCovering(nextS, nextPtr);
      }
  }
  else {
    currPtr = coverings + currS * coverLen;
    nextPtr = coverings + nextS * coverLen;
  }

  for(i = 0; i < (coverLen - 1) * 2; i++)
    if(*currPtr == *nextPtr) {
      currPtr++;
      nextPtr++;
      i++;
    }
    else if(*currPtr < *nextPtr) {
      costDelta += costds[covered[*currPtr++] - 1];
    }
    else
      costDelta -= costds[covered[*nextPtr++]];

  return costDelta;
}


/*
** acceptNeighbor() changes the current solution to the latest solution
** computed.
*/

void acceptNeighbor(void)
{
  int i;
  rankType currS;
  rankType *coveringsPtr;

  currS = kset[setNumber];
  if(onTheFly)
    coveringsPtr = coverings + currSto * coverLen;
  else
    coveringsPtr = coverings + currS * coverLen;
  for(i = 0; i < coverLen - 1; i++)
    covered[coveringsPtr[i]]--;
  if(onTheFly)
    coveringsPtr = coverings + nextSto * coverLen;
  else
    coveringsPtr = coverings + nextS * coverLen;
  for(i = 0; i < coverLen - 1; i++)
    covered[coveringsPtr[i]]++;
  kset[setNumber] = nextS;
}


/*
** approxInitT() tries to find a good initial value for T, so that the
** probability of accepting a cost increasing move is approximately the
** probability set by the user. T_ITER is the count of iterations when
** calculating the initial temperature from `initProb'. T_LIFESAVER is
** the initial temperature, if no cost increasing moves are found during
** the iteration.
**
*/

#define T_LIFESAVER 1.0
#define T_ITER 300

static float approxInitT(void)
{
  float T;
  int i, m2;
  costType costDelta;

  T = 0.0;
  m2 = 0;
  for(i = 0; i < T_ITER; i++) {
    costDelta = computeNeighbor();
    if(costDelta > 0) {
      m2++;
      T += -costDelta;
    }
  }
  if(m2 == 0)
    T = T_LIFESAVER;
  else
    T = T / m2 / log(initProb);
  return T;
}


/*
** simulatedAnnealing() is the algorithm. `coolFact' is the cooling
** factor, `initProb' the wanted initial probability for accepting a
** cost increasing move, `frozen' is the count of successive temperatures
** without cost decrease, before the iteration is considered frozen.
** `iterLenght' is number of iterations performed at each temperature.
**
*/

#define T_PRINT_ITER 300

costType simulatedAnnealing(double coolFact, double initProb,
			    int iterLength, int frozen, int endLimit)
{
  float deltaF_;
  float r, D;
  costType costDelta, actCost, currCost, lastCost, bestSeen;
  int notChanged = 0, i, j, k2, m1, m2, m3, m0, l;
  varietyType set[maxv + 1];
  float T;

  if(verbose)
    printf("Starting annealing...\n\n");
  calculateCosts();
  currCost = initSolution();
  bestSeen = lastCost = currCost;

  if(Tset)
    T = initialT;                         /* T was given as a parameter */
  else
    T = approxInitT();

  /* tests the probability for cost increasing moves */
  if(verbose) {
    m1 = m2 = 0;
    deltaF_ = 0.0;
    for(i = 0; i < T_PRINT_ITER; i++) {
      costDelta = computeNeighbor();
      if(costDelta > 0) {
	m1++;
	if(random01() < ExpProb(costDelta / T))
	  m2++;
      }
    }
    if(m1 == 0) {
      m1 = 1;
      m2 = 0;
    }
    printf("initial inc%%  = %.2f\n\n", (double) m2 / (double) m1);
  }

  if(verbose >= 2)
    printf("      T      cost   best   inc%%   tot%%   0-m%%\n"
	   "    ------------------------------------------\n");
  while(notChanged < frozen) {
    m1 = m2 = m3 = m0 = 0;
    for(i = 0; i < iterLength; i++) {
      costDelta = computeNeighbor();
      iterCounter++;
      if(costDelta <= 0) {
	m3++;
	acceptNeighbor();
	currCost += costDelta;
	if(currCost <= endLimit) {
	  endT = T;
	  if(verbose >= 2)
	    printf("\n");
	  if(verbose)
	    printf("...annealing accomplished.\n\n");
	  return currCost;       /* a good enough final solution was found */
	}
	if(costDelta < 0) {
	  notChanged = 0;
	  if(currCost < bestSeen)
	    bestSeen = currCost;
	}
	else
	  m0++;
      }
      else {
	r = random01();
	D = costDelta / T;
	if(r < ExpProb(D)) {
	  acceptNeighbor();
	  m1++;
	  currCost += costDelta;
	}
	else
	  m2++;
      }
    }
    if(lastCost <= currCost)
      notChanged++;
    lastCost = currCost;
    if(m2 == 0)
      m2 = 1; /* prevent division by zero */
    if(verbose >= 2)
      printf("    %5.2f   %4d   %4d    %4.1f   %4.1f   %4.3f\n",
	     T, currCost, bestSeen, (double) m1 / (double) (m1 + m2) * 100.0,
	     (double) (m3 + m1) / (double) (m1 + m2 + m3) * 100.0,
	     (double) (m0) / (double) (m1 + m2 + m3));
    T *= coolFact;
  }
  endT = T;
  if(verbose >= 2)
    printf("\n");
  if(verbose)
    printf("...annealing accomplished.\n\n");
  return currCost;
}


/*
** `localOptimization()' is a separate function for efficiency reasons. It
** performs the local optimization or hill-climbing procedure.
**
*/

costType localOptimization(int frozen, int endLimit)
{
  costType costDelta, currCost;
  int notChanged = 0, i, found;

  if(verbose >= 2)
    printf("Starting local optimization...\n");
  calculateCosts();
  currCost = initSolution();
  do {
    while(notChanged < frozen) {
      costDelta = computeNeighbor();
      iterCounter++;
      if(costDelta < 0) {
	acceptNeighbor();
	notChanged = 0;
	currCost += costDelta;
	if(currCost <= endLimit)
	  return currCost;       /* a good enough final solution was found */
      }
      else
	notChanged++;
    }
    found = 0;
    if(!onTheFly)
      for(i = 0; i < neighborLen && !found; i++)
	/* check a neighbor solution */;
  } while(found);
  if(verbose >= 2)
    printf("...local optimization accomplished.\n\n");
  return currCost;
}
