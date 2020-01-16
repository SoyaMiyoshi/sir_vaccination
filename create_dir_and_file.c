// #include "sir.h"
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <unistd.h>

// // macros for conversion from int to string
// #define char_len 5

// extern GLOBALS g;
// extern NODE *n;

// extern FILE *logfile;

// void create_dir_and_file(char *log_dirname, char *log_filename, char *argv[]) {
// 	char *b = "b";
// 	char *c = "c";
// 	char *v = "v";
//     // char *r = "r";
//     // char *m = "m";

// 	char bs[char_len] = {'\0'};
// 	char cs[char_len] = {'\0'};
// 	char vs[char_len] = {'\0'};
//     // char rs[char_len] = {'\0'};
//     // char ms[char_len] = {'\0'};

// 	struct stat st = {0};

// 	snprintf(bs, char_len, "%2.2f\n", g.beta);
// 	snprintf(cs, char_len, "%2.2f\n", g.coverage);
// 	snprintf(vs, char_len, "%2.2f\n", g.vac_cost);

// 	if (strcmp(argv[7], b) == 0) {
// 		// fprintf(stderr,"vary beta\n");
// 		sprintf(log_dirname, "log/c=%s:v=%s", cs, vs);
// 		sprintf(log_filename, "log/c=%s:v=%s/b=%s.txt", cs,
// 			vs, bs);
// 	} else if (strcmp(argv[7], c) == 0) {
// 		// fprintf(stderr,"vary initial coverage\n");
// 		sprintf(log_dirname, "log/b=%s:v=%s", bs, vs);
// 		sprintf(log_filename, "log/b=%s:v=%s/c=%s.txt", bs,
// 			vs, cs);
// 	} else if (strcmp(argv[7], v) == 0) {
// 		// fprintf(stderr, "vary vaccination cost\n");
// 		sprintf(log_dirname, "log/b=%s:c=%s", bs, cs);
// 		sprintf(log_filename, "log/b=%s:c=%s/v=%s.txt", bs,
// 			cs, vs);
// 	} else {
// 		//printf("argv[7] should be 'b', 'c', or 'v'.\n");
// 	}

// 	if (stat(log_dirname, &st) == -1) {
// 		mkdir(log_dirname, 0700);
// 	}
// }
