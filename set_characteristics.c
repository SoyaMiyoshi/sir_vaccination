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

void make_decision(unsigned int index) {

	unsigned int you;
	
	if (n[index].nature == Conforming) {
			g.numCf++;
			unsigned int count = 0;

			if (n[index].immune == 1) {
				count += 1;
			}

			for (unsigned int i = 0; i < n[index].deg; i++) {
				you = n[index].nb[i];
				if (n[you].immune == 1) count++;
			}

			if (n[index].deg + 1 > 2 * count) {
				n[index].decision = 0;
			}

			if (n[index].deg + 1 == 2 * count) {
				n[index].decision = n[index].immune;
			}

			if (n[index].deg + 1 < 2 * count) {
				n[index].decision = 1;
			}
		}

	if (n[index].nature == Rational) {
			g.numRational++;

			float max = n[index].payoff;
			int successful = index;

			for (unsigned int i = 0; i < n[index].deg; i++) {
				you = n[index].nb[i];
				if (n[you].payoff > max) {
					max = n[you].payoff;
					successful = you;
				}
			}
			n[index].decision = n[successful].immune;
		}
		
	if (get_one_or_zero_randomly(0.01)) {
            n[index].decision = !n[index].decision;
		}
}

void set_characteristics_randomly() {
	initialize_sensus();
	for (unsigned int j = 0; j < g.n; j++) {
		if(get_one_or_zero_randomly(g.degree_rationality)){
			n[j].nature = Rational;
		}
		else{
			n[j].nature = Conforming;
		}
		make_decision(j);
	}
}

void set_characteristics_using_memory() {
	initialize_sensus();
	for (unsigned int index = 0; index < g.n; index++) {
		
		// Forget the oldest memory
		n[index].head = removeHeadFromLink(n[index].head);

		// Form characteristics based on memory
		struct oneMemory * ref  = malloc(sizeof(struct oneMemory));
		float payoff_rational = 0.0;
		int num_rational = 0;
		float payoff_conforming = 0.0;
		int num_conforming = 0;

		ref = n[index].head -> next;
		while (ref != n[index].tail) {

			if (ref -> nature == Rational) {
				num_rational ++;
				payoff_rational += ref -> payoff;
			}

			else {
				num_conforming ++;
				payoff_conforming += ref -> payoff;
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
			if (payoff_rational/ num_rational > payoff_conforming/ num_conforming) {
				n[index].nature = Rational;
			} else {
				n[index].nature = Conforming;
			}
		}

		if (get_one_or_zero_randomly(0.01)) {
			if (n[index].nature == Conforming) {
				n[index].nature = Rational;
			}
			else {
				n[index].nature = Conforming;
			}
		}

		make_decision(index);
	}
}