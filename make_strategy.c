#include <math.h>
#include <stdlib.h>
#include "sir.h"

extern NODE *n;
extern GLOBALS g;

extern FILE *logfile;

void vaccinate() {

	float covrg_each = 0.0;

	// set immunity based on the decisions, and reset payoff and ninf
	for (unsigned int me = 0; me < g.n; me++) {
		n[me].immune = n[me].decision;
		n[me].ninf = 0;

		if (n[me].immune == 1) {
			n[me].payoff = -g.vac_cost;
			covrg_each += 1.0;
		}
		else {
			n[me].payoff = 0;
		}	
	}
	g.coverage = covrg_each / g.n;
}
