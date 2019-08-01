// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include "sir.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

// Macros for conversion from int to string
#define CHAR_LEN 5

GLOBALS g;
NODE *n;

void printResult(double ss1, double ss2, double st1, double st2, float coverage) {
  
  printf("%g %g ", ss1, sqrt((ss2 - SQ(ss1)) / (NAVG - 1))); // outbreaksize (avg, stderr)
	printf("%g %g ", st1, sqrt((st2 - SQ(st1)) / (NAVG - 1))); // time to extinction (avg, stderr)
  printf("%f", coverage);

}

void setGlobals(int argc, char *argv[]){
    FILE *fp;

  // a help message
  if ((argc < 8) || (argc > 9)) {
    fprintf(stderr, "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
                    "4[cost of vaccination] 5[fraction of conformist] "
                    "6[fraction of zealot] 7[filename] <seed>\n"
                    "Note that 3[initial coverage] and 5[fraction of "
                    "conformist] refers to the fraction of the population that "
                    "belongs to each class\n"
                    "EXCLUDING the zealot.\n"
                    "For example, if 10 percent of the population is zealot, "
                    "then out of the rest (90 percent)*(initial coverage) "
                    "percent would get vaccination.\n");
  }

  if (argc == 9)
    g.state = (uint64_t)strtoull(argv[8], NULL, 10);
  else
    pcg_init();

  g.beta = atof(argv[2]);

  g.coverage = atof(argv[3]);
  if (g.coverage < 0 || g.coverage > 1) {
    fprintf(stderr, "Coverage should be 0 to 1.\n");
    exit(1);
  }

  g.vac_cost = atof(argv[4]);
  if (g.vac_cost < 0 || g.vac_cost > 1) {
    fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
    exit(1);
  }

  g.conformist_fraction = atof(argv[5]);
  if (g.vac_cost < 0 || g.vac_cost > 1) {
    fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
    exit(1);
  }

  g.zealot_fraction = atof(argv[6]);
  if (g.vac_cost < 0 || g.vac_cost > 1) {
    fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
    exit(1);
  }

  fp = fopen(argv[1], "r");
  if (!fp) {
    fprintf(stderr, "can't open '%s'\n", argv[1]);
    exit(1);
  }
  read_data(fp);
  fclose(fp);

  g.heap = malloc((g.n + 1) * sizeof(unsigned int));

  for (int i = 0; i < 0x10000; i++) {
    g.rexp[i] = -log((i + 1.0) / 0x10000) / g.beta;
  }

  g.numZl = 0;
	g.numCf = 0;
	g.numImt = 0;
}

void setCharacteristics(){
    for (unsigned int j = 0; j < g.n; j++) {
    n[j].is_zealot = GetRandomInt(g.zealot_fraction);

    if (n[j].is_zealot == 1) {
      n[j].immunity = 0;
      n[j].decision = 0;
      g.numZl++;
    }

    if (n[j].is_zealot == 0) {
      n[j].is_conformist = GetRandomInt(g.conformist_fraction);
      if (n[j].is_conformist == 1){
        g.numCf++;
      }
      else{
        g.numImt++;
      }
      n[j].immunity = GetRandomInt(g.coverage);
      if (n[j].immunity == 1) {
        n[j].payoff += -g.vac_cost;
        n[j].payoff_each += -g.vac_cost;
      }
    }
  }
}

void createDirAndFile(char *log_dirname, char *log_filename, char *argv[]){
  char *b = "b";
  char *c = "c";
  char *v = "v";
  char *cf = "cf";
  char *zf = "zf";
  char bs[CHAR_LEN] = {'\0'};
  char cs[CHAR_LEN] = {'\0'};
  char vs[CHAR_LEN] = {'\0'};
  char cfs[CHAR_LEN] = {'\0'};
  char zfs[CHAR_LEN] = {'\0'};

  struct stat st = {0};

  snprintf(bs, CHAR_LEN, "%2.2f\n", g.beta);
  snprintf(cs, CHAR_LEN, "%2.2f\n", g.coverage);
  snprintf(vs, CHAR_LEN, "%2.2f\n", g.vac_cost);
  snprintf(cfs, CHAR_LEN, "%2.2f\n", g.conformist_fraction);
  snprintf(zfs, CHAR_LEN, "%2.2f\n", g.zealot_fraction);

  if (strcmp(argv[7], b) == 0){
    //fprintf(stderr,"Vary Beta\n");
    sprintf(log_dirname,"log/c=%s:v=%s:cf=%s:zf=%s",cs, vs, cfs, zfs);
    sprintf(log_filename, "log/c=%s:v=%s:cf=%s:zf=%s/b=%s.txt",cs, vs, cfs, zfs, bs);
  } else if (strcmp(argv[7], c) == 0){
    //fprintf(stderr,"Vary Initial Coverage\n");
    sprintf(log_dirname,"log/b=%s:v=%s:cf=%s:zf=%s",bs, vs, cfs, zfs);
    sprintf(log_filename, "log/b=%s:v=%s:cf=%s:zf=%s/c=%s.txt",bs, vs, cfs, zfs, cs);
  } else if (strcmp(argv[7], v) == 0){
    //fprintf(stderr, "Vary Vaccination Cost\n");
    sprintf(log_dirname,"log/b=%s:c=%s:cf=%s:zf=%s",bs, cs, cfs, zfs);
    sprintf(log_filename, "log/b=%s:c=%s:cf=%s:zf=%s/v=%s.txt",bs, cs, cfs, zfs, vs);
  } else if (strcmp(argv[7], cf) == 0){
    //fprintf(stderr,"Vary Fraction of Conformist\n");
    sprintf(log_dirname,"log/b=%s:c=%s:v=%s:zf=%s",bs, cs, vs, zfs);
    sprintf(log_filename, "log/b=%s:c=%s:v=%s:zf=%s/cf=%s.txt",bs, cs, vs, zfs, cfs);
  } else if (strcmp(argv[7], zf) == 0){
    //fprintf(stderr,"Vary Fraction of Zealot\n");
    sprintf(log_dirname,"log/b=%s:c=%s:v=%s:cf=%s",bs, cs, vs, cfs);
    sprintf(log_filename, "log/b=%s:c=%s:v=%s:cf=%s/zf=%s.txt",bs, cs, vs, cfs, zfs);
  } else{
    printf("argv[7] should be 'b', 'c', 'v', 'cf', or 'zf'.\n");
  }

  if (stat(log_dirname, &st) == -1) {
    mkdir(log_dirname, 0700);
  }
}

void infect(FILE *logfile) {
  unsigned int i, you, me = g.heap[1];
  float t, now = n[me].time;

  del_root();
  n[me].heap = I_OR_R;
  // get the recovery time
  n[me].time += g.rexp[pcg_16()] * g.beta; // bcoz g.rexpr has a / g.beta factor
  if (n[me].time > g.t)
    g.t = n[me].time;
  g.s++;

  // go through the neighbors of the infected node . .
  for (i = 0; i < n[me].deg; i++) {
    you = n[me].nb[i];

    if (n[you].heap != I_OR_R &&
        n[you].immunity == 0) { // if you is S, and not immune, you
                                // can be infected.

      t = now + g.rexp[pcg_16()];

      if ((t < n[me].time) && (t < n[you].time)) {
        n[you].time = t;
        if (n[you].heap == NONE) { // if not listed before, then
                                   // extend the heap
          g.heap[++g.nheap] = you;
          n[you].heap = g.nheap;
          n[you].payoff = n[you].payoff - 1.0;
          n[you].payoff_each += -1;
          n[you].ninf++;
          // fprintf(logfile, "oops, node %d get infected!\n", you);
        }
        up_heap(n[you].heap); // this works bcoz the only
                              // heap relationship that can
                              // be violated is the one
                              // between you and its parent
      }
    }
  }
}

void sir(FILE *logfile) {
  unsigned int i, source = 0;

  g.t = 0.0;
  g.s = 0;

  // initialize
  for (i = 0; i < g.n; i++) {
    n[i].heap = NONE;
    n[i].time = DBL_MAX; // to a large value
  }

  // get & infect the source
  for (i = 0; i < g.n; i++) {
    source = pcg_32_bounded(g.n);
    if (n[source].immunity != 1) {
      n[source].payoff = n[source].payoff - 1;
      n[source].ninf ++;
      //fprintf(logfile, "source is %d \n", source);
      break;
    }
    if (i == g.n)
      break;
  }

  n[source].time = 0.0;
  n[source].heap = 1;
  g.heap[g.nheap = 1] = source;

  // run the outbreak
  while (g.nheap)
    infect(logfile);
}

int main(int argc, char *argv[]) {
  unsigned int i, j;
  double st1 = 0.0, ss1 = 0.0;
  double st2 = 0.0, ss2 = 0.0; // for averages

  float payoffAllAvg = 0.0, payoffAllSqdAvg = 0.0;
  float payoffImtAvg = 0.0, payoffImtSqdAvg = 0.0;
  float payoffConfAvg = 0.0, payoffConfSqdAvg = 0.0;
  float payoffZlAvg = 0.0, payoffZlSqdAvg = 0.0;

  float payoffAllAvg_temp = 0.0;
  float payoffImtAvg_temp = 0.0;
  float payoffConfAvg_temp = 0.0;
  float payoffZlAvg_temp = 0.0;

  setGlobals(argc, argv);
  
  setCharacteristics();

  char log_dirname[100];
  char log_filename[100];
  createDirAndFile(log_dirname, log_filename, argv);

  FILE *logfile;
  logfile = fopen(log_filename, "w");
 
  for (int k = 0; k < NAVG; k++) {
    sir(logfile);
    ss1 += (double)g.s;
    ss2 += SQ((double) g.s); // stderr を計算するため
    st1 += g.t;
    st2 += SQ(g.t);

    for (i = 0; i < g.n; i++) {

      payoffAllAvg_temp += n[i].payoff_each;

      for (i = 0; i < g.n; i++) {
      if (n[i].is_zealot) {
        payoffZlAvg_temp += n[i].payoff_each;
      } else {
      if (n[i].is_conformist) {
        payoffConfAvg_temp += n[i].payoff_each;
      } else {
        payoffImtAvg_temp += n[i].payoff_each;
            } 
          } 
      if(n[i].immunity == 0){
        payoff_each = 0;
        }
      }

    }

    payoffAllAvg += payoffAllAvg_temp/g.n;
    payoffZlAvg += payoffZlAvg_temp/g.numZl;
    payoffConfAvg += payoffConfAvg_temp/g.numCf;
    payoffImtAvg += payoffImtAvg_temp/g.numImt;
    
    payoffAllSqdAvg += SQ(payoffAllAvg_temp/g.n);
    payoffZlSqdAvg += SQ(payoffZlAvg_temp/g.numZl);
    payoffConfSqdAvg += SQ(payoffConfAvg_temp/g.numCf);
    payoffImtSqdAvg += SQ(payoffImtAvg_temp/g.numImt);
  }

  for (i = 0; i < g.n; i++) {
    if (n[i].immunity != 1) {
      n[i].payoff = n[i].payoff / (float)NAVG;
    }
  }

  ss1 /= NAVG;
  ss2 /= NAVG;
  st1 /= NAVG;
  st2 /= NAVG;

  payoffAllAvg /= NAVG;
  payoffZlAvg /= NAVG;
  payoffConfAvg /= NAVG;
  payoffImtAvg /= NAVG;
  
  payoffAllSqdAvg /= NAVG;
  payoffZlSqdAvg /= NAVG;
  payoffConfSqdAvg /= NAVG;
  payoffImtSqdAvg /= NAVG;

  // User-entered "coverage" is "coverage excluding zealot"

  printf("%f %f\n", payoffAllAvg, SQ( (payoffAllSqdAvg - SQ(payoffAllAvg))/ (NAVG - 1) ));

  // printResult(ss1, ss2, st1, st2, g.coverage * (1 - g.zealot_fraction) );

  // from second seasons, each agent chooses his strategy wisely
  for (i = 1; i < SEASONS; i++) {
    st1 = 0.0, ss1 = 0.0;
    st2 = 0.0, ss2 = 0.0;

    make_strategy(logfile);

    for (j = 0; j < NAVG; j++) {
      sir(logfile);
      ss1 += (double)g.s;
      ss2 += SQ((double) g.s);
      st1 += g.t;
      st2 += SQ(g.t);
    }
    ss1 /= NAVG;
    ss2 /= NAVG;
    st1 /= NAVG;
    st2 /= NAVG;

    for (j = 0; j < g.n; j++) {
      if (n[j].immunity != 1) {
        n[j].payoff = n[j].payoff / (float)NAVG;
      }
    }

    //printResult(ss1, ss2, st1, st2, g.coverage);

  }
  fclose(logfile);

  for (i = 0; i < g.n; i++) free(n[i].nb);
  free(n);
  free(g.heap);

  return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
