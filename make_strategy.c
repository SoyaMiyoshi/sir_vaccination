#include <math.h>
#include <stdlib.h>
#include "sir.h"

extern NODE *n;
extern GLOBALS g;

extern FILE *logfile;

void make_strategy() {
	unsigned int me, you;

	// Rather than traversing all the n[i],
	// re-reading network file is more efficient...
	for (me = 0; me < g.n; me++) {
		if (n[me].is_zealot == 0) {
			if (n[me].is_conformist) {
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

                /*
				fprintf(logfile,
					"%d is a conformist whose decision was "
					"%d\n",
					me, n[me].immune);

				for (unsigned int i = 0; i < n[me].deg; i++) {
					you = n[me].nb[i];
					fprintf(logfile,
						"He knows node %d whose "
						"decision was %d \n",
						n[me].nb[i], n[you].immune);
				}

				if (n[me].deg + 1 == 2 * count) {
                    fprintf(logfile,
                        "I have %d neighbours including myself "
                        "and I will keep my strategy that is %d \n\n",
                        n[me].deg + 1 ,
                        n[me].decision);
				}

                if (n[me].deg + 1 != 2 * count) {
                    fprintf(logfile,
                        "Majority is %d so um, he will do %d\n\n",
                        n[me].deg + 1 < 2 * count,
                        n[me].decision);
				}
				*/

			}

			else {
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

				/*
				fprintf(logfile,
					"%d is a strategist whose decision was "
					"%d \n",
					me, n[me].immune);
				fprintf(logfile, "%d's (self) payoff is %f \n",
					me, n[me].payoff);
				for (unsigned int i = 0; i < n[me].deg; i++) {
					you = n[me].nb[i];
					fprintf(logfile,
						"He knows node %d whose payoff "
						"is %f \n",
						n[me].nb[i], n[you].payoff);
				}

				fprintf(logfile,
					"Successful nb is %d whose strategy is "
					"%d \n\n",
					successful, n[successful].immune);
				*/
			}
		}

		 // Mutation at a low probability
		if (get_random_int(0.01)) {
                //fprintf(logfile, "He is %d, before mutation: %d \n", me, n[me].decision);
                n[me].decision = !n[me].decision;
				//fprintf(logfile, "Node %d mutated! \n", me);
                //fprintf(logfile, "He is %d, after mutation: %d \n", me, n[me].decision);
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
		/*
		fprintf(logfile,
			"I am %d and I will get vaccination next year? > %d\n",
			me, n[me].immune);*/
	}
	g.coverage = covrg_each / g.n;
}
