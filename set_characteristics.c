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

void vaccinate_everyone() {
	for (unsigned int j = 0; j < g.n; j++) {
		n[j].immune = get_random_int(g.coverage);
	}
}

void set_characteristics_randomly() {
	for (unsigned int j = 0; j < g.n; j++) {
		if(get_random_int(0.5)){
			n[j].nature = Conforming;
		}
		else{
			n[j].nature = Rational;
		}
	}
}

void set_characteristics_memory_based() {
	for (unsigned int index = 0; index < g.n; index++) {
		struct oneMemory * ref  = malloc(sizeof(struct oneMemory));

		ref = n[index].head -> next;
		float experience = 0;

		while (ref != n[index].tail) {
			experience += (ref -> nature) * (ref -> payoff);
			ref = ref -> next;
    	}

		if (experience > 0) {
			n[index].nature = Rational;
		} else {
			n[index].nature = Conforming;
		}
	}
}