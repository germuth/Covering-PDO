/*   cover.c
**
**   This program tries to find covering designs using simulated annealing.
**
**   Written by Kari J. Nurmela at the Digital Systems Laboratory at
**   Helsinki University of Technology.
**
**   Copyright (c) 1993 Digital Systems Laboratory, Helsinki University of
**   Technology.
**
**   This program can be freely used whenever the following stipulations
**   are followed:
**
**   1) This program may be used for research only. No commercial use is
**      allowed.
**
**   2) Whenever the program has been used to obtain results that are
**      published, a reference should be made to the report
**      `Constructing Covering Designs by Simulated Annealing' by
**      Kari J. Nurmela and Patric R. J. \"Osterg\aa rd. (\LaTeX)
**      Helsinki University of Technology, Digital Systems Laboratory,
**      Series B: Technical Reports, No. 10, January 1993, ISSN 0783-540X,
**      ISBN 951-22-1382-6.
**
**   3) Bugs in the program should be reported to Kari.Nurmela@hut.fi or
**      Patric.Ostergard@hut.fi. We also appreciate if improvements and
**      new features are sent to the same e-mail addresses.
**
*/


/*
**
**   This file contains the main program. When porting to another system,
**   see file "cover.h" for some system-spesific definitions. The recording
**   of the CPU-time in this file must perhaps be modified, too, when
**   porting to non-unix systems.
**
*/

//coverings - what m-sets are covered by each k-set
//coverings[k][m]
//covered - how many times each m-set is covered
//covered[m]

#include <stdio.h>
//changed to help OSX compilation?
//#include <malloc/malloc.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <math.h>
#include "cover.h"
#include "bincoef.h"
#include "tables.h"
#include "anneal.h"
#include "exp.h"
#include "exhaustive.h"


float coolFact=0.99, initProb=0.5;
//int v=4, k=2, t=2, m=2, b=6;
int v=5, k=3, t=2, m=2, b=4;
int testCount = 1;
int restrictedNeighbors = 0;
float initialT = 1.0;
int frozen = 10;
int endLimit = 0;
int apprexp = 0;
int Tset = 0;
int L = 24;
int Lset = 0;
float LFact = 1.0;
int localOpt = 0;
int exhaust = 1; //lets enable by default for now
int onTheFly = 0;
int coverNumber = 1;
char resultFileName[100] = {'c','o','v','e','r','.','r','e','s','\0'};
char logFileName[100] = {'c','o','v','e','r','.','l','o','g','\0'};
int solX = 0;
long unsigned memoryLimit = 0;
int searchB = 0;
float SBFact = 0.95;
int pack = 0;
int check = 1;
int verbose = 1;
unsigned int PRNGseed;

/*
** printSolution prints the current solution.
**
*/

void printSolution(FILE *fp) {
  int i,j;
  varietyType set[maxv + 1];
  varietyType *vptr;

  for(j = 0; j < b; j++) {
    printSubset(fp, kset[j], k);
    fprintf(fp, "\n");
  }
}


/*
** coverError prints an error message and exits the program
**
*/

void coverError(int errno)
{
  static char *errmsg[] = {
    "No Error",
    "Binomial coefficient overflow",
    "Internal overflow",
    "Invalid parameters",
    "Parameter v too large, not enough space reserved",
    "Memory allocation error",
    "(See above)",
    "Parameter b is larger than maxkSetCount",
    "Space demands exceed the limit given by MemoryLimit option",
    "Internal error. Make a bug report.",
    "RankType is too small to contain the binomial coefficients needed.",
    "Cost change calculation gives wrong result.",
  };

  fprintf(stderr, "ERROR: %s\n", errmsg[errno]);
  exit(errno);
}


/*
** newBAfterSuccess gradually decreases b (for option SearchB=1)
**
*/

static int newBAfterSuccess(int oldB)
{
  int lb;

  lb = (int) (SBFact * oldB + 0.5);
  if(lb == oldB)
    lb--;
  return lb;
}


