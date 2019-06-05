// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include "sir.h"
#include <math.h>
#include <stdlib.h>

GLOBALS g;
NODE *n;

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

         t = now + g.rexp[pcg_16()];

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
   unsigned int i, source = 0;

   g.t = 0.0;
   g.s = 0;

   // initialize
   for (i = 0; i < g.n; i++) {
      n[i].heap = NONE;
      n[i].time = DBL_MAX; // to a large value
   }

   // get & infect the source
   for (i = 0; i < g.n; i++){
       source = pcg_32_bounded(g.n);
       if(n[source].immunity != 1) break;
       if(i == g.n) break;
   }

   n[source].time = 0.0;
   n[source].heap = 1;
   g.heap[g.nheap = 1] = source;

   // run the outbreak
   while (g.nheap) infect();
}



int main (int argc, char *argv[]) {
   unsigned int i, j;
   double st1 = 0.0, ss1 = 0.0;
   //double st2 = 0.0, ss2 = 0.0; // for averages

   FILE *fp;

   // just a help message
   if ((argc < 7) || (argc > 8)) {
      fprintf(stderr, "usage: ./sir 1[nwk file] 2[beta] 3[initial_coverage] 4[cost of vaccination] 5[fraction of conformist] 6[fraction of zealot] <seed>\n");
      return 1;
   }

   if (argc == 8) g.state = (uint64_t) strtoull(argv[7], NULL, 10);
   else pcg_init();

   g.beta = atof(argv[2]);

   g.coverage = atof(argv[3]);
   if(g.coverage < 0 || g.coverage > 1){
       fprintf(stderr, "Coverage should be 0 to 1.\n");
       return 1;
   }

   g.vac_cost = atof(argv[4]);
   if(g.vac_cost < 0 || g.vac_cost > 1  ){
       fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
       return 1;
   }

   g.conformist_fraction = atof(argv[5]);
       return 1;
   }

   g.zealot_fraction = atof(argv[6]);
   if(g.vac_cost < 0 || g.vac_cost > 1  ){
       fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
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

    // initial simulation (coverage is user-entered), first season
    for(j = 0; j < g.n; j++){
        n[j].immunity = GetRandomInt(g.coverage);
        if(n[j].immunity == 1) {
            n[j].payoff += -g.vac_cost;
        }
    }
    for (i = 0; i < NAVG; i++) {
        sir();
        ss1 += (double) g.s;
        //ss2 += SQ((double) g.s);
        st1 += g.t;
        //st2 += SQ(g.t);
    }

    ss1 /= NAVG;
    //ss2 /= NAVG;
    st1 /= NAVG;
    //st2 /= NAVG;

    printf("%g %g %f\n", ss1, st1, g.coverage);
    //printf("avg. outbreak size: %g \n", ss1);
    //printf("avg. time to extinction: %g \n", st1);

    for (i = 0; i < g.n ; i++) n[i].payoff = n[i].payoff / (float) NAVG;

    FILE *output;
    output = fopen("output/test.csv", "a+");
    // fprintf(output, "inverse rationality = %f, inverse conformity = %f , threshold = %f \n", g.KI, g.KC, g.fai);
    // fprintf(output, "P_imitate, P_conform, P \n");

    // from second seasons, each agent chooses his strategy wisely
    for (i = 1; i < SEASONS; i++){
        st1 = 0.0, ss1 = 0.0;
        // st2 = 0.0, ss2 = 0.0;
        make_strategy(output);

        for (j = 0; j < NAVG ; j++) {
            sir();
            ss1 += (double) g.s;
            //ss2 += SQ((double) g.s);
            st1 += g.t;
            //st2 += SQ(g.t);
        }
        ss1 /= NAVG;
        //ss2 /= NAVG;
        st1 /= NAVG;
        //st2 /= NAVG;

        printf("%g %g %f\n", ss1, st1, g.coverage);
        //printf("avg. outbreak size: %g \n", ss1);
        //printf("avg. time to extinction: %g \n", st1);

        for (j = 0; j < g.n; j++) n[j].payoff = n[j].payoff / (float) NAVG;
    }
    fclose(output);

   // cleaning up
   for (i = 0; i < g.n; i++) free(n[i].nb);
   free(n); free(g.heap);

   return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
