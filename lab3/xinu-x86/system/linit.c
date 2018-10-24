/*	linit.c	- linit	initialize lock system */
#include <xinu.h>

/* Lab 3: Complete this function */

// declare any global variables here

void linit(void) {
    intmask mask = disable();
	// your implementation goes here
    // make sure to call this in initialize.c
    int32 i;
    struct lockent *lockptr;
    for (i = 0; i < NLOCKS; i++) {
		lockptr = &locktab[i];
		lockptr->lstate = FREE;
        lockptr->readcount =0;
	}
    restore(mask);
}
