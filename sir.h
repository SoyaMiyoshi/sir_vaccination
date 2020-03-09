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

#define NAVG 2000 // number of runs for averages

#define SEASONS 500
#define CUTOFF 200

#define I_OR_R (UINT_MAX - 1)
#define NONE UINT_MAX

#define S(x) (n[(x)].time > now)

// auxiliary macro
#define SQ(x) ((x) * (x))

typedef struct RECORD {
	double proportion_conformists;
	double coverage;
	double outbreak_size;
} RECORD;

typedef struct GLOBALS {
	// INPUT PARAMETERS
	double beta;  // infection rate argv[2]
	// NETWORK SPECS
	unsigned int n;
	// OTHER GLOBALS
	unsigned int nheap, *heap;
	// OUTBREAK STATS
	unsigned int s;
	// Vaccination coverage
	double coverage;  // argv[3]
	double vac_cost;  // 0 < this < 1 argv[4]
	double degree_rationality; // argv[6]
	int memory_length;

	double ss1;

	double t;
	// FOR RNG
	uint64_t state;
	uint32_t rmem;
	double rexp[0x10000];
} GLOBALS;

enum Nature {
    Conforming = -1,
    Rational = 1
};

typedef struct oneMemory {
    enum Nature nature;
    double payoff;
    struct oneMemory *next;
} oneMemory;

typedef struct Storage {
	double payoff_conforming;
	double payoff_rational;
	int num_conforming;
	int num_rational;
} Storage;

typedef struct NODE {
	unsigned int deg, *nb;  // degree and network neighbors
	unsigned int heap;
	unsigned int immune;
	unsigned int decision;
	double payoff;
	double time;
	enum Nature nature;
	struct oneMemory *head;
	struct oneMemory *tail;
	struct Storage *storage;
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
extern int get_one_or_zero_randomly(double);
// extern double get_random_double();

// set_global.c
extern void set_global();

// extern void calculate_payoff_each_group();
// extern void print_result(double);
// extern bool check_convergence(int, int, double);
extern void create_dir_and_file(char *, char *, char *[]);

// linked-list.c
extern struct oneMemory * addToLink(struct oneMemory *, double, enum Nature);
extern struct oneMemory * removeHeadFromLink(struct oneMemory *);
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