/*
** searchBContinues calculates the new b, if needed (for option
** SearchB = 1)
**
*/


static int newSplitB(int b, int hi, int lo, int found)
{
  int hlp;

  if(hi - lo <= 1)
    return 0;
  bIs(lo + (hi - lo + 1) / 2);
  return 1;
}

static int searchBContinues(int found, int *hiB, int *loB)
{
  if(!searchB)
    return 0;
  if(*loB == -1) /* no failed yet */
    if(found) {
      *hiB = b;
      bIs(newBAfterSuccess(b));
      return 1;
    }
    else {
      *loB = b;
      return newSplitB(b, *hiB, *loB, found);
    }
  else {
    if(found)
      *hiB = b;
    else
      *loB = b;
    return newSplitB(b, *hiB, *loB, found);
  }
}


/*
** printParams prints the parameters given to the program.
**
*/

void printParams(FILE *fp)
{
  fprintf(fp, "Design parameters:\n"
	  "------------------\n"
	  "t - (v,m,k,l) = %d - (%d,%d,%d,%d)\nb = %d\n\n",
	  t, v, m, k, coverNumber, b);
  fprintf(fp, "Optimization parameters:\n"
	  "------------------------\n"
	  "TestCount     = %d\nCoolingFactor = %.4f\n", testCount, coolFact);
  if(Tset)
    fprintf(fp, "InitTemp      = %.3f\n", initialT);
  else
    fprintf(fp, "InitProb      = %.2f\n", initProb);
  fprintf(fp, "frozen        = %d\n"
	  "RestrNeighbor = %d\n", frozen, restrictedNeighbors);
  if(Lset)
    fprintf(fp, "L             = %d\n", L);
  else
    fprintf(fp, "LFact         = %.2f\n", LFact);
  fprintf(fp, "EndLimit      = %d\n"
    "local         = %d\n"
    "exhaustive    = %d\n"
    "apprexp       = %d\n"
	  "OntheFly      = %d\nPack          = %d\n"
	  "log           = %s\nresult        = %s\nSolX          = %d\n"
	  "verbose       = %d\nMemoryLimit   = %lu\n"
	  "check         = %d\n\n", endLimit, localOpt, exhaust, apprexp, onTheFly,
	  pack, logFileName, resultFileName, solX, verbose, memoryLimit,
	  check);
  fflush(fp);
}


/*
** Here is the main program.
**
*/

