/*   cover.h
**
**   This file contains definitions common to all modules (or many of
**   them). This file should be included in *every* module.
**
*/

#ifndef _cover_
#define _cover_


#include <stdio.h>
#include <limits.h>

#define maxv 40
/* binomial coefficients are tried to calculate up to binCoef[maxv][?]
 * (overflow is checked and the program is not halted 
 */

#define min(X,Y) ((X) < (Y) ? (X) : (Y))


#define randomize() srandom((int)time(NULL))
/* initialize random number generator */

#define setPRNGseed(X) srandom(X)
/* give the seed X for the random number generator */

#define rnd(num) (random() % (num))
/* gives an integer random number between 0 and num - 1, inclusive */

#define MAXRANDOM INT_MAX
#define random01() ((double) random() / MAXRANDOM)
/* gives a floating point random number in [0,1) */


typedef unsigned rankType;
/* this type should be able to contain binCoef[v][k] and binCoef[v][m] */

typedef unsigned short coveredType;
/* this type should be able to contain maxkSetCount. Keep it unsigned, if
   you want the definition of maxkSetCount to continue to work. */

#define maxkSetCount ((((unsigned long) 1) << (8 * sizeof(coveredType))) - 1)
/* maximum number of k-sets. If your type `char' is different from 8 bits 
   or you have some other problems, replace this with the absolute number.
   See also `coveredType' above. */

typedef unsigned binCoefType;
/* this type should be able to contain binomial coefficients needed */

typedef unsigned char varietyType;
/* this type should be able to contain maxv + 1 (!) */

typedef int costType;
/* this type should be able to contain maximum cost */

typedef int costDType;
/* this type should be able to contain the difference between any two
   solutions */

extern int v, k, t, m, b; /* global parameters */
extern int localOpt, exhaust, pdoFlag;
extern float coolFact, initProb;
extern int check;
extern int testCount;
extern int restrictedNeighbors;
extern char resultFileName[];
extern char logFileName[];
extern float initialT;
extern int frozen;
extern int Tset;
extern int bcount;
extern int Lset;
extern int L;
extern float LFact;
extern int endLimit;
extern int apprexp;
extern int onTheFly;
extern int coverNumber;
extern int solX;
extern long unsigned memoryLimit;
extern int searchB;
extern float SBFact;
extern int pack;
extern int check;
extern int verbose;
extern unsigned int PRNGseed;

/* functions common to all modules */

extern void coverError(int errno);

/* error numbers */

#define BINCOEF_OVERFLOW       1
#define INTERNAL_OVERFLOW      2
#define INVALID_PARAMETERS     3
#define V_TOO_LARGE            4
#define MEM_ALLOC_ERROR        5
#define SEE_ABOVE_ERROR        6
#define B_TOO_LARGE            7
#define TOO_MUCH_SPACE         8
#define INTERNAL_ERROR         9
#define RANKTYPE_OVERFLOW     10
#define CHECK_SOLUTION_ERROR  11

#endif  /* #ifndef _cover_ */
