/*	lock.c - lock */
#include <xinu.h>

/* Lab 3: Complete this function */

syscall lock(int32 ldes, int32 type, int32 lpriority) {

	
	intmask mask;
	mask = disable();
	
	struct lockent *lockptr;

	if(isbadlock(ldes)){
		restore(mask);
		return SYSERR;
	}

	if ((lockptr = &locktab[ldes])->lstate!=FREE) {
		restore(mask);
		return DELETED;
	}
	
	if(lockptr->lstate == USED){
		lockptr->lstate = type;
		switch(type){
			case READ :
				lockptr->lstate = READ;
				lockptr->readcount++;
				break;
			case WRITE :
					lockptr->lstate = WRITE;
					lockptr->readcount = -1;
				break;

		}
	}

	switch(lockptr->lstate){
		case READ :
			if(type == READ){
				lockptr->lstate = READ;
				lockptr->readcount++;
				break;
			}
			/* else wait till 0 */
		case WRITE :
			/* wait */
			break;

	}

        //your implementation goes here
	//most of the logic can be implemented here
	//consider different cases as mentioned in the handout


	restore(mask);
	return OK;
}
