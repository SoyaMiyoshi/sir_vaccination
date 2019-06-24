// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sir.h"

extern NODE *n;
extern GLOBALS g;

void read_data(FILE *fp) {
	unsigned int i, me, you;

	g.n = 0;

	// scan the system size
	while (2 == fscanf(fp, "%u %u\n", &me, &you)) {
		if (g.n < me) g.n = me;
		if (g.n < you) g.n = you;
	}

	g.n++;

	n = calloc(g.n, sizeof(NODE));

	rewind(fp);

	// scan the degrees
	while (2 == fscanf(fp, "%u %u\n", &me, &you)) {
		n[me].deg++;
		n[you].deg++;
	}

	// allocate adjacency lists
	for (i = 0; i < g.n; i++) {
		n[i].payoff = 0;
		n[i].nb = malloc(n[i].deg * sizeof(unsigned int));
		n[i].deg = 0;
	}

	rewind(fp);

	// fill adjacency lists
	while (2 == fscanf(fp, "%u %u\n", &me, &you)) {
		n[me].nb[n[me].deg++] = you;
		n[you].nb[n[you].deg++] = me;
	}
}
