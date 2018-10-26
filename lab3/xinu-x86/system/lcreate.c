/*	lcreate.c - lcreate	*/
#include <xinu.h>

/* Lab 3: Complete this function */

int32 lcreate() {

  // your implementation

  intmask	mask;			/* Saved interrupt mask		*/
	int32	lock;			/*  ID to return	*/

	mask = disable();

  static	int32	nextlock = 0;	/* index to try	*/
	int32	i;			/* Iterate through # entries	*/

	for (i=0 ; i<NLOCKS ; i++) {
		lock = nextlock++;
		if (nextlock >= NLOCKS)
			nextlock = 0;
		if (locktab[lock].lstate == FREE) {
      locktab[lock].lstate = USED;
			locktab[lock].readcount = 0;
/* 			locktab[lock].time = locktab[lock].time != i?locktab[lock].time:i;
 */			locktab[lock].rWaitCount = 0;
			locktab[lock].wWaitCount = 0;
      restore(mask);
			return lock;
		}
	}


	restore(mask);
	return SYSERR;
}
