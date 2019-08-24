#include "sir.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// macros for conversion from int to string
#define char_len 5

extern GLOBALS g;
extern NODE *n;

extern FILE *logfile;

void create_dir_and_file(char *log_dirname, char *log_filename, char *argv[]) {
	char *b = "b";
	char *c = "c";
	char *v = "v";
	char *cf = "cf";
	char *zf = "zf";
	char bs[char_len] = {'\0'};
	char cs[char_len] = {'\0'};
	char vs[char_len] = {'\0'};
	char cfs[char_len] = {'\0'};
	char zfs[char_len] = {'\0'};

	struct stat st = {0};

	snprintf(bs, char_len, "%2.2f\n", g.beta);
	snprintf(cs, char_len, "%2.2f\n", g.coverage);
	snprintf(vs, char_len, "%2.2f\n", g.vac_cost);
	snprintf(cfs, char_len, "%2.2f\n", g.conformist_proportion);
	snprintf(zfs, char_len, "%2.2f\n", g.zealot_proportion);

	if (strcmp(argv[7], b) == 0) {
		// fprintf(stderr,"vary beta\n");
		sprintf(log_dirname, "log/c=%s:v=%s:cf=%s:zf=%s", cs, vs, cfs,
			zfs);
		sprintf(log_filename, "log/c=%s:v=%s:cf=%s:zf=%s/b=%s.txt", cs,
			vs, cfs, zfs, bs);
	} else if (strcmp(argv[7], c) == 0) {
		// fprintf(stderr,"vary initial coverage\n");
		sprintf(log_dirname, "log/b=%s:v=%s:cf=%s:zf=%s", bs, vs, cfs,
			zfs);
		sprintf(log_filename, "log/b=%s:v=%s:cf=%s:zf=%s/c=%s.txt", bs,
			vs, cfs, zfs, cs);
	} else if (strcmp(argv[7], v) == 0) {
		// fprintf(stderr, "vary vaccination cost\n");
		sprintf(log_dirname, "log/b=%s:c=%s:cf=%s:zf=%s", bs, cs, cfs,
			zfs);
		sprintf(log_filename, "log/b=%s:c=%s:cf=%s:zf=%s/v=%s.txt", bs,
			cs, cfs, zfs, vs);
	} else if (strcmp(argv[7], cf) == 0) {
		// fprintf(stderr,"vary fraction of conformist\n");
		sprintf(log_dirname, "log/b=%s:c=%s:v=%s:zf=%s", bs, cs, vs,
			zfs);
		sprintf(log_filename, "log/b=%s:c=%s:v=%s:zf=%s/cf=%s.txt", bs,
			cs, vs, zfs, cfs);
	} else if (strcmp(argv[7], zf) == 0) {
		// fprintf(stderr,"vary fraction of zealot\n");
		sprintf(log_dirname, "log/b=%s:c=%s:v=%s:cf=%s", bs, cs, vs,
			cfs);
		sprintf(log_filename, "log/b=%s:c=%s:v=%s:cf=%s/zf=%s.txt", bs,
			cs, vs, cfs, zfs);
	} else {
		printf("argv[7] should be 'b', 'c', 'v', 'cf', or 'zf'.\n");
	}

	if (stat(log_dirname, &st) == -1) {
		mkdir(log_dirname, 0700);
	}
}
