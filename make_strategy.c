#include <math.h>
#include <stdlib.h>
#include "sir.h"

extern NODE *n;
extern GLOBALS g;

void make_strategy(FILE *output) {
	unsigned int me, i, you, count, successful, majority;
	// int max;

	for (me = 0; me < g.n; me++) {
	    //fprintf(output, "----------\n");
		if (n[me].is_zealot == 0) {
			if (n[me].is_conformist) {
				// Strategy-making for conformists
				count = 0;
				majority = 1;
				// Include yourself
				if (n[me].immunity == 1) {
					count += 1;
				}

				for (i = 0; i < n[me].deg; i++) {
					if (n[n[me].nb[i]].immunity == 1)
						count++;
				}

				if ((float)(n[me].deg + 1) / 2 > (float)count + 0.001) {

					majority = 0;
				}
				// Set the decision (for next year) according to
				// the majority
				n[me].decision = majority;

				fprintf(output, "%d is a conformist whose decision was %d\n", me, n[me].immunity);
				for (i = 0; i < n[me].deg; i++){
				    fprintf(output, "He knows node %d whose decision was %d \n",n[me].nb[i], n[n[me].nb[i]].immunity);
				}

				fprintf(output, "Majority is %d so he will do %d\n\n", majority, n[me].decision);
			}

			else {
				// strategy-making for imitators
				count = 0;  // meaningless, debugging purpose

				// Include yourself
				float max = n[me].payoff;
				successful = me;

				for (i = 0; i < n[me].deg; i++) {
					you = n[me].nb[i];
					if (n[you].payoff > max) {
						max = n[you].payoff;
						successful = you;
					}
				}

				// Set the decision (for next year) according to
				// the best-performing neighbour
				n[me].decision = n[successful].immunity;

				fprintf(output, "%d is a strategist whose decision was %d \n", me, n[me].immunity);
				fprintf(output, "%d's (self) payoff is %f \n", me, n[me].payoff);
				for (i = 0; i < n[me].deg; i++){

				    fprintf(output, "He knows node %d whose payoff is %f \n",n[me].nb[i], n[n[me].nb[i]].payoff);
				}

				fprintf(output, "Successful nb is %d whose strategy is %d \n\n", successful, n[successful].immunity);
			}
		}

		// fprintf(output, "zealot?,conformist?,how many neighbour
		// vaccinate?,decision? \n");
		//fprintf(output, "%d, %d, %d, %d\n", n[me].is_zealot,
		//	n[me].is_conformist, count, n[me].decision);
	}

	// fprintf(output, "---------\n");

	float covrg_each = 0.0;

	// set immunity based on the decisions, and reset payoff
	for (me = 0; me < g.n; me++) {
		n[me].immunity = n[me].decision;
		n[me].payoff = 0;
		if (n[me].immunity == 1) {
			/*
			if(n[me].is_zealot == 1){
			    fprintf(output, "something, wrong\n");
			}*/
			n[me].payoff += -g.vac_cost;
			covrg_each += 1.0;
		}
		n[me].ninf = 0;
		fprintf(output, "I am %d and I will get vaccination? > %d\n", me, n[me].immunity);
	}
	// fprintf(output, "%d, %d, %d, %d\n",);
	g.coverage = covrg_each / g.n;
}
