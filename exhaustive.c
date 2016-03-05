/*
A cornputer program included in Bate's thesis described how one can construct, for
small parameters, a minimal (n, k, p, t) lottery design. The basic principle of this
cornputer program was to find a minimal (n, k, p, t) lottery design by considering al1
possible potential designs. The basis of this search program is given by the following
algorithm.
*/

#include "cover.h"
#include "bincoef.h"
#include "tables.h"
#include "exp.h"

int currB = 0;

static void addBlock(rankType currBlock){
    int j;
    kset[currB] = currBlock;
    currB++;
    rankType *coveringsPtr = coverings + (int) currBlock * coverLen;
    for(j = 0; j < coverLen - 1; j++) {
        covered[coveringsPtr[j]]++;
    }
}

static void removeBlock(rankType currBlock){
    int j;
    currB--;
    rankType *coveringsPtr = coverings + (int) currBlock * coverLen;
    for(j = 0; j < coverLen - 1; j++) {
        covered[coveringsPtr[j]]--;
    }
}

//looks at covered table to deterine solution cost
static costType calcSolutionCost() {
  int i;
  costType cost;
  coveredType *ptr;

  for(i = 0, cost = (costType) 0, ptr = covered; i < coveredLen;
      i++, ptr++)
    cost += costs[*ptr];

  if(verbose) {
    printf("Cost      = %d\n", cost);
  }
  return cost;
}

costType initSolution(void) {
  calculateCosts();
  //starting solution has only one block, currLength = 1
  addBlock((rankType) rnd(binCoef[v][k]));

  return calcSolutionCost();
  //return initCost;
}

/* returns true if design found*/
//checks if solution exists for current b
//must be called with each b, one by one
costType bruteforce() {
    rankType i, j, mi;
    costType cost;
    if(currB > b){ //too many blocks
        return 1;
    }
    if(currB == b){
        return checkSolution();
    }
    //get rank of first m-set not covered
    for(mi = 0; mi < binCoef[v][m]; mi++){
        //TODO could improve speed by counting the number of remaining uncovered sets
        //and test whether it is even possible to cover the remaining ones with how many blocks you have left
        if(!covered[mi]){
            //this m-set not currently covered

            //for each k-set
            for(i = 0; i < binCoef[v][k]; i++){
                //check if it covers this m-set
                for(j = 0; j < coverLen - 1; j++){
                    //   if(coverings[i][j] == mi)
                    if(coverings[i * coverLen + j] == mi){
                        //this k-set would cover our currently uncovered m-set
                        addBlock(i);
                        cost = bruteforce();
                        if( cost == 0){
                            return cost;
                        }
                        removeBlock(i);
                    }
                }
            }
            if(!covered[mi]){
                return 1; //if this m-set couldn't be covered
            }
        }
    }
    //all m-sets are covered, if all blocks are used
    return currB == 4;
}
/*
  Find first p-set not covered
  If all are represented then {
    Retun tme meanzng a design hm been found.
  } Else {

    If design contains maximum number of blocks allowed then {
      Return false to mean no design found
    } Else {
      For each k-set representing the p-set do {

        Add the k-set to the design.
        Flag every p-set represented by th6 k-set-
        Cal1 CenerateDesign and store its return value in
        variable named found.
        If found is true then {
          Return tme to mean a design has been found.
        } Else {
          unflag the p-sets.
          Remove k-set from design.
        }

      }

      Return false meaning not found.
    }
  }
}
*/
