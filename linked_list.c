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

struct oneMemory * addToLink(struct oneMemory *tail, float payoff, enum Nature nature){
    tail->payoff = payoff;
	tail->nature = nature;
    tail->next = malloc(sizeof(struct oneMemory));
    // return new tail
    return(tail->next);
}

struct oneMemory * removeHeadFromLink(struct oneMemory * head){
    struct oneMemory * tmp = head -> next;
    free(head);
    // return new head 
    return(tmp);
}

void add_to_memory() {
	for (unsigned int ind = 0 ; ind < g.n; ind++) {
		n[ind].tail = addToLink(n[ind].tail, n[ind].payoff, n[ind].nature);
	}
}

void add_to_memory_and_remove_old() {
	for (unsigned int ind = 0 ; ind < g.n; ind++) {
		n[ind].tail = addToLink(n[ind].tail, n[ind].payoff, n[ind].nature);
		n[ind].head = removeHeadFromLink(n[ind].head);
	}
}
