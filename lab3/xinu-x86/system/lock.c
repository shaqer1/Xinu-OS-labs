/*	lock.c - lock */
#include <xinu.h>

/* Lab 3: Complete this function */

syscall lock(int32 ldes, int32 type, int32 lpriority) {

	
	intmask mask;

	mask = disable();
	
	struct lockent *lockptr;

	if(isbadlock(ldes) || (type != READ && type != WRITE)){
		restore(mask);
		//kprintf("bad lock\n"); 
		return SYSERR;
	}

	lockptr = &locktab[ldes];

	/* if (lockptr->lstate==FREE && lockptr->time/NLOCKS>=1) {
		restore(mask);
		//kprintf("deleted lock\n");
		return DELETED;
	} */

	if(lockptr->lstate==FREE){
		restore(mask);
		//kprintf("deleted lock\n");
		return SYSERR;
	}

	
	struct	procent *prptr;
	prptr = &proctab[currpid];

	
	if(lockptr->lstate == USED){
		lockptr->lstate = type;
		switch(type){
			case READ :
				lockptr->lstate = READ;
				lockptr->readcount++;
				break;
			case WRITE :
					lockptr->lstate = WRITE;
					lockptr->readcount = 0;
				break;

		}
		restore(mask);
		return OK;
	}
	switch(lockptr->lstate){
		case READ :
			if(type == READ){
				if(lockptr->rWaitCount > 0 && lpriority > firstkey(lockptr->writeQueue)){
					insert(currpid, lockptr->readQueue, lpriority);
					lockptr->rWaitCount++;
					//kprintf("in writelock stopping read on it %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);

					prptr->prstate = PR_LOCK;	/* Set state to waiting	*/
				}else{
					lockptr->lstate = READ;
					lockptr->readcount++;
				}
			}else{
				/* else wait till 0 */
				insert(currpid, lockptr->writeQueue, lpriority);
				lockptr->wWaitCount++;
				//kprintf("in Read lock  %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);
			
				prptr->prstate = PR_LOCK;	/* Set state to waiting	*/
			}
			break;
		case WRITE :
			/* wait  till writer is done */
			//kprintf("Lock is writing\n");
			switch(type){
				case READ :
					insert(currpid, lockptr->readQueue, lpriority);
					lockptr->rWaitCount++;
					//kprintf("in writelock stopping read on it %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);

					prptr->prstate = PR_LOCK;	/* Set state to waiting	*/
					break;
				case WRITE :
					insert(currpid, lockptr->writeQueue, lpriority);
					//kprintf("I had to wait:(\n");
					lockptr->wWaitCount++;
					//kprintf("in Writelock %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);
					prptr->prstate = PR_LOCK;	/* Set state to waiting	*/
					break;
			}
			break;

	}

        //your implementation goes here
	//most of the logic can be implemented here
	//consider different cases as mentioned in the handout

	resched();
	restore(mask);
	if(lockptr->lstate==FREE){
		//restore(mask);
		//kprintf("deleted lock\n");
		return DELETED;
	}
	return OK;
}
