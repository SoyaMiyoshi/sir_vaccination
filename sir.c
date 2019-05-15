// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include "sir.h"
#include <math.h>

GLOBALS g;
NODE *n;

int GetRandomInt(float trsh){
    float my_rand = pcg_16()*(1.0)/(1.0+UINT16_MAX);
    if(my_rand < trsh) return 1;
    else return 0;
}

float GetRandomFloat(){
    float my_rand = pcg_16()*(1.0)/(1.0+UINT16_MAX);
    return my_rand;
}

void infect () {
   unsigned int i, you, me = g.heap[1];
   float t, now = n[me].time;

   del_root();
   n[me].heap = I_OR_R;
   // get the recovery time
   n[me].time += g.rexp[pcg_16()] * g.beta; // bcoz g.rexpr has a / g.beta factor
   if (n[me].time > g.t) g.t = n[me].time;
   g.s++;

   // go through the neighbors of the infected node . .
   for (i = 0; i < n[me].deg; i++) {
      you = n[me].nb[i];
      if (n[you].heap != I_OR_R) { // if you is S, you can be infected

          if(n[you].immunity == 1){
             t = now + g.efficacy*g.rexp[pcg_16()]; // get the infection time
         }
         else{
             t = now + g.rexp[pcg_16()];
         }

         if ((t < n[me].time) && (t < n[you].time)) {
            n[you].time = t;
            if (n[you].heap == NONE) { // if not listed before, then extend the heap
               g.heap[++g.nheap] = you;
               n[you].heap = g.nheap;
               n[you].payoff = n[you].payoff - 1.0;
               n[you].ninf++;
            }
            up_heap(n[you].heap); // this works bcoz the only heap relationship that can be violated is the one between you and its parent
         }
      }
   }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// this routine runs one SIR outbreak from a random seed node

void sir () {
   unsigned int i, source;

   g.t = 0.0;
   g.s = 0;

   // initialize
   for (i = 0; i < g.n; i++) {
      n[i].heap = NONE;
      n[i].time = DBL_MAX; // to a large value
   }

   // get & infect the source
   while(true){
       source = pcg_32_bounded(g.n);
       if(n[source].immunity != 1) break;
   }

   n[source].time = 0.0;
   n[source].heap = 1;
   g.heap[g.nheap = 1] = source;

   // run the outbreak
   while (g.nheap) infect();
}

void imitate(){
    unsigned int me,i,you,successful,conf;
    int max;
    float pconf;

    // make decisions based on payoff comparison
    for(me = 0; me < g.n; me++){
       max = -100;
       successful = me;
       for(i = 0; i < n[me].deg; i++){
            you = n[me].nb[i];
            if(n[you].payoff > max){
                max = n[you].payoff;
                successful = you;
            }
       }
       pconf = 1/(1 + exp(-(max - n[me].payoff)/RATIONALITY));
       //printf("%f", pconf);
       conf = GetRandomInt(pconf);
       if(conf == 1) n[me].decision = n[successful].immunity;
       else n[me].decision = GetRandomInt(0.5);
       printf("successful neighbour %d, his vacc? %d, so I vacc? %d\n", successful, n[successful].immunity, n[me].decision);
    }

    // set immunity based on the decisions, and reset payoff
    for(me = 0; me < g.n; me++){
        n[me].immunity = n[me].decision;
        n[me].payoff = 0;
        if(n[me].immunity == 1){
            n[me].payoff += -VAC_COST;
        }
    }

}

int main (int argc, char *argv[]) {
   unsigned int i, j;
   FILE *fp;

   // just a help message
   if ((argc < 5) || (argc > 6)) {
      fprintf(stderr, "usage: ./sir [nwk file] [beta] [coverage] [efficacy] <seed>\n");
      return 1;
   }

   if (argc == 6) g.state = (uint64_t) strtoull(argv[5], NULL, 10);
   else pcg_init();

   g.beta = atof(argv[2]);

   g.coverage = atof(argv[3]);
   if(g.coverage < 0 || g.coverage > 1){
       fprintf(stderr, "Coverage should be 0 to 1.\n");
       return 1;
   }

   g.efficacy = atof(argv[4]);
   if(g.efficacy < 0 ){
       fprintf(stderr, "Efficacy should be greater than 0.\n");
       return 1;
   }

   fp = fopen(argv[1], "r");
   if (!fp) {
      fprintf(stderr, "can't open '%s'\n", argv[1]);
      return 1;
   }
   read_data(fp);
   fclose(fp);

   g.heap = malloc((g.n + 1) * sizeof(unsigned int));

   for (i = 0; i < 0x10000; i++)
      g.rexp[i] = -log((i + 1.0) / 0x10000) / g.beta;

    // initial simulation (coverage is user-entered)
    for(j = 0; j < g.n; j++){
        n[j].immunity = GetRandomInt(g.coverage);
        if(n[j].immunity == 1){
            n[j].payoff += -VAC_COST;
                }
        sir();
    }

   for (i = 0; i < NAVG - 1; i++) sir();
    for (i = 0; i < g.n ; i++) n[i].payoff = n[i].payoff / (float) NAVG;
   // print result
   for (i = 0; i < g.n; i++) printf("%u %g %g\n", i, n[i].ninf / (double) NAVG, n[i].payoff);

    for (i = 0; i < SEASONS; i++){
        imitate();

        for (i = 0; i < NAVG ; i++) sir();

        for (i = 0; i < g.n; i++) n[i].payoff = n[i].payoff / (float) NAVG;
    }
    printf("-----------------\n");
    for (i = 0; i < g.n; i++) printf("%u %g %g\n", i, n[i].ninf / (double) NAVG, n[i].payoff);

   // cleaning up
   for (i = 0; i < g.n; i++) free(n[i].nb);
   free(n); free(g.heap);

   return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
