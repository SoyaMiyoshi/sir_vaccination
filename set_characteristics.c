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

void set_characteristics() {
	for (unsigned int j = 0; j < g.n; j++) {
		n[j].is_zealot = get_random_int(g.zealot_proportion);

		if (n[j].is_zealot == 1) {
			n[j].immune = 0;
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
			n[j].immune = get_random_int(g.coverage);
			if (n[j].immune == 1) {
				n[j].payoff = -g.vac_cost;
				n[j].payoff_each = -g.vac_cost;
			}
		}
	}
}