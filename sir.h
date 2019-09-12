// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// code for SIR on networks by Petter Holme (2018)
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef TIME
#include <time.h>
#endif
#include <stdbool.h>

#define NAVG 100  // number of runs for averages

#define SEASONS 1000

#define I_OR_R (UINT_MAX - 1)
#define NONE UINT_MAX

#define S(x) (n[(x)].time > now)

// auxiliary macro
#define SQ(x) ((x) * (x))

typedef struct GLOBALS {
	// INPUT PARAMETERS
	float beta;  // infection rate argv[2]
	// NETWORK SPECS
	unsigned int n;
	// OTHER GLOBALS
	unsigned int nheap, *heap;
	// OUTBREAK STATS
	unsigned int s;
	// Vaccination coverage
	float coverage;  // argv[3]
	float vac_cost;  // 0 < this < 1 argv[4]

	float conformist_proportion;  // argv[5]
	float zealot_proportion;      // argv[6]

	unsigned int numZl;
	unsigned int numCf;
	unsigned int numImt;

	double st1, ss1;
    double st2, ss2; 

	float pfAllAvg;
	float pfZlAvg;
	float pfConfAvg;
	float pfImtAvg;

	float pfAllSqdAvg;
	float pfZlSqdAvg;
	float pfConfSqdAvg;
	float pfImtSqdAvg;

	float convergenceWatcher[SEASONS];

	float t;
	// FOR RNG
	uint64_t state;
	uint32_t rmem;
	float rexp[0x10000];
} GLOBALS;

typedef struct NODE {
	unsigned int deg, *nb;  // degree and network neighbors
	unsigned int heap;
	unsigned int ninf;  // number of infections
	unsigned int immune;
	unsigned int decision;
	float payoff;
	float payoff_each;
	float time;

	unsigned int is_zealot;
	unsigned int is_conformist;

} NODE;

// heap.c
extern void up_heap(unsigned int);
extern void del_root();

// misc.c
extern void init_rng();
extern void read_data(FILE *);

// pcg_rnd.c
extern uint16_t pcg_16();
extern uint32_t pcg_32();
extern uint32_t pcg_32_bounded();
extern void pcg_init();
extern int get_random_int(float);
extern float get_random_float();

// set_global.c
extern void set_global();

// create_dir_and_file.c
extern void create_dir_and_file();

// make_strategy.c
extern void make_strategy();

// observe_status.c
extern void calculate_outbreaksize_and_timetoext();
extern void calculate_payoff_each_group();
extern void reset_result_each_season();
extern void calculate_payff_each_agent();
extern void finalize_result_each_season();
extern void print_result(float);
extern bool check_convergence(int, int, float);

// set-characteristics.c
extern void set_characteristics();
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
