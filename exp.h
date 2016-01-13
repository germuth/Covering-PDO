/*
** exp.h
**
** This file contains the declarations needed for the approximate 
** exponentiation. The accuracy is determined by the label EXPSIZE.
** If accuracy is not good enough, increase EXPSIZE. Then the table
** will take more space.
**
*/

#ifndef _exp_
#define _exp_

#define EXPSIZE 2000
extern double exps[];
extern double neglibleExp;

void calculate_exps(void);

#include <math.h>

#define ApprExpProb(R) ((R) > neglibleExp ? \
			0.0 : exps[(int) ((R) / neglibleExp * EXPSIZE + 0.5)])

#define ExpProb(R) (apprexp ? ApprExpProb(R) : (exp(-R)))


#endif /* #ifndef _exp_ */
