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

#define NAVG 100 // number of runs for averages

#define SEASONS 500
#define CUTOFF 200

#define I_OR_R (UINT_MAX - 1)
#define NONE UINT_MAX

#define S(x) (n[(x)].time > now)

// auxiliary macro
#define SQ(x) ((x) * (x))

typedef struct RECORD {
	float proportion_conformists;
	float coverage;
	float outbreak_size;
} RECORD;

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
	float degree_rationality; // argv[6]
	int memory_length;

	double ss1;

	float t;
	// FOR RNG
	uint64_t state;
	uint32_t rmem;
	float rexp[0x10000];
} GLOBALS;

enum Nature {
    Conforming = -1,
    Rational = 1
};

struct oneMemory {
    enum Nature nature;
    float payoff;
    struct oneMemory *next;
};

struct Storage {
	float payoff_conforming;
	float payoff_rational;
	int num_conforming;
	int num_rational;
} Storage;

typedef struct NODE {
	unsigned int deg, *nb;  // degree and network neighbors
	unsigned int heap;
	unsigned int ninf;  // number of infections
	unsigned int immune;
	unsigned int decision;
	float payoff;
	float time;
	enum Nature nature;
	struct oneMemory * head;
	struct oneMemory * tail;
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
extern int get_one_or_zero_randomly(float);
// extern float get_random_float();

// set_global.c
extern void set_global();

// extern void calculate_payoff_each_group();
// extern void print_result(float);
// extern bool check_convergence(int, int, float);
// extern void create_dir_and_file(char *, char *, char *[]);

// linked-list.c
extern struct oneMemory * addToLink(struct oneMemory *, float, enum Nature);
extern struct oneMemory * removeHeadFromLink(struct oneMemory *);
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -