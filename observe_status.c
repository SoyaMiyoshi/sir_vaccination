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

void add_to_tmp(){
	g.ss1 += (double)g.s;
	// g.ss2 += SQ((double)g.s);  // stderr を計算するため
	// g.st1 += g.t;
	// g.st2 += SQ(g.t);
}

void calculate_payoff_each_group() {
	// float pfAllAvg_temp = 0.0;
	// float pfRationalAvg_temp = 0.0;
	// float pfConfAvg_temp = 0.0;

	// for (unsigned int ind = 0; ind < g.n; ind++) {
	// 	pfAllAvg_temp += n[ind].payoff_each;

	// 	if (n[ind].nature == Conforming) {
	// 		pfConfAvg_temp += n[ind].payoff_each;
	// 	} 
	// 	if (n[ind].nature == Rational) {
	// 		pfRationalAvg_temp += n[ind].payoff_each;
	// 	}

	// 	if (n[ind].immune == 0) {
	// 		n[ind].payoff_each = 0;
	// 	}
	// }

	// g.pfAllAvg += pfAllAvg_temp / g.n;
	// g.pfConfAvg += pfConfAvg_temp / g.numCf;
	// g.pfRationalAvg += pfRationalAvg_temp / g.numRational;

	// g.pfAllSqdAvg += SQ(pfAllAvg_temp / g.n);
	// g.pfConfSqdAvg += SQ(pfConfAvg_temp / g.numCf);
	// g.pfRationalSqdAvg += SQ(pfRationalAvg_temp / g.numRational);
}

void reset_result_each_season() {
	g.ss1 = 0;
	// g.ss2 = 0;
	// g.st1 = 0;
	// g.st2 = 0;  // for averages

	// g.pfAllAvg = 0;
	// g.pfAllSqdAvg = 0;
	// g.pfRationalAvg = 0;
	// g.pfRationalSqdAvg = 0;
	// g.pfConfAvg = 0;
	// g.pfConfSqdAvg = 0;
}

void calculate_payff_each_agent() {
	for (unsigned int ind = 0; ind < g.n; ind++) {
		if (n[ind].immune != 1) {
			n[ind].payoff = n[ind].payoff / (float)NAVG;
		}
	}
}

void finalize_result_each_season() {
	g.ss1 /= NAVG;
	// g.ss2 /= NAVG;
	// g.st1 /= NAVG;
	// g.st2 /= NAVG;

	// g.pfAllAvg /= NAVG;
	// g.pfConfAvg /= NAVG;
	// g.pfRationalAvg /= NAVG;
	// g.pfAllSqdAvg /= NAVG;
	// g.pfConfSqdAvg /= NAVG;
	// g.pfRationalSqdAvg /= NAVG;
}

void print_result(float coverage) {
	printf("%g ", g.ss1);
	// printf("%g %g ", g.ss1,
	//        sqrt((g.ss2 - SQ(g.ss1)) /
	// 	    (NAVG - 1)));  // outbreaksize (avg, stderr)
	// printf("%g %g ", g.st1,
	//        sqrt((g.st2 - SQ(g.st1)) /
	// 	    (NAVG - 1)));  // time to extinction (avg, stderr)
	// printf("%g %g ", g.pfAllAvg,
	//        sqrt((g.pfAllSqdAvg - SQ(g.pfAllAvg)) / (NAVG - 1)));
	// printf("%g %g ", g.pfConfAvg,
	//        sqrt((g.pfConfSqdAvg - SQ(g.pfConfAvg)) / (NAVG - 1)));
	// printf("%g %g ", g.pfRationalAvg,
	//        sqrt((g.pfRationalSqdAvg - SQ(g.pfRationalAvg)) / (NAVG - 1)));
    printf("%g ", (double)g.numCf/g.n); // Proportion of conforming nodes 
	printf("%f \n", coverage);
}

bool check_convergence(int run, int n, float threshold){

	if ( n <= run && fabs(g.convergenceWatcher[run - n] - g.convergenceWatcher[run]) < n*threshold ) {
		// fprintf(logfile, "Convereged at run %d, values : run - n %g, run %g \n", run, g.convergenceWatcher[run - n] , g.convergenceWatcher[run]);
		return true;
	} 
	
	return false; 
	
}