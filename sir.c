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

// Macros for conversion from int to string
#define CHAR_LEN 5

GLOBALS g;
NODE *n;

FILE *logfile;

void set_global(int argc, char *argv[]) {
	FILE *fp;

	// a help message
	if ((argc < 8) || (argc > 9)) {
		fprintf(
		    stderr,
		    "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
		    "4[cost of vaccination] 5[proportion of conformist] "
		    "6[proportion of zealot] 7[filename] <seed>\n"
		    "Note that 3[initial coverage] and 5[proportion of "
		    "conformist] refers to the proportion of the population "
		    "that "
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

	g.conformist_proportion = atof(argv[5]);
	if (g.conformist_proportion < 0 || g.conformist_proportion > 1) {
		fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
		exit(1);
	}

	g.zealot_proportion = atof(argv[6]);
	if (g.zealot_proportion < 0 || g.zealot_proportion > 1) {
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

void set_characteristics() {
	for (unsigned int j = 0; j < g.n; j++) {
		n[j].is_zealot = get_random_int(g.zealot_proportion);

		if (n[j].is_zealot == 1) {
			n[j].immunity = 0;
			n[j].decision = 0;
			g.numZl++;
		}

		if (n[j].is_zealot == 0) {
			n[j].is_conformist =
			    get_random_int(g.conformist_proportion);
			if (n[j].is_conformist == 1) {
				g.numCf++;
			} else {
				g.numImt++;
			}
			n[j].immunity = get_random_int(g.coverage);
			if (n[j].immunity == 1) {
				n[j].payoff = -g.vac_cost;
				n[j].payoff_each = -g.vac_cost;
			}
		}
	}
}

void create_dir_and_file(char *log_dirname, char *log_filename, char *argv[]) {
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
	snprintf(cfs, CHAR_LEN, "%2.2f\n", g.conformist_proportion);
	snprintf(zfs, CHAR_LEN, "%2.2f\n", g.zealot_proportion);

	if (strcmp(argv[7], b) == 0) {
		// fprintf(stderr,"Vary Beta\n");
		sprintf(log_dirname, "log/c=%s:v=%s:cf=%s:zf=%s", cs, vs, cfs,
			zfs);
		sprintf(log_filename, "log/c=%s:v=%s:cf=%s:zf=%s/b=%s.txt", cs,
			vs, cfs, zfs, bs);
	} else if (strcmp(argv[7], c) == 0) {
		// fprintf(stderr,"Vary Initial Coverage\n");
		sprintf(log_dirname, "log/b=%s:v=%s:cf=%s:zf=%s", bs, vs, cfs,
			zfs);
		sprintf(log_filename, "log/b=%s:v=%s:cf=%s:zf=%s/c=%s.txt", bs,
			vs, cfs, zfs, cs);
	} else if (strcmp(argv[7], v) == 0) {
		// fprintf(stderr, "Vary Vaccination Cost\n");
		sprintf(log_dirname, "log/b=%s:c=%s:cf=%s:zf=%s", bs, cs, cfs,
			zfs);
		sprintf(log_filename, "log/b=%s:c=%s:cf=%s:zf=%s/v=%s.txt", bs,
			cs, cfs, zfs, vs);
	} else if (strcmp(argv[7], cf) == 0) {
		// fprintf(stderr,"Vary Fraction of Conformist\n");
		sprintf(log_dirname, "log/b=%s:c=%s:v=%s:zf=%s", bs, cs, vs,
			zfs);
		sprintf(log_filename, "log/b=%s:c=%s:v=%s:zf=%s/cf=%s.txt", bs,
			cs, vs, zfs, cfs);
	} else if (strcmp(argv[7], zf) == 0) {
		// fprintf(stderr,"Vary Fraction of Zealot\n");
		sprintf(log_dirname, "log/b=%s:c=%s:v=%s:cf=%s", bs, cs, vs,
			cfs);
		sprintf(log_filename, "log/b=%s:c=%s:v=%s:cf=%s/zf=%s.txt", bs,
			cs, vs, cfs, zfs);
	} else {
		printf("argv[7] should be 'b', 'c', 'v', 'cf', or 'zf'.\n");
	}

	if (stat(log_dirname, &st) == -1) {
		mkdir(log_dirname, 0700);
	}
}

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
		    n[you].immunity == 0) {  // if you is S, and not immune, you
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
	unsigned int i, source_temp = 0;
	int source = -1;

	g.t = 0.0;
	g.s = 0;

	for (i = 0; i < g.n; i++) {
		n[i].heap = NONE;
		n[i].time = DBL_MAX;  // to a large value
	}

	source_temp = pcg_32_bounded(g.n);

	if (n[source_temp].immunity) {
		fprintf(logfile, "Chosen source is immune!\n");
		return;
	}

	else {
		fprintf(logfile, "Chosen source %d\n", source_temp);
		source = source_temp;
		n[source].payoff = n[source].payoff - 1;
		n[source].payoff_each = n[source].payoff_each - 1;
		n[source].ninf++;

		n[source].time = 0.0;
		n[source].heap = 1;
		g.heap[g.nheap = 1] = source;

		while (g.nheap) infect();
	}
}

void calculate_outbreaksize_and_timetoext() {
	g.ss1 += (double)g.s;
	g.ss2 += SQ((double)g.s);  // stderr を計算するため
	g.st1 += g.t;
	g.st2 += SQ(g.t);
}

void calculate_payoff_each_group() {
	float pfAllAvg_temp = 0.0;
	float pfImtAvg_temp = 0.0;
	float pfConfAvg_temp = 0.0;
	float pfZlAvg_temp = 0.0;

	for (unsigned int ind = 0; ind < g.n; ind++) {
		pfAllAvg_temp += n[ind].payoff_each;

		if (n[ind].is_zealot) {
			pfZlAvg_temp += n[ind].payoff_each;
		} else {
			if (n[ind].is_conformist) {
				pfConfAvg_temp += n[ind].payoff_each;
			} else {
				pfImtAvg_temp += n[ind].payoff_each;
			}
		}

		/* fprintf(logfile, "I am %d, immu? %d, Z? %d, Conf? %d, Pay_ec
		  %f Pay_tot %f \n", ind, n[ind].immunity, n[ind].is_zealot,
		  n[ind].is_conformist, n[ind].payoff_each, n[ind].payoff);*/

		if (n[ind].immunity == 0) {
			n[ind].payoff_each = 0;
		}
		// fprintf(logfile, "I am %d, Conf? %d, Payoff_each %f \n", l,
		// n[l].is_conformist, n[l].payoff_each);
	}

	g.pfAllAvg += pfAllAvg_temp / g.n;
	g.pfZlAvg += pfZlAvg_temp / g.numZl;
	g.pfConfAvg += pfConfAvg_temp / g.numCf;
	g.pfImtAvg += pfImtAvg_temp / g.numImt;

	g.pfAllSqdAvg += SQ(pfAllAvg_temp / g.n);
	g.pfZlSqdAvg += SQ(pfZlAvg_temp / g.numZl);
	g.pfConfSqdAvg += SQ(pfConfAvg_temp / g.numCf);
	g.pfImtSqdAvg += SQ(pfImtAvg_temp / g.numImt);
}

void run_simulation() {
	sir();
	calculate_outbreaksize_and_timetoext();
	calculate_payoff_each_group();
}

void reset_result_each_season() {
	g.ss1 = 0;
	g.ss2 = 0;
	g.st1 = 0;
	g.st2 = 0;  // for averages

	g.pfAllAvg = 0;
	g.pfAllSqdAvg = 0;
	g.pfImtAvg = 0;
	g.pfImtSqdAvg = 0;
	g.pfConfAvg = 0;
	g.pfConfSqdAvg = 0;
	g.pfZlAvg = 0;
	g.pfZlSqdAvg = 0;
}

void calculate_payff_for_each_agent() {
	for (unsigned int ind = 0; ind < g.n; ind++) {
		if (n[ind].immunity != 1) {
			n[ind].payoff = n[ind].payoff / (float)NAVG;
		}
	}
}

void finalize_result_each_season() {
	g.ss1 /= NAVG;
	g.ss2 /= NAVG;
	g.st1 /= NAVG;
	g.st2 /= NAVG;

	g.pfAllAvg /= NAVG;
	g.pfZlAvg /= NAVG;
	g.pfConfAvg /= NAVG;
	g.pfImtAvg /= NAVG;
	g.pfAllSqdAvg /= NAVG;
	g.pfZlSqdAvg /= NAVG;
	g.pfConfSqdAvg /= NAVG;
	g.pfImtSqdAvg /= NAVG;
}

void print_result(float coverage) {
	printf("%g %g ", g.ss1,
	       sqrt((g.ss2 - SQ(g.ss1)) /
		    (NAVG - 1)));  // outbreaksize (avg, stderr)
	printf("%g %g ", g.st1,
	       sqrt((g.st2 - SQ(g.st1)) /
		    (NAVG - 1)));  // time to extinction (avg, stderr)
	printf("%g %g ", g.pfAllAvg,
	       sqrt((g.pfAllSqdAvg - SQ(g.pfAllAvg)) / (NAVG - 1)));
	printf("%g %g ", g.pfZlAvg,
	       sqrt((g.pfZlSqdAvg - SQ(g.pfZlAvg)) / (NAVG - 1)));
	printf("%g %g ", g.pfConfAvg,
	       sqrt((g.pfConfSqdAvg - SQ(g.pfConfAvg)) / (NAVG - 1)));
	printf("%g %g ", g.pfImtAvg,
	       sqrt((g.pfImtSqdAvg - SQ(g.pfImtAvg)) / (NAVG - 1)));
	printf("%f \n", coverage);
}

int main(int argc, char *argv[]) {
	set_global(argc, argv);

	set_characteristics();

	char log_dirname[100];
	char log_filename[100];
	create_dir_and_file(log_dirname, log_filename, argv);

	logfile = fopen(log_filename, "w");

	for (int run = 0; run < SEASONS + 1; run++) {
		reset_result_each_season();

		for (int k = 0; k < NAVG; k++) {
			fprintf(logfile,
				"~~~~~~~ Season %d, Sim %d th ~~~~~~~ \n", run,
				k);
			run_simulation();
		}

		calculate_payff_for_each_agent();

		finalize_result_each_season();

		if (run == 0) {
			print_result(g.coverage * (1 - g.zealot_proportion));
		} else {
			print_result(g.coverage);
		}

		make_strategy();
	}

	fclose(logfile);

	for (unsigned int re = 0; re < g.n; re++) free(n[re].nb);
	free(n);
	free(g.heap);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
