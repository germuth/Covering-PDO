/*   setoper.h
**
**   This file contains declarations needed for subset ranking and unranking
**   plus making a complement of a given subset.
**
*/


#ifndef _setoper_
#define _setoper_


rankType rankSubset(varietyType *subset, int card);
void getFirstSubset(varietyType *subset, int card);
int getNextSubset(varietyType *subset, int card, int v);
void unrankSubset(rankType rank, varietyType *subset, int card);
void makeComplement(varietyType *s, varietyType *c, int v);
void printSubset(FILE *fp, rankType r, int card);

#endif
