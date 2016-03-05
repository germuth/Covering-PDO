/*   anneal.h
**
**   This file contains declarations needed for starting the simulated
**   annealing process.
**
*/

costType simulatedAnnealing(double coolFact, double initProb, 
			    int iterLength, int frozen, int endLimit);
costType localOptimization(int frozen, int endLimit);

extern int iterCounter;
extern float endT;
