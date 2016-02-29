/*
A cornputer program included in Bate's thesis described how one can construct, for
small parameters, a minimal (n, k, p, t) lottery design. The basic principle of this
cornputer program was to find a minimal (n, k, p, t) lottery design by considering al1
possible potential designs. The basis of this search program is given by the following
algorithm.
*/

#include <math.h>
#include <string.h>
#include "cover.h"
#include "bincoef.h"
#include "tables.h"
#include "exp.h"
/* returns true if design found*/
//checks if solution exists for current b
//must be called with each b, one by one
costType exhastive() {
  int bi;

  //start with random solution
  costType initialCost = initSolution();

  int index = 0;
  //get rank of first m-set not covered
  for(rankType mi = 0; mi < m; mi++){
      if(!covered[mi]){
          //this m-set not currently covered

      }
  }

  //if empty,
  //  return cost of 0, kset contains solution
  //else
  //  if b == kset.length
  //    return no design possible
  //  else
  //    find all k-sets that cover first missing p-set
  //    for all found k-sets
  //      try adding k-set to design
  //      costType solution = exhaustive();
  //      if solution == 0
  //        return solution;
  //      else
  //        remove k-set, try next one
  //    design not possible
  //

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
