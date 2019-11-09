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

// extern FILE *logfile;

void initialize_sensus() {
	g.numCf = 0;
	g.numRational = 0;
}

void vaccinate_everyone() {
	for (unsigned int j = 0; j < g.n; j++) {
		n[j].immune = get_one_or_zero_randomly(g.coverage);
	}
}

void set_characteristics_randomly() {
	initialize_sensus();
	for (unsigned int j = 0; j < g.n; j++) {
		if(get_one_or_zero_randomly(g.degree_rationality)){
			n[j].nature = Rational;
			g.numRational ++;
		}
		else{
			n[j].nature = Conforming;
			g.numCf ++;
		}
	}
}

// This function to model the spread of conformism 
// To do : which of the parametar spreads conformism in this setting if modelled this way? 
// Actually referring to the same data each season is inefficient.
void set_characteristics_memory_based() {
	initialize_sensus();
	for (unsigned int index = 0; index < g.n; index++) {
		struct oneMemory * ref  = malloc(sizeof(struct oneMemory));

		float experience_rational = 0.0;
		int num_rational = 0;
		float experience_conforming = 0.0;
		int num_conforming = 0;

		ref = n[index].head -> next;
		while (ref != n[index].tail) {

			if (ref -> nature == Rational) {
				num_rational ++;
				experience_rational += ref -> payoff;
			}

			else {
				num_conforming ++;
				experience_conforming += ref -> payoff;
			}
			ref = ref -> next;
    	}

		if (num_rational == 0) {
			n[index].nature = Conforming;
		}
		
		if (num_conforming == 0) {
			n[index].nature = Rational;
		}

		if (num_rational != 0 && num_conforming != 0) {
			if (experience_rational / num_rational > experience_conforming/ num_conforming) {
				n[index].nature = Rational;
			} else {
				n[index].nature = Conforming;
			}
		}

		// mutation
		if (get_one_or_zero_randomly(0.01)) {
			if (n[index].nature == Conforming) {
				n[index].nature = Rational;
			}
			else {
				n[index].nature = Conforming;
			}
		}

		if (n[index].nature == Conforming) {
			g.numCf++;
		} else {
			g.numRational++;
		}

	}
}