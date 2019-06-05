#include "sir.h"
#include <math.h>
#include <stdlib.h>

extern NODE *n;
extern GLOBALS g;

void make_strategy(FILE *output){
    unsigned int me,i,you,count,successful,majority;
    int max;

    for(me = 0; me < g.n; me++){
       max = -100;
       count = 0;
       majority = 1;
       for(i = 0; i < n[me].deg; i++){
            you = n[me].nb[i];
            if(n[you].payoff > max){
                max = n[you].payoff;
                successful = you;
            }
            if(n[you].immunity == 1) count++;
       }
       if( (n[me].deg+1)/2 > count ) {majority = 0; count = n[me].deg - count;}
    }

    //fprintf(output, "---------\n");

    // set immunity based on the decisions, and reset payoff
    for(me = 0; me < g.n; me++){
        n[me].immunity = n[me].decision;
        n[me].payoff = 0;
        if(n[me].immunity == 1){
            n[me].payoff += -g.vac_cost;
            covrg_each += 1.0;
        }
        n[me].ninf = 0;
    }

    g.coverage = covrg_each/g.n;

}