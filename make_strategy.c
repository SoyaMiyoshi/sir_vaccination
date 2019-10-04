#include <math.h>
#include <stdlib.h>
#include "sir.h"

extern NODE *n;
extern GLOBALS g;

extern FILE *logfile;

void make_strategy() {
	unsigned int me, you;

	for (me = 0; me < g.n; me++) {
		if (n[me].nature == Conforming) {
			// Strategy-making for conformists
			unsigned int count = 0;

			// Include yourself
			if (n[me].immune == 1) {
				count += 1;
			}

			// Count the num of neighbours
			// who chose to vaccinate
			for (unsigned int i = 0; i < n[me].deg; i++) {
				you = n[me].nb[i];
				if (n[you].immune == 1) count++;
			}

			// If less than half of neighbours
			// chose to vaccinate
			if (n[me].deg + 1 > 2 * count) {
				n[me].decision = 0;
			}

			// If 5:5, then does not change the decision
			if (n[me].deg + 1 == 2 * count) {
				n[me].decision = n[me].immune;
			}

			// If more than half
			if (n[me].deg + 1 < 2 * count) {
				n[me].decision = 1;
			}

		}

		if (n[me].nature == Rational) {
			// strategy-making for imitators

			// Include yourself
			float max = n[me].payoff;
			int successful = me;

			for (unsigned int i = 0; i < n[me].deg; i++) {
				you = n[me].nb[i];
				if (n[you].payoff > max) {
					max = n[you].payoff;
					successful = you;
				}
			}

			// Set the decision (for next year) according to
			// the best-performing neighbour
			n[me].decision = n[successful].immune;

		}
		
		 // Mutation at a low probability
		if (get_random_int(0.01)) {
                n[me].decision = !n[me].decision;
		}
	}

	float covrg_each = 0.0;

	// set immune based on the decisions, and reset payoff and ninf
	for (me = 0; me < g.n; me++) {
		n[me].immune = n[me].decision;
		n[me].payoff = 0;
		n[me].ninf = 0;
		if (n[me].immune == 1) {
			n[me].payoff = -g.vac_cost;
			n[me].payoff_each = -g.vac_cost;
			covrg_each += 1.0;
		}
		else {
			n[me].payoff_each = 0;
		}
		n[me].ninf = 0;
	
	}
	g.coverage = covrg_each / g.n;
}
