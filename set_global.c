#include "sir.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern GLOBALS g;
extern NODE *n;

extern FILE *logfile;

void set_global(int argc, char *argv[]) {
	FILE *fp;

	// a help message
	if ((argc < 7) || (argc > 8)) {
		fprintf(
		    stderr,
		    "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
		    "4[cost of vaccination] 5[probability of becoming rational] 6[filename] <seed>\n"
		    "Note that 3[initial coverage]"
		    "refers to the proportion of the population "
		    "that "
		    "choose to vaccinate in initial stage\n"
		 	);
		exit(1);
	}

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

	g.probability_becoming_rational = atof(argv[5]);
	if (g.probability_becoming_rational < 0 || g.probability_becoming_rational > 1) {
		fprintf(stderr, "Probability of becoming rational should 0 to 1 \n");
		exit(1);
	}

	if (argc == 8)
		g.state = (uint64_t)strtoull(argv[7], NULL, 10);
	else
		pcg_init();
	
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

	g.numCf = 0;
	g.numRational = 0;

	for (int ind = 0; ind < SEASONS; ind++) {
		g.convergenceWatcher[ind] = 0;
	}
}
