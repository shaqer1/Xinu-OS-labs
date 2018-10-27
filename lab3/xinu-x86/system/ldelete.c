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
	//kprintf("deleting and readying\n");
	while(lockptr->rWaitCount > 0){
		ready(dequeue(lockptr->readQueue));
		lockptr->rWaitCount--;
	}
	while(lockptr->wWaitCount > 0){
		ready(dequeue(lockptr->writeQueue));
		lockptr->wWaitCount--;
	}
	lockptr->lstate = FREE;
	//memset(lockptr->idMask, 0, sizeof(lockptr->idMask));
	//lockptr->time+=NLOCKS;
	//kprintf("deleted and readied\n");
	resched_cntl(DEFER_STOP);

	restore(mask);
	return OK;
}
