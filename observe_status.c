#include "sir.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern GLOBALS g;
extern NODE *n;

extern FILE *logfile;

void print_result(float coverage) {
	float ua1 = 0;
	float ua2 = 0;
	int nc = 0; // number of conforming node
	float ur1 = 0; // utility of rationalist
	float ur2 = 0; // its variance
	float uc1 = 0; // utility of conformists
	float uc2 = 0;

	float nv1 = 0; // utility of non-vaccinator
	float nv2 = 0; // its variance 

	for (unsigned int ind = 0; ind < g.n; ind++) {
		if (n[ind].nature == Rational) {
			ur1 += n[ind].payoff;
			ur2 += n[ind].payoff*n[ind].payoff;
		} else {
			uc1 += n[ind].payoff;
			uc2 += n[ind].payoff*n[ind].payoff;
			nc++;
		}

		if (n[ind].immune == 0) {
			nv1 += n[ind].payoff;
			nv2 += n[ind].payoff*n[ind].payoff;
		}

	}
	ua1 = (ur1 + uc1)/g.n;
	ua2 = (ur2 + uc2)/g.n - ua1*ua1;
	ur1 = ur1/(g.n - nc);
	ur2 = ur2/(g.n - nc) - ur1*ur1;
	uc1 = uc1/nc;
	uc2 = uc2/nc - uc1*uc1;

	nv1 = nv1/(g.n*(1 - coverage));
	nv2 = nv2/(g.n*(1 - coverage)) - nv1*nv1;

	printf("%g ", g.ss1/g.n);
    printf("%g ", (double)nc/g.n); // Proportion of conforming nodes 
	printf("%f ", coverage);
	printf("%f %f ", ua1, ua2);
	printf("%f %f ", uc1, uc2);
	printf("%f %f ", ur1, ur2);
	printf("%f %f \n", nv1, nv2);

}

bool check_convergence(int run, int n, float threshold){

	if ( n <= run && fabs(g.convergenceWatcher[run - n] - g.convergenceWatcher[run]) < n*threshold ) {
		return true;
	} 
	return false; 
	
}