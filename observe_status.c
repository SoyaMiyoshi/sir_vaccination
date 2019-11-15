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
	g.utility = 0;

	for (unsigned int ind = 0; ind < g.n; ind++) {
		g.utility += n[ind].payoff;
	}
	g.utility = g.utility/g.n;

	printf("%g ", g.ss1/g.n);
    printf("%g ", (double)g.numCf/g.n); // Proportion of conforming nodes 
	printf("%f ", coverage);
	printf("%g\n", g.utility);
}

bool check_convergence(int run, int n, float threshold){

	if ( n <= run && fabs(g.convergenceWatcher[run - n] - g.convergenceWatcher[run]) < n*threshold ) {
		return true;
	} 
	return false; 
	
}