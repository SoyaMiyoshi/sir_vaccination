#include "sir.h"
#include <math.h>
#include <stdlib.h>

extern NODE *n;
extern GLOBALS g;

void make_strategy(FILE *output){
    unsigned int me,i,you,count,successful,majority;
    int max;
    float pimit, pconf, pto1, covrg_each;
    covrg_each = 0;

    // choose strategy in reference to his neighbor
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


       //if (n[me].immunity == 1){
       // Probability that switch to 0

       /* swith_to_0 <= (successful.payoff, me.payoff, num of majority){
                if(successful.immunity == 0 && majority == 0){
                    p(imitate to successful) + p(follow the majority) - p(imitate to successful) * p(follow the majority);
                }
                if(successful.immunity == 1 && majority == 0){
                    ??

                }

           }*/

       //}


       /*if (n[me].immunity == 0){
       // Probability that switch to 1
       }*/

       pimit = 1/(1 + exp(-(max - n[me].payoff)/g.KI));
       fprintf(output, "max - n[me] = %f pimit(raw) = %f\n", max - n[me].payoff, pimit );
       pconf = 1/(1 + exp(-(count - g.fai * n[me].deg)/g.KC));

       if(n[successful].immunity == 0) pimit = (1 - pimit);
       if(majority == 0) pconf = (1 - pconf);

       pto1 = pimit + pconf - pimit*pconf;
       n[me].decision = GetRandomInt(pto1);

       fprintf(output,"Num of vaccinated around me = %d, I know %d neighbours, so majority is %d\n", count, n[me].deg, majority);
       fprintf(output, "pimit is %f coz %f, pconf is %f coz %f, pto1 is %f, therefore I do %d\n",
                pimit, max - n[me].payoff, pconf, count - g.fai * n[me].deg, pto1, n[me].decision);
    }

    fprintf(output, "---------\n");

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