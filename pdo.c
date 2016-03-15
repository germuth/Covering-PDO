#include <math.h>
#include <stdlib.h>
#include "cover.h"
#include "bincoef.h"
#include "tables.h"
#include "exp.h"

//always accept sideways moves
//moves at the same level are counted together towards the number of moves needed to find better solution
//initialize jdf to 10*neighbourhood-size
//update jdf = ceil[ n*jdf + newestAmountOFMoves / n+1 ]
//try 2*jdf amount of moves

int *jdf;
int *jdc;
int currCost;
int counter;
int maxCost = 0;
int printCount = 0;

static void updateJDF(void){
    /* WAS GETTING OVERFLOWS SOMETIMES
    double ans = jdc[currCost] * jdf[currCost] + counter;
    ans /= (double)(jdc[currCost] + 1);
    */
    double ans = jdf[currCost] + counter;
    ans /= (double)(jdc[currCost] + 1);
    ans *= (double) jdc[currCost]; //do the multiplication after division, not before

    jdf[currCost] = (int)ceil(ans);

    //only try up to a million times?
    if(pdoMaxJDF && jdf[currCost] > pdoMaxJDF){
        jdf[currCost] = pdoMaxJDF;
    }
}

static costType initSolution(void) {
    int i, j, i2, count, maxCount, l;
    costType initCost;
    coveredType *ptr;
    rankType *coveringsPtr;


    for(i = 0; i < b; i++) {
        if(randomStartFlag){
            kset[i] = rnd(binCoef[v][k]);
            //printf("%d\n", i);
        }else if(greedyStartFlag){
            //look at first m-set not covered
            //construct partial block that covers it
            //if block has leftover spaces, attempt to cover next m-set

            int idx = 0;
            varietyType *block = calloc(k + 1, sizeof(int));

            for(j = 0; j < binCoef[v][m]; j++){
                if(k < idx+t){
                    //not enough room to cover another one
                    break;
                }
                if(!covered[j]){
                    //this m-set not covered
                    //take first t elements of m-set, and add to block
                    varietyType *mset = calloc(m + 1, sizeof(int));
                    unrankSubset(covered[j], mset, m);
                    for(i2 = 0; i2 < t; i2++){
                        block[i2] = mset[i2 + idx];
                    }
                    idx += t;
                }
            }
            //fill in block with other stuff
            varietyType *com = calloc(v - idx, sizeof(int));
            makeComplement(block, com, v);
            for(; idx < k; idx++){
                block[idx] = com[rnd(v-idx)];//TODO hope it doesn't hit same number twice
            }

            qsort((char *) block, k, sizeof(varietyType), compareVarieties);
            //block done
            kset[i] = rankSubset(block, k);

            /*
            //TODO this would be way, way, way too slow
            maxCount = 0;
            //go through all k-sets, and pick the best one?
            for(j = 0; j < binCoef[v][k]; j++){
                rankType block = (rankType) j;
                coveringsPtr = coverings;
                count = 0;
                //count the number of m-sets this block would cover, that arent' already covered
                for(i2 = 0; i2 < coverLen - 1; i2++){
                    if(!covered[coveringsPtr[i2]]){
                        count++;
                    }
                }
                if(count > maxCount){
                    maxCount = count;
                }
            }
            */
        } else{
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
    int indices[] = {0,1,2,3,4,5,6,7,8,9};
    if(pdoPrint == 2){
        //print first 10 costs that have been found rather than just first 10
        int i = 0;
        int curr = 0;
        for(; i < maxCost && curr < 10; i++){
            if(jdc[i] > 1){
                indices[curr] = i;
                curr++;
            }
        }
        if(curr != 10){
            qsort((int *)indices, 10, sizeof(int), compareVarieties);
        }
    }
    printf("                                                                                                             ");
    printf("\rlevel\t\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
    indices[0], indices[1], indices[2], indices[3],
    indices[4], indices[5], indices[6], indices[7],
    indices[8], indices[9]);
    printf("                                                                                                             ");
    printf("\rjdc\t\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
    jdc[indices[0]], jdc[indices[1]], jdc[indices[2]], jdc[indices[3]],
    jdc[indices[4]], jdc[indices[5]], jdc[indices[6]], jdc[indices[7]],
    jdc[indices[8]], jdc[indices[9]]);
    printf("                                                                                                             ");
    printf("\rjdf\t\t\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
    jdf[indices[0]], jdf[indices[1]], jdf[indices[2]], jdf[indices[3]],
    jdf[indices[4]], jdf[indices[5]], jdf[indices[6]], jdf[indices[7]],
    jdf[indices[8]], jdf[indices[9]], currCost);
    printf("\033[F");
    printf("\033[F");
}

costType pdo() {
    int i;

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
        jdf[i] = pdoK * neighborLen;
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

    printProgress();

    while( currCost > 0){
        counter++;
        if( costDifference > 0) { //if( neighborCost > currCost){
            if( counter > pdoJ * jdf[currCost] ){ //TODO parameter here
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
            if(pdoPrint && !printCount--){
                printProgress();
                printCount = pdoPrintFreq;
            }
            counter = 0;
            currCost += costDifference;
        }
        //try random neighbour
        costDifference = computeNeighbor();
    }
    if(pdoPrint){
        printf("\n\n\n\n");
    }

    return 0;
}
