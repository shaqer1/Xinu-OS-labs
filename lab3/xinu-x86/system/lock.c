/*	lock.c - lock */
#include <xinu.h>

/* Lab 3: Complete this function */

void inherit();

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
				lockptr->idMask[currpid] = 1;
				prptr->lockMask[ldes] = 1;
				break;
			case WRITE :
					lockptr->lstate = WRITE;
					lockptr->readcount = 0;
					lockptr->idMask[currpid] = 1;
					prptr->lockMask[ldes] = 1;
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
					lockptr->maxprio = proctab[currpid].prprio > lockptr->maxprio?
						proctab[currpid].prprio:lockptr->maxprio;
					lockptr->rWaitCount++;
					prptr->lockid = ldes;
					inherit();
					//kprintf("setting lock id to %d\n", prptr->lockid);
					//kprintf("in writelock stopping read on it %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);

					prptr->prstate = PR_LOCK;	/* Set state to waiting	*/
				}else{
					lockptr->lstate = READ;
					lockptr->readcount++;
					lockptr->idMask[currpid] = 1;
					prptr->lockMask[ldes] = 1;			
				}
			}else{
				/* else wait till 0 */
				insert(currpid, lockptr->writeQueue, lpriority);
				lockptr->maxprio = proctab[currpid].prprio > lockptr->maxprio?
						proctab[currpid].prprio:lockptr->maxprio;
				lockptr->wWaitCount++;
				prptr->lockid = ldes;
				inherit();
				//kprintf("setting lock id to %d\n", prptr->lockid);
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
					lockptr->maxprio = proctab[currpid].prprio > lockptr->maxprio?
						proctab[currpid].prprio:lockptr->maxprio;
					lockptr->rWaitCount++;
					prptr->lockid = ldes;
					inherit();
					//kprintf("setting lock id to %d\n", prptr->lockid);
					//kprintf("in writelock stopping read on it %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);

					prptr->prstate = PR_LOCK;	/* Set state to waiting	*/
					break;
				case WRITE :
					insert(currpid, lockptr->writeQueue, lpriority);
					//kprintf("I had to wait:(\n");
					lockptr->maxprio = proctab[currpid].prprio > lockptr->maxprio?
						proctab[currpid].prprio:lockptr->maxprio;
					prptr->lockid = ldes;
					inherit();
					//kprintf("setting lock id to %d\n", prptr->lockid);
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


void inherit(){
	byte changed = 1;
	struct procent *prptr;
	while(changed != 0){
		changed = 0;
		for(int i =0; i <NPROC; i++){
			prptr = &proctab[i];
			if(prptr->lockid != -1){
				//kprintf("\nhere %d \n", prptr->lockid);
				struct lockent *lock = &locktab[prptr->lockid];

				for(int j = 0; j < NPROC; j++){
					if(lock->idMask[j] == 1 && proctab[i].prprio >
							 proctab[j].prprio){
						//kprintf("\nhere2\n");
						proctab[j].prinh =  proctab[j].prprio;
						proctab[j].prprio = proctab[i].prprio;
						changed = 1;
					}
				}
			}

		}
	}
}