#include "sir.h"
#include <math.h>
#include <stdlib.h>

extern NODE *n;
extern GLOBALS g;

void make_strategy(FILE *output) {
    unsigned int me, i, you, count, successful, majority;
    //int max;

    for (me = 0; me < g.n; me++) {

        if(n[me].is_zealot == 0) {

            if (n[me].is_conformist) {
                // Strategy-making for conformists
                count = 0;
                majority = 1;
                // Include yourself
                if (n[me].immunity == 1) {
                    count += 1;
                }

                for (i = 0; i < n[me].deg; i++) {
                    if (n[n[me].nb[i]].immunity == 1) count++;
                }

                if ((n[me].deg + 1) / 2 > count) {
                    majority = 0;
                }

                // Set the decision (for next year) according to the majority
                n[me].decision = majority;

            }

            else {
                // strategy-making for imitators
                count = 0; // meaningless, debugging purpose

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

                // Set the decision (for next year) according to the best-performing neighbour
                n[me].decision = n[successful].immunity;
            }

        }

        //fprintf(output, "zealot?,conformist?,how many neighbour vaccinate?,decision? \n");
        fprintf(output, "%d, %d, %d, %d\n", n[me].is_zealot, n[me].is_conformist, count, n[me].decision);

    }


    // fprintf(output, "---------\n");

    float covrg_each = 0.0;

    // set immunity based on the decisions, and reset payoff
    for(me = 0; me < g.n; me++){

        n[me].immunity = n[me].decision;
        n[me].payoff = 0;
        if(n[me].immunity == 1){
            /*
            if(n[me].is_zealot == 1){
                fprintf(output, "something, wrong\n");
            }*/
            n[me].payoff += -g.vac_cost;
            covrg_each += 1.0;
        }
        n[me].ninf = 0;
    }
    //fprintf(output, "%d, %d, %d, %d\n",);
    g.coverage = covrg_each/g.n;

}