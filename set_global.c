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
	if ((argc < 8) || (argc > 9)) {
		fprintf(
		    stderr,
		    "usage: ./sir 1[nwk file] 2[beta] 3[initial coverage] \n"
		    "4[cost of vaccination] 5[probability of becoming rational] 6[length of memory] 7[filename] 8<seed>\n"
		 	);
		exit(1);
	}

	g.beta = atof(argv[2]);

	g.coverage = atof(argv[3]);
	if (g.coverage < 0 || g.coverage > 1) {
		fprintf(stderr, "Coverage (4th argv) should be 0 to 1.\n");
		exit(1);
	}

	g.vac_cost = atof(argv[4]);
	if (g.vac_cost < 0 || g.vac_cost > 1) {
		fprintf(stderr, "Vaccination cost (5th argv) should 0 to 1\n");
		exit(1);
	}

	g.degree_rationality = atof(argv[5]);
	if ((g.degree_rationality < 0) || (g.degree_rationality > 1)) {
		fprintf(stderr, "Degree of rationality (6th argv) should 0 to 1 \n");
		exit(1);
	}

	g.memory_length = atoi(argv[6]);
	if (g.memory_length > SEASONS - CUTOFF) {
		fprintf(stderr, "Memory too long, in current setting it should be less than %d \n", SEASONS - CUTOFF);
		exit(1);
	}

	if (argc == 9)
		g.state = (uint64_t)strtoull(argv[8], NULL, 10);
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
	
}
