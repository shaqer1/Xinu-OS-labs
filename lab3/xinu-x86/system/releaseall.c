/*	releaseall.c - releaseall	*/
#include <xinu.h>
#include <stdarg.h>

/* Lab 3: Complete this function */

void recalculateMaxPrio(struct	lockent *);
void assignPrio(pid32);

syscall releaseall (int32 numlocks, ...) {

	//your implementation goes here
	intmask mask;			/* Saved interrupt mask		*/
	struct	lockent *lockptr;		/* Ptr to sempahore table entry	*/

	mask = disable();
	va_list ap;

	

	va_start(ap, numlocks);
	for(int32 i =0; i< numlocks; i++){
		int32 lock = va_arg(ap, int32);
		if (isbadlock(lock)) {
			//kprintf("bad lock id in release all\n");
			restore(mask);
			return SYSERR;
		}
		lockptr= &locktab[lock];
		if (lockptr->lstate == FREE) {
			//kprintf("Free id in release all\n");
			restore(mask);
			return SYSERR;
		}

		proctab[currpid].lockMask[lock] = 0;
		locktab[lock].idMask[currpid] = 0;

		if(lockptr->wWaitCount == 0 && lockptr->rWaitCount ==0){
			lockptr->lstate = USED;
		}

		if(lockptr->readcount > 0 && lockptr->lstate == READ){
			lockptr->readcount--;
		}

		resched_cntl(DEFER_START);
		if(lockptr->wWaitCount == 0){
			while(lockptr->rWaitCount > 0){
				pid32 pid = dequeue(lockptr->readQueue);
				ready(pid);				//kprintf("readying all read process\n");
				if(proctab[pid].prprio == lockptr->maxprio){recalculateMaxPrio(lockptr);}
				assignPrio(pid);
				proctab[pid].lockid = -1;
				lockptr->idMask[pid] = 1;
				proctab[pid].lockMask[lock] = 1;
				lockptr->rWaitCount--;
			}
			lockptr->rWaitCount = 0;
			lockptr->lstate = USED;

			//kprintf("in release all %x %x \n", lockptr->wWaitCount, lockptr->rWaitCount);
			//kprintf("rWaitCount %d\n", lockptr->rWaitCount);
		}else {
			if(lockptr->rWaitCount == 0 && lockptr->readcount == 0
			/*  && lockptr->wWaitCount > 0 && lockptr->wWaitCount > 0 */){/* check if no readers */
				pid32 pid = dequeue(lockptr->writeQueue);
				ready(pid);				//kprintf("readying a write process\n");
				if(proctab[pid].prprio == lockptr->maxprio){recalculateMaxPrio(lockptr);}
				assignPrio(pid);
				proctab[pid].lockid = -1;	
				lockptr->idMask[pid] = 1;
				proctab[pid].lockMask[lock] = 1;			//kprintf("readying a write process\n");
				lockptr->wWaitCount--;
				lockptr->lstate = WRITE;
			} else {
				int32 maxPrioW = firstkey(lockptr->writeQueue);
				int32 maxPrioR = firstkey(lockptr->readQueue);
				//kprintf("read max: %d, write max: %d\n", maxPrioR, maxPrioW);
				if(maxPrioW >= maxPrioR && lockptr->readcount == 0){
					pid32 pid = dequeue(lockptr->writeQueue);
					ready(pid);					//kprintf("readying writer over readx process\n");
					if(proctab[pid].prprio == lockptr->maxprio){recalculateMaxPrio(lockptr);}
					assignPrio(pid);
					proctab[pid].lockid = -1;	
					lockptr->idMask[pid] = 1;
					proctab[pid].lockMask[lock] = 1;
					lockptr->wWaitCount--;
					lockptr->lstate = WRITE;
				}else {
					//kprintf("readying readers over writer process\n");
					int32 counter = maxPrioR;
					while(counter >= maxPrioW){
						pid32 pid = dequeue(lockptr->readQueue);
						ready(pid);					
						if(proctab[pid].prprio == lockptr->maxprio){recalculateMaxPrio(lockptr);}
						assignPrio(pid);
						proctab[pid].lockid = -1;	
						lockptr->idMask[pid] = 1;
						proctab[pid].lockMask[lock] = 1;					
						counter =  firstkey(lockptr->readQueue);
					}
					lockptr->lstate = READ;
				}
			}
		}
		resched_cntl(DEFER_STOP);
	}
	va_end(ap);
	restore(mask);
	return OK;
}


void recalculateMaxPrio(struct	lockent *lockptr){
	qid16 i = firstid(lockptr->readQueue);
	while(i != queuetail(lockptr->readQueue)){
		lockptr->maxprio = proctab[i].prprio > lockptr->maxprio?
						proctab[i].prprio:lockptr->maxprio;
		i = queuetab[i].qnext;
	}
	i = firstid(lockptr->writeQueue);
	while(i != queuetail(lockptr->writeQueue)){
		lockptr->maxprio = proctab[i].prprio > lockptr->maxprio?
						proctab[i].prprio:lockptr->maxprio;
		i = queuetab[i].qnext;
	}
}

void assignPrio(pid32 pid){
	int32 max = -1;
	for(int i = 0; i < NLOCKS; i++){
		if(proctab[pid].lockMask[i] ==1 && locktab[i].maxprio > max){
			max = locktab[proctab[pid].lockMask[i]].maxprio;
		}
	}
	//kprintf("Max: %d\n", max);
	if(max != -1){
		proctab[pid].prprio = max;
		proctab[pid].prinh = -1;
	}
}