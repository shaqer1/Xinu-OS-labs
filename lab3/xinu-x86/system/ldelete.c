/*	ldelete.c - ldelete 	*/
#include <xinu.h>

/* Lab 3: Complete this function */

syscall ldelete( 
		int32 ldes	/* lock descriptor */
	)
{
	// your implementation

	intmask mask;
	mask = disable();
	
	struct lockent *lockptr;

	if (isbadlock(ldes) || (lockptr = &locktab[ldes])->lstate==FREE) {
		restore(mask);
		return SYSERR;
	}
	
	lockptr->lstate = FREE;

	return OK;
}
