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
		//fprintf(logfile, "Chosen source is immune!\n");
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

int main(int argc, char *argv[]) {
	set_global(argc, argv);

	for (int run = 0; run < SEASONS; run++) {
		reset_result_each_season();

		for (int k = 0; k < NAVG; k++) {
			sir();
			add_to_tmp();
			}
		calculate_payff_each_agent();
		finalize_result_each_season();

		g.convergenceWatcher[run] = (float)g.numCf/g.n;

		if (run < 5) {
			add_to_memory();
			set_characteristics_randomly();
			make_strategy();
		} else if ( run < SEASONS - 1 ) {
			if ( 20 < run && check_convergence(run, 5, 0.00005)) {
				print_result(g.coverage);
				break;
			}
			add_to_memory_and_remove_old();
			set_characteristics_memory_based();
			make_strategy();
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
