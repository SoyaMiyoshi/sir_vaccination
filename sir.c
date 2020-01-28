#include "sir.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define DEBUG_FLAG 1

#ifdef DEBUG_FLAG
#include <assert.h>
#define debug_array_size 100000
int n_infs[debug_array_size];
#endif

GLOBALS g;
NODE *n;
RECORD record;

// FILE *logfile;

void infect() {
	unsigned int i, you, me = g.heap[1];
	double t, now = n[me].time;

	del_root();
	n[me].heap = I_OR_R;
	n[me].time +=
	    g.rexp[pcg_16()] * g.beta;  // bcoz g.rexpr has a / g.beta factor
	if (n[me].time > g.t) g.t = n[me].time;
	g.s++;

	for (i = 0; i < n[me].deg; i++) {
		you = n[me].nb[i];

		if (n[you].heap != I_OR_R &&
		    n[you].immune == 0) {  // if you is S, and not immune, you
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
				}
				up_heap(n[you].heap);
			}
		}
	}
}

void sir() {
	unsigned int i, source = 0;

	g.t = 0.0;
	g.s = 0;

	source = pcg_32_bounded(g.n);

	if (n[source].immune) {
		return;
	}

	else {
		for (i = 0; i < g.n; i++) {
			n[i].heap = NONE;
			n[i].time = DBL_MAX;  // to a large value
		}

		n[source].payoff = n[source].payoff - 1;

		n[source].time = 0.0;
		n[source].heap = 1;
		g.heap[g.nheap = 1] = source;

		while (g.nheap) infect();

		// test if payoff is subtracted corectly from infected nodes
		#if DEBUG_FLAG
			for (i = 0; i < g.n; i++) {
				if (n[i].time != DBL_MAX) {
					n_infs[i]++;
				}
				if (!n[i].immune) {
					assert(fabs(n[i].payoff + n_infs[i]) < 0.001);
				}
			}
		#endif

	}
}

void make_decision(int me) {
	unsigned int you;

	if (n[me].nature == Conforming) {	
		unsigned int count = 0;

		if (n[me].immune == 1) {
			count += 1;
		}

		for (unsigned int i = 0; i < n[me].deg; i++) {
			you = n[me].nb[i];
			if (n[you].immune == 1) count++;
		}

		if (n[me].deg + 1 > 2 * count) {
			n[me].decision = 0;
		}

		if (n[me].deg + 1 == 2 * count) {
			n[me].decision = n[me].immune;
		}
		if (n[me].deg + 1 < 2 * count) {
			n[me].decision = 1;
		}	

	}

	if (n[me].nature == Rational) {
		double max = n[me].payoff;
		int successful = me;

		for (unsigned int i = 0; i < n[me].deg; i++) {
			you = n[me].nb[i];
			if (n[you].payoff > max) {
				max = n[you].payoff;
				successful = you;
			}
		}

		n[me].decision = n[successful].immune;
	}

}

void vaccinate() {
	double covrg_each = 0.0;

	for (unsigned int me = 0; me < g.n; me++) {

		// test if decision making is correcly done
		#if DEBUG_FLAG
			if(me == 0 ) {
				if(n[me].nature == Conforming) {
					unsigned int count = 0;
					for (unsigned int i = 0; i < n[me].deg; i++) {
						int you = n[me].nb[i];
						if (n[you].immune == 1) count++;
					}
					if(n[me].immune) count ++;

					if(n[me].decision) assert(count*2 >= n[me].deg + 1);
					if(!n[me].decision) assert(count*2 < n[me].deg + 1);
				}
			}
		#endif

		n[me].immune = n[me].decision;

		if (n[me].immune == 1) {
			n[me].payoff = -g.vac_cost;
			covrg_each += 1.0;
		}
		else {
			n[me].payoff = 0;
		}	
	}
	g.coverage = covrg_each / g.n;

	// test if payoff is subtracted correctly for the vaccinated nodes
	#if DEBUG_FLAG
		double tsp = 0.0;
		for (unsigned int me = 0; me < g.n; me++) {
			tsp += n[me].payoff;
		}
		assert(fabs(covrg_each*g.vac_cost - (-1)*tsp) < 0.001);
		assert(g.coverage <= 1);
		assert(0 <= g.coverage );
	#endif

}

void develop_nature(unsigned int index) {

	// test if memory length is short here
	#if DEBUG_FLAG
		if(index == 0) {
			oneMemory *ref;
			double payoff_conforming = 0;
			double payoff_rational = 0;
			int count = 1;

			ref = n[0].head ;
			while (ref != n[0].tail ) {
				if (ref -> nature == Conforming) {
					payoff_conforming += ref -> payoff;
				}
							
				if (ref -> nature == Rational) {
					payoff_rational += ref -> payoff;
				}
				count++;
				ref = ref -> next;
			}
			assert(count == g.memory_length);
			if(payoff_conforming && n[index].storage -> payoff_conforming) assert(fabs(payoff_conforming - n[index].storage -> payoff_conforming) < 0.01);
			if(payoff_rational && n[index].storage -> payoff_rational) assert(fabs(payoff_rational - n[index].storage -> payoff_rational) < 0.01);
		}
	#endif	

	n[index].tail = addToLink(n[index].tail, n[index].payoff, n[index].nature);

	Storage *str = n[index].storage;
	if(n[index].nature == Conforming) {
		str -> payoff_conforming += n[index].payoff;
		str -> num_conforming += 1;

	}
	if(n[index].nature == Rational) {
		str -> payoff_rational += n[index].payoff;
		str -> num_rational += 1;
	}

	if(str->num_conforming !=0 && str->num_rational!=0) {
		if(str->payoff_rational/str->num_rational <= str->payoff_conforming/str->num_conforming ) {
			n[index].nature = Conforming;
		} else {
			n[index].nature = Rational;
		}
	}

	// test if nature determined by storage is consistent with LL. 
	#if DEBUG_FLAG
		oneMemory *ref;
		double payoff_conforming = 0.0;
		double payoff_rational = 0.0;
		int count_conforiming = 0;
		int count_rational = 0;

		ref = n[index].head ;
		while ( ref != n[index].tail ) {
			if (ref -> nature == Conforming) {
				payoff_conforming += ref -> payoff;
			}
						
			if (ref -> nature == Rational) {
				payoff_rational += ref -> payoff;
			}
			ref = ref -> next;
		}

		if((count_conforiming) && (count_conforiming)) {
			if((payoff_rational/count_rational) < (payoff_conforming/count_conforiming)) {
				assert(n[index].nature == Conforming);
			} else {
				assert(n[index].nature == Rational);
			}
		}
	#endif

	// test if memory became long here, also if Storage is consistent with LL.
	#if DEBUG_FLAG
		if ( index == 0) {
			oneMemory *ref;
			double payoff_conforming = 0.0;
			double payoff_rational = 0.0;
			int count = 0;

			ref = n[0].head ;
			while ( ref != n[0].tail ) {
				if ( ref -> nature == Conforming ) {
					payoff_conforming += ref -> payoff;
				}
							
				if ( ref -> nature == Rational ) {
					payoff_rational += ref -> payoff;
				}
				count++;
				ref = ref -> next;
			}
			assert(count == g.memory_length);
			if(payoff_conforming && n[index].storage -> payoff_conforming) assert(fabs(payoff_conforming - n[index].storage -> payoff_conforming) < 0.01);
			if(payoff_rational && n[index].storage -> payoff_rational) assert(fabs(payoff_rational - n[index].storage -> payoff_rational) < 0.01);
		}
	#endif	

	// Remove old data from storage (次の性格を決める処理が終わった後にやる)
	if(n[index].head -> nature == Conforming) {
		str -> payoff_conforming -= n[index].head -> payoff;
		str -> num_conforming -= 1.0;
	}
	if(n[index].head -> nature == Rational) {
		str -> payoff_rational -= n[index].head -> payoff;
		str -> num_rational -= 1.0;
	}

	n[index].head = removeHeadFromLink(n[index].head);

}

int main(int argc, char *argv[]) {
	set_global(argc, argv);

	// The network size is too large for test?
	#if DEBUG_FLAG
	if(g.n > debug_array_size) {
		fprintf(stderr, "Nwk size is too large for testing. Less than %d \n", debug_array_size);
		exit(1);
	}
	#endif 
	// test if payoff is subtracted correctly from those who initially vaccinated
	#if DEBUG_FLAG
		double tsp = 0;
		int num_immune = 0;
		for (unsigned int me = 0; me < g.n; me++) {
			tsp += n[me].payoff;
			if(n[me].immune) {
				num_immune ++;
			}
		}
		printf("Initial coverage input %lf, actual initial coverage %lf\n", g.coverage, (double)num_immune/g.n);
		assert(fabs(num_immune*g.vac_cost + tsp )< 0.001);
		assert( g.coverage <= 1);
		assert( 0 <= g.coverage );
	#endif

	for (int run = 0; run < SEASONS; run++) {

		// reset num of inf arrays 
		#if DEBUG_FLAG
		for (unsigned int i = 0; i < g.n; i++) {
			n_infs[i] = 0;
		}
		#endif

		g.ss1 = 0.0;
		if( 1 < NAVG ) {
			for (int k = 0; k < NAVG; k++) {
				sir();
				g.ss1 += (double)g.s;
			}

			for (unsigned int ind = 0; ind < g.n; ind++) {
				if (n[ind].immune == 0) {
					n[ind].payoff = n[ind].payoff / (double)NAVG;
				}
			}

			g.ss1 /= NAVG;
		} else {
			sir();
			g.ss1 += (double)g.s;
		}

	// test if outbreak size is less than (1 - coverage)
	#if DEBUG_FLAG
		if (run != 0) assert(g.ss1/g.n <= (1 - g.coverage));
	#endif 

	// test if payoff is in [-1 ,0]
	#if DEBUG_FLAG
		for(unsigned int indd = 0; indd< g.n; indd ++) {
			assert((-1 <= n[indd].payoff) && (n[indd].payoff <= 0));
		}
	#endif 

		if (run == 0) {
			for (unsigned int j = 0; j < g.n; j++) {
				if (get_one_or_zero_randomly(g.degree_rationality)) {
						n[j].nature = Rational;
					} else {
						n[j].nature = Conforming;
					}
					make_decision(j);
			}
			vaccinate();
		} 
		
		if (0 < run && run < g.memory_length) {
			for (unsigned int j = 0; j < g.n; j++) {
				n[j].tail = addToLink(n[j].tail, n[j].payoff, n[j].nature);
				
				// Add to storage 
				if(n[j].nature == Conforming) {
					n[j].storage->payoff_conforming += n[j].payoff;
					n[j].storage->num_conforming += 1;
				} 
				if(n[j].nature == Rational) {
					n[j].storage->payoff_rational += n[j].payoff;
					n[j].storage->num_rational += 1;
				}

				if (get_one_or_zero_randomly(g.degree_rationality)) {
					n[j].nature = Rational;
				} else {
					n[j].nature = Conforming;
				}	
				make_decision(j);
			}
			vaccinate();
		}

		if (run == g.memory_length) {

			for (unsigned int index = 0; index < g.n; index++) {
				n[index].head = removeHeadFromLink(n[index].head);

				develop_nature(index);
				make_decision(index);
			}
			vaccinate();

		} 
		
		if (g.memory_length < run && run < SEASONS - CUTOFF) {

			for (unsigned int index = 0; index < g.n; index++) {
				develop_nature(index);
				make_decision(index);
			}
			vaccinate();

		} 
		
		if (SEASONS - CUTOFF <= run && run < SEASONS - 1) {

			record.coverage += g.coverage;
			record.outbreak_size += g.ss1;

			for (unsigned int index = 0; index < g.n; index++) {
				if (n[index].nature == Conforming) {
					record.proportion_conformists += 1;
				}
				develop_nature(index);
				make_decision(index);
			}
			vaccinate();

		} 
		
		if ( run == SEASONS - 1 ) {

			record.coverage += g.coverage;
			record.outbreak_size += g.ss1;

			for (unsigned int index = 0; index < g.n; index++) {
				if (n[index].nature == Conforming) {
					record.proportion_conformists += 1;
				}
			}
		}
	}

	record.proportion_conformists /= CUTOFF*g.n;
	record.coverage /= CUTOFF;
	record.outbreak_size /= CUTOFF*g.n;

	printf("%f %f %f \n", record.proportion_conformists, record.coverage, record.outbreak_size);

	for (unsigned int re = 0; re < g.n; re++) free(n[re].nb);
	free(n);
	free(g.heap);

	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
