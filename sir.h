// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#ifdef TIME
#include <time.h>
#endif
#include <stdbool.h>

#define NAVG 5 // number of runs for averages

#define SEASONS 100

#define I_OR_R (UINT_MAX - 1)
#define NONE UINT_MAX

#define S(x) (n[(x)].time > now)

// auxiliary macro
#define SQ(x) ((x) * (x))

typedef struct GLOBALS {
	// INPUT PARAMETERS
	float beta; // infection rate
	// NETWORK SPECS
	unsigned int n;
	// OTHER GLOBALS
	unsigned int nheap, *heap;
	// OUTBREAK STATS
	unsigned int s;
	// Vaccination coverage
	float coverage;
	// Efficacy of vaccination.
	// It increases the time t (t*efficacy) for the infected node to get the vaccinated node infected.
	float efficacy;

	/* for eq(4) */
	float vac_cost; //0 < this < 1
	float KI; // Rationality strength

	/* for eq(5) */
	float KC; // Absolute conformity
	float fai; // Threshold fraction of neighbour

	float t;
	// FOR RNG
	uint64_t state;
	uint32_t rmem;
	float rexp[0x10000];
} GLOBALS;

typedef struct NODE {
	unsigned int deg, *nb; // degree and network neighbors
	unsigned int heap;
	unsigned int ninf; // number of infections
	unsigned int immunity;
	unsigned int decision;
	float payoff;
    float time;
} NODE;

// heap.c
extern void up_heap (unsigned int);
extern void del_root ();

// misc.c
extern void init_rng ();
extern void read_data (FILE *);

// pcg_rnd.c
extern uint16_t pcg_16 ();
extern uint32_t pcg_32 ();
extern uint32_t pcg_32_bounded ();
extern void pcg_init ();

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
