#include "cover.h"
#include "bincoef.h"
#include "tables.h"
#include "exp.h"
#include <math.h>
#include <stdlib.h>

//always accept sideways moves
//moves at the same level are counted together towards the number of moves needed to find better solution
//initialize jdf to 10*neighbourhood-size
//update jdf = ceil[ n*jdf + newestAmountOFMoves / n+1 ]
//try 2*jdf amount of moves

int *jdf;
int *jdc;
int currCost;
int counter;

static void updateJDF(void){
    double ans = jdc[currCost] * jdf[currCost] + counter;
    ans /= (double)(jdc[currCost] + 1);

    jdf[currCost] = (int)ceil(ans);
}

static costType initSolution(void) {
    int i, j;
    costType initCost;
    coveredType *ptr;
    rankType *coveringsPtr;


    for(i = 0; i < b; i++) {
        if(randomStartFlag){
            kset[i] = rnd(binCoef[v][k]);
        }else{
            //TODO hardcoded for k=5
            int v1,v2,v3,v4,v5;
            scanf("%d %d %d %d %d",&v1,&v2,&v3,&v4,&v5);
            varietyType *block = calloc(6, sizeof(int));
            block[0] = v1 - 1;
            block[1] = v2 - 1;
            block[2] = v3 - 1;
            block[3] = v4 - 1;
            block[4] = v5 - 1;
            block[5] = maxv + 1;
            kset[i] = rankSubset(block, k);
        }
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

    return initCost;
}

static void printProgress(void){
    printf("\rjdc\t\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
    jdc[0], jdc[1], jdc[2], jdc[3], jdc[4], jdc[5], jdc[6], jdc[7], jdc[8], jdc[9]);
    printf("\rjdf\t\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
    jdf[0], jdf[1], jdf[2], jdf[3], jdf[4], jdf[5], jdf[6], jdf[7], jdf[8], jdf[9]);
    printf("\033[F");
}
costType pdo() {
    int i;
    int maxCost = 0;

    //costs[lambda] holds costs for covering a single m-set lambda times
    calculateCosts();
    for(i = 0; i < b; i++){
        if(costs[i] > maxCost){
            maxCost = costs[i];
        }
    }
    //if the most we can be pushished for one m-set is max-cost, then the highest
    //possible cost is number of m-sets * max pushishment per m-set
    maxCost *= binCoef[v][m];

    //Jump Down Function: cost -> moves needed to find good neighbour
    jdf = calloc(maxCost, sizeof(int));
    for(i = 0; i < maxCost; i++){
        jdf[i] = 10 * neighborLen;
    }
    //Jump Down Count: jdc[i] = number of times we have jumped down from a solution at level i
    jdc = calloc(maxCost, sizeof(int));
    for(i = 0; i < maxCost; i++){
        jdc[i] = 1;
    }

    counter = 0;
    currCost = initSolution();

    //get random neighbour and its cost
    int costDifference = computeNeighbor();

    printf("level\t\t\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\n");
    printProgress();

    while( currCost > 0){
        counter++;
        if( costDifference > 0) { //if( neighborCost > currCost){
            if( counter > 2 * jdf[currCost] ){ //TODO parameter here
                //failed to find jump down from this level, accept bad neighbour anyway
                acceptNeighbor();
                counter = 0;
                currCost += costDifference;
            }else{
                //undo move

            }
        }else if(costDifference == 0) { //if( neighborCost == currCost ){
            acceptNeighbor();
            //counter keeps counting
        }else {
            updateJDF();
            jdc[currCost]++;//increment number of times we have jumped down
            acceptNeighbor();
            printProgress();
            counter = 0;
            currCost += costDifference;
        }
        //try random neighbour
        costDifference = computeNeighbor();
    }
    printf("\n\n\n");

    return 0;
}
