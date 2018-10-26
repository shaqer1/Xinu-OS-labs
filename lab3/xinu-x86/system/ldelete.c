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
	
	resched_cntl(DEFER_START);
	while(lockptr->rWaitCount > 0){
		ready(dequeue(lockptr->readQueue));
	}
	lockptr->rWaitCount = 0;
	while(lockptr->wWaitCount > 0){
		ready(dequeue(lockptr->writeQueue));
	}
	lockptr->wWaitCount =0;
	resched_cntl(DEFER_STOP);
	lockptr->lstate = FREE;
/* 	lockptr->time+=NLOCKS;
 */
	restore(mask);
	return OK;
}