int main(int argc, char **argv) {
  //test stuff
  costType retVal;
  int j, i, count, bcounter;
  int iterSum;
  struct rusage before, after;
  costType costSum, finalCost = 0, costSquareSum;
  float CPU, CPUsum;
  int solFound = 0;
  int bestCost = -1;
  FILE *logFp, *resFp;
  int hiB = -1, loB = -1;
  float costSD = 0.;

  randomize();

  parseArguments(argc, argv);

  if(verbose)
    printf("\n"
	   "cover 1.0a - find covering designs using simulated annealing\n"
	   "============================================================\n\n");
  logFp = fopen(logFileName, "w");
  if(!logFp) {
    fprintf(stderr, "Can't open log file %s.\n", logFileName);
    coverError(SEE_ABOVE_ERROR);
  }
  calculateBinCoefs();   /* compute tables for binomial coefficients */
  calculate_exps();      /* and approximate exponentiation           */
  if(!Lset)
    L = (int) (LFact * k * (v - k) * b + 0.5);
    /* L = LFact * neighborhood size, if not spesified otherwise */
  if(searchB)
    bIs(newBAfterSuccess(hiB = b));
  else
    bIs(b);          /* number of k-sets */

  if(verbose && !exhaust)
    printParams(stdout);
  printParams(logFp);
  fprintf(logFp, "\nRuns:\n-----\n");

  CPUsum = 0.0;
  costSum = costSquareSum = 0;
  iterSum = 0;


  /*
  varietyType ss[3 + 1];
  getFirstSubset(ss, 3);
  do {
    for(int j = 0; j < 4; j++) {
        printf("%d ", ss[j]);
    }
    printf("\n");
  }while( getNextSubset(ss, 3, 5) );
  printf("done\n");
  */
  //neighbour and cover tables
  computeTables(t, k, m, v);       /* compute tables for this design */

  do {
    for(count = 0; count < testCount; count++) {
      iterCounter = 0;
      getrusage(RUSAGE_SELF, &before);
      if(exhaust){
        initSolution();
        finalCost = bruteforce();
      }else if(localOpt){
        finalCost = localOptimization(L, endLimit);
      }else {
        finalCost = simulatedAnnealing(coolFact, initProb, L, frozen, endLimit);
      }
      if(finalCost <= endLimit) {
        solFound = 1;
        sortSolution();
        count = testCount; /* need no more runs */
      }
      if(bestCost == -1 || finalCost < bestCost) {
	       bestCost = finalCost;
      }
      getrusage(RUSAGE_SELF, &after);
      CPU = after.ru_utime.tv_sec + after.ru_utime.tv_usec / 1000000.0 -
	     (before.ru_utime.tv_sec + before.ru_utime.tv_usec / 1000000.0);
      costSum += finalCost;
      costSquareSum += finalCost * finalCost;
      if(verbose)
	printf("Result:\n"
	       "-------\n"
	       "finalCost     = %d\n"
	       "CPU-time      = %.2f\n"
	       "Iterations    = %d\n"
	       "EndTemp       = %.3f\n\n",
	       finalCost, CPU, iterCounter, endT);
      fprintf(logFp, "cost          = %d\n", finalCost);
      fflush(logFp);
      if(verbose)
	if(finalCost <= endLimit) {
	  printf("Solution:\n"
		 "---------\n");
	} else {
	  printf("EndLimit was not reached.\n\n");
	  if(verbose >= 2) {
	    printf("Inadequate solution:\n"
		   "--------------------\n");
	    printSolution(stdout);
	  }
	}
      if(finalCost <= endLimit) {
	if(verbose)
	  printSolution(stdout);
	resFp = fopen(resultFileName, "w");
	if(!resFp) {
	  fprintf(stderr, "Can't open file %s.\n", resultFileName);
	  coverError(SEE_ABOVE_ERROR);
	}
	printSolution(resFp);
	fclose(resFp);
      }
      if(check)
	if(checkSolution() != finalCost)
	  coverError(CHECK_SOLUTION_ERROR);
	else {
	  if(verbose)
	    printf("Final cost check OK.\n\n");
	}
      else
	if(verbose)
	  printf("\n");
      CPUsum += CPU;
      iterSum += iterCounter;
    } /* for(count... */
  } while(searchBContinues(finalCost<=endLimit, &hiB, &loB));
  if(testCount > 1)
    costSD = sqrt((float) costSquareSum / (testCount - 1) -
		  (float) costSum * (float) costSum / (float) testCount /
		  (float) (testCount - 1));
  if(verbose) {
    printf("Statistics:\n"
	   "-----------\n"
	   "averageCost   = %.3f\n"
	   "av. CPU-time  = %.2f\n"
	   "av.iterations = %.2f\n"
	   "bestCost      = %d\n",
	   (float) costSum / (float) testCount,
	   CPUsum / (float) testCount,
	   (float) iterSum / (float) testCount,
	   bestCost);
    if(testCount > 1) {
      printf("costStandardDeviation = %.3f\n", costSD);
    }
  }
  fprintf(logFp, "\nStatistics:\n-----------\nbestCost      = %d\n"
	  "CPU-time      = %.2f\n", bestCost, CPUsum);
  if(testCount > 1)
    fprintf(logFp, "costStandardDeviation = %.3f\n", costSD);

  fflush(stdout);
  fflush(logFp);

  freeTables();
  return !solFound; /* returns 0 if a solution was found */
}
