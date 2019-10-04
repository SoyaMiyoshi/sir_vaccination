// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

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

FILE *logfile;

void infect() {
	unsigned int i, you, me = g.heap[1];
	float t, now = n[me].time;

	del_root();
	n[me].heap = I_OR_R;
	// get the recovery time
	n[me].time +=
	    g.rexp[pcg_16()] * g.beta;  // bcoz g.rexpr has a / g.beta factor
	if (n[me].time > g.t) g.t = n[me].time;
	g.s++;

	// go through the neighbors of the infected node . .
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
					n[you].payoff_each =
					    n[you].payoff_each - 1.0;
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

		//fprintf(logfile, "Chosen source %d\n", source);
		n[source].payoff = n[source].payoff - 1;
		n[source].payoff_each = n[source].payoff_each - 1;
		n[source].ninf++;

		n[source].time = 0.0;
		n[source].heap = 1;
		g.heap[g.nheap = 1] = source;

		while (g.nheap) infect();
	}
}

struct oneMemory * addToLink(struct oneMemory *tail, float payoff, enum Nature nature){
    tail->payoff = payoff;
	tail->nature = nature;
    tail->next = malloc(sizeof(struct oneMemory));
    //return new tail
    return(tail->next);
}

struct oneMemory * removeHeadFromLink(struct oneMemory * head){
    struct oneMemory * tmp = head -> next;
    free(head);
    //return new head 
    return(tmp);
}

void add_to_memory() {
	for (unsigned int ind = 0 ; ind < g.n; ind++) {
		n[ind].tail = addToLink(n[ind].tail, n[ind].payoff, n[ind].nature);
	}
}

void remove_oldest_memory(){
	for (unsigned int ind = 0 ; ind < g.n; ind++) {
		n[ind].head = removeHeadFromLink(n[ind].head);
	}
}

// set characteristics randomly first, then each chooses one that miximized payoff 
int main(int argc, char *argv[]) {
	set_global(argc, argv);
	char log_dirname[100];
	char log_filename[100];
	create_dir_and_file(log_dirname, log_filename, argv);
	logfile = fopen(log_filename, "w");

	int convergence_flag = 0;

	// first, [coverage] percent of the population
	// will get the vaccination
	vaccinate_everyone();
	set_characteristics_randomly();


	for (int run = 0; run < SEASONS + 1; run++) {
		reset_result_each_season();

		for (int k = 0; k < NAVG; k++) {
			//fprintf(logfile,"~~~~~~~ Season %d, Sim %d th ~~~~~~~ \n", run,k);
			sir();
			calculate_outbreaksize_and_timetoext();
			calculate_payoff_each_group();
		}

		calculate_payff_each_agent();
		finalize_result_each_season();

		if( check_convergence(run, 10, 0.001) ){
			fprintf(logfile, "System converged!\n");
			print_result(g.coverage);
			convergence_flag = 1;
			break;
		}

		add_to_memory();
		if (run < 5) {
			set_characteristics_randomly();
		} else {
			remove_oldest_memory();
			set_characteristics_memory_based();
		}
		
		make_strategy();
	}

	if (convergence_flag == 0) {
		fprintf(logfile, "System did not converge\n");
		print_result(g.coverage);
	}

	fclose(logfile);

	for (unsigned int re = 0; re < g.n; re++) free(n[re].nb);
	free(n);
	free(g.heap);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
