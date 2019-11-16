#include "sir.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

GLOBALS g;
NODE *n;

void infect() {
	unsigned int i, you, me = g.heap[1];
	float t, now = n[me].time;

	del_root();
	n[me].heap = I_OR_R;
	n[me].time +=
	    g.rexp[pcg_16()] * g.beta;  // bcoz g.rexpr has a / g.beta factor
	if (n[me].time > g.t) g.t = n[me].time;
	g.s++;

	for (i = 0; i < n[me].deg; i++) {
		you = n[me].nb[i];

		if (n[you].heap != I_OR_R &&
		    n[you].immune == 0) {  // if you is S, and not immune, you
					     // can be infected.
			t = now + g.rexp[pcg_16()];

			if ((t < n[me].time) && (t < n[you].time)) {
				n[you].time = t;
				if (n[you].heap ==
				    NONE) {  // if not listed before, then
					     // extend the heap
					g.heap[++g.nheap] = you;
					n[you].heap = g.nheap;
					n[you].payoff = n[you].payoff - 1.0;
					// n[you].payoff_each = n[you].payoff_each - 1.0;
					n[you].ninf++;
					// fprintf(logfile, "oops, node %d get
					// infected from %d\n", you, me);
				}
				up_heap(n[you].heap);
			}
		}
	}
}

void sir() {
	unsigned int i, source = 0;

	g.t = 0.0;
	g.s = 0;

	source = pcg_32_bounded(g.n);

	if (n[source].immune) {
		return;
	}

	else {
		for (i = 0; i < g.n; i++) {
		n[i].heap = NONE;
		n[i].time = DBL_MAX;  // to a large value
		}

		n[source].payoff = n[source].payoff - 1;
		n[source].ninf++;

		n[source].time = 0.0;
		n[source].heap = 1;
		g.heap[g.nheap = 1] = source;

		while (g.nheap) infect();
	}
}

void make_decision(int me) {
	unsigned int you;

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
		n[me].decision = n[successful].immune;
	}
	
	// Mutation at a low probability
	if (get_one_or_zero_randomly(0.01)) {
			n[me].decision = !n[me].decision;
	}

}

void vaccinate() {
	float covrg_each = 0.0;

	// set immunity based on the decisions, and reset payoff and ninf
	for (unsigned int me = 0; me < g.n; me++) {
		n[me].immune = n[me].decision;
		n[me].ninf = 0;

		if (n[me].immune == 1) {
			n[me].payoff = -g.vac_cost;
			// n[me].payoff_each = -g.vac_cost;
			covrg_each += 1.0;
		}
		else {
			n[me].payoff = 0;
		}	
	}
	g.coverage = covrg_each / g.n;
}

int main(int argc, char *argv[]) {
	set_global(argc, argv);

	for (int run = 0; run < SEASONS; run++) {
		g.ss1 = 0;

		for (int k = 0; k < NAVG; k++) {
			sir();
			g.ss1 += (double)g.s;
		}

		for (unsigned int ind = 0; ind < g.n; ind++) {
			if (n[ind].immune == 0) {
				n[ind].payoff = n[ind].payoff / (float)NAVG;
			}
		}

		g.ss1 /= NAVG;
		g.convergenceWatcher[run] = (float)g.ss1/g.n;

		if (run == 0) {
			for (unsigned int j = 0; j < g.n; j++) {
				if (get_one_or_zero_randomly(g.degree_rationality)) {
						n[j].nature = Rational;
					} else {
						n[j].nature = Conforming;
					}
					make_decision(j);
				}
			vaccinate();
		}
		if (0 < run && run < 6) {
			for (unsigned int j = 0; j < g.n; j++) {
				n[j].tail = addToLink(n[j].tail, n[j].payoff, n[j].nature);
				if (get_one_or_zero_randomly(g.degree_rationality)) {
					n[j].nature = Rational;
				}
				else{
					n[j].nature = Conforming;
				}
				make_decision(j);
			}
			vaccinate();
		} else if ( run < SEASONS - 1 ) {

			if ( 100 < run && check_convergence(run, 5, 0.0002)) {
				print_result(g.coverage);
				break;
			}

			for (unsigned int index = 0; index < g.n; index++) {
				n[index].tail = addToLink(n[index].tail, n[index].payoff, n[index].nature);
				n[index].head = removeHeadFromLink(n[index].head);

				struct oneMemory * ref  = malloc(sizeof(struct oneMemory));
				float payoff_rational = 0.0;
				int num_rational = 0;
				float payoff_conforming = 0.0;
				int num_conforming = 0;

				ref = n[index].head -> next;
				while (ref != n[index].tail) {
					if (ref -> nature == Rational) {
						num_rational ++;
					 	payoff_rational += ref -> payoff;
					}

					else {
						num_conforming ++;
						payoff_conforming += ref -> payoff;
					}
					ref = ref -> next;
				}

				if (num_rational == 0) {
					n[index].nature = Conforming;
				}
				
				if (num_conforming == 0) {
					n[index].nature = Rational;
				}

				if (num_rational != 0 && num_conforming != 0) {
					if (payoff_rational / num_rational > payoff_conforming/ num_conforming) {
						n[index].nature = Rational;
					} else {
						n[index].nature = Conforming;
					}
				}			

				// mutation
				if (get_one_or_zero_randomly(0.01)) {
					if (n[index].nature == Conforming) {
						n[index].nature = Rational;
					}
					else {
						n[index].nature = Conforming;
					}
				}
				make_decision(index);
			}
			vaccinate();
		} else {
			print_result(g.coverage);
		}

	}

	for (unsigned int re = 0; re < g.n; re++) free(n[re].nb);
	free(n);
	free(g.heap);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
