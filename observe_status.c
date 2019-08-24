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

void calculate_outbreaksize_and_timetoext() {
	g.ss1 += (double)g.s;
	g.ss2 += SQ((double)g.s);  // stderr を計算するため
	g.st1 += g.t;
	g.st2 += SQ(g.t);
}

void calculate_payoff_each_group() {
	float pfAllAvg_temp = 0.0;
	float pfImtAvg_temp = 0.0;
	float pfConfAvg_temp = 0.0;
	float pfZlAvg_temp = 0.0;

	for (unsigned int ind = 0; ind < g.n; ind++) {
		pfAllAvg_temp += n[ind].payoff_each;

		if (n[ind].is_zealot) {
			pfZlAvg_temp += n[ind].payoff_each;
		} else {
			if (n[ind].is_conformist) {
				pfConfAvg_temp += n[ind].payoff_each;
			} else {
				pfImtAvg_temp += n[ind].payoff_each;
			}
		}

		/* fprintf(logfile, "I am %d, immu? %d, Z? %d, Conf? %d, Pay_ec
		  %f Pay_tot %f \n", ind, n[ind].immune, n[ind].is_zealot,
		  n[ind].is_conformist, n[ind].payoff_each, n[ind].payoff);*/

		if (n[ind].immune == 0) {
			n[ind].payoff_each = 0;
		}
		// fprintf(logfile, "I am %d, Conf? %d, Payoff_each %f \n", l,
		// n[l].is_conformist, n[l].payoff_each);
	}

	g.pfAllAvg += pfAllAvg_temp / g.n;
	g.pfZlAvg += pfZlAvg_temp / g.numZl;
	g.pfConfAvg += pfConfAvg_temp / g.numCf;
	g.pfImtAvg += pfImtAvg_temp / g.numImt;

	g.pfAllSqdAvg += SQ(pfAllAvg_temp / g.n);
	g.pfZlSqdAvg += SQ(pfZlAvg_temp / g.numZl);
	g.pfConfSqdAvg += SQ(pfConfAvg_temp / g.numCf);
	g.pfImtSqdAvg += SQ(pfImtAvg_temp / g.numImt);
}

void reset_result_each_season() {
	g.ss1 = 0;
	g.ss2 = 0;
	g.st1 = 0;
	g.st2 = 0;  // for averages

	g.pfAllAvg = 0;
	g.pfAllSqdAvg = 0;
	g.pfImtAvg = 0;
	g.pfImtSqdAvg = 0;
	g.pfConfAvg = 0;
	g.pfConfSqdAvg = 0;
	g.pfZlAvg = 0;
	g.pfZlSqdAvg = 0;
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
	g.ss2 /= NAVG;
	g.st1 /= NAVG;
	g.st2 /= NAVG;

	g.pfAllAvg /= NAVG;
	g.pfZlAvg /= NAVG;
	g.pfConfAvg /= NAVG;
	g.pfImtAvg /= NAVG;
	g.pfAllSqdAvg /= NAVG;
	g.pfZlSqdAvg /= NAVG;
	g.pfConfSqdAvg /= NAVG;
	g.pfImtSqdAvg /= NAVG;
}

void print_result(float coverage) {
	printf("%g %g ", g.ss1,
	       sqrt((g.ss2 - SQ(g.ss1)) /
		    (NAVG - 1)));  // outbreaksize (avg, stderr)
	printf("%g %g ", g.st1,
	       sqrt((g.st2 - SQ(g.st1)) /
		    (NAVG - 1)));  // time to extinction (avg, stderr)
	printf("%g %g ", g.pfAllAvg,
	       sqrt((g.pfAllSqdAvg - SQ(g.pfAllAvg)) / (NAVG - 1)));
	printf("%g %g ", g.pfZlAvg,
	       sqrt((g.pfZlSqdAvg - SQ(g.pfZlAvg)) / (NAVG - 1)));
	printf("%g %g ", g.pfConfAvg,
	       sqrt((g.pfConfSqdAvg - SQ(g.pfConfAvg)) / (NAVG - 1)));
	printf("%g %g ", g.pfImtAvg,
	       sqrt((g.pfImtSqdAvg - SQ(g.pfImtAvg)) / (NAVG - 1)));
	printf("%f \n", coverage);
}