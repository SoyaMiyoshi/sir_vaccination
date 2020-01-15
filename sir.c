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
RECORD record;

FILE *logfile;

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

void develop_nature(unsigned int index) {

	// Add to storage 
	if(n[index].nature == Conforming) {
		n[index].storage.payoff_conforming += n[index].payoff / g.memory_length;
		n[index].storage.num_conforming += 1;
	} else {
		n[index].storage.payoff_rational += n[index].payoff / g.memory_length;
		n[index].storage.num_rational += 1;
	}

    // Add to link
	n[index].tail = addToLink(n[index].tail, n[index].payoff, n[index].nature);

	// Remove from storage 
	if(n[index].head -> nature == Conforming) {
		n[index].storage.payoff_conforming -= n[index].head -> payoff / g.memory_length;
		n[index].storage.num_conforming -= 1;
	} else {
		n[index].storage.payoff_rational -= n[index].head -> payoff / g.memory_length;
		n[index].storage.num_rational -= 1;
	}

	// Remove from link
	n[index].head = removeHeadFromLink(n[index].head);

	if(n[index].storage.num_conforming != 0 && n[index].storage.num_rational != 0){
		if(n[index].storage.payoff_conforming > n[index].storage.payoff_rational){
			n[index].nature = Conforming;
		} else {
			n[index].nature = Rational;
		}
	}

}

int main(int argc, char *argv[]) {
	set_global(argc, argv);

	char log_dirname[100];
	char log_filename[100];
	create_dir_and_file(log_dirname, log_filename, argv);
	logfile = fopen(log_filename, "w");

	for (int run = 0; run < SEASONS; run++) {
		g.ss1 = 0;

		if( 1 < NAVG ) {
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
		} else {
			sir();
			g.ss1 += (double)g.s;
		}

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

		} else if (run < g.memory_length + 1) {

			for (unsigned int j = 0; j < g.n; j++) {
				n[j].tail = addToLink(n[j].tail, n[j].payoff, n[j].nature);
				if (get_one_or_zero_randomly(g.degree_rationality)) {
					n[j].nature = Rational;
				} else {
					n[j].nature = Conforming;
				}
				make_decision(j);
			}
			vaccinate();

		} else if (run < SEASONS - CUTOFF) {

			for (unsigned int index = 0; index < g.n; index++) {
				develop_nature(index);
				make_decision(index);
			}
			vaccinate();

		} else if (run < SEASONS - 1) {

			record.coverage += g.coverage;
			record.outbreak_size += g.ss1;

			for (unsigned int index = 0; index < g.n; index++) {
				if (n[index].nature == Conforming) {
					record.proportion_conformists += 1;
				}
				develop_nature(index);
				make_decision(index);
			}
			vaccinate();

		} else if ( run == SEASONS - 1 ) {

			record.coverage += g.coverage;
			record.outbreak_size += g.ss1;

			for (unsigned int index = 0; index < g.n; index++) {
				if (n[index].nature == Conforming) {
					record.proportion_conformists += 1;
				}
			}
		}
	}


	record.proportion_conformists /= CUTOFF*g.n;
	record.coverage /= CUTOFF;
	record.outbreak_size /= CUTOFF*g.n;

	printf("%f %f %f \n", record.proportion_conformists, record.coverage, record.outbreak_size);

	for (unsigned int re = 0; re < g.n; re++) free(n[re].nb);
	free(n);
	free(g.heap);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
