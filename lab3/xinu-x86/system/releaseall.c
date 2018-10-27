/*	releaseall.c - releaseall	*/
#include <xinu.h>
#include <stdarg.h>

/* Lab 3: Complete this function */

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
			restore(mask);
			return SYSERR;
		}
		lockptr= &locktab[lock];
		if (lockptr->lstate == FREE) {
			restore(mask);
			return SYSERR;
		}

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
