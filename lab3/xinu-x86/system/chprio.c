/* chprio.c - chprio */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  chprio  -  Change the scheduling priority of a process
 *------------------------------------------------------------------------
 */

pri16	chprio(
	  pid32		pid,		/* ID of process to change	*/
	  pri16		newprio		/* New priority			*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	pri16	oldprio;		/* Priority to return		*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16) SYSERR;
	}
	prptr = &proctab[pid];
	oldprio = prptr->prprio;
	byte cont = 1; 
	for(int i = 0; cont && i < NLOCKS; i++){
		if(prptr->lockMask[i] == 1){
			assignPrio(pid);
			cont = 0;
		}
	}
	
	if(cont ==0){
		prptr->prprio = (newprio>prptr->prprio)?newprio:prptr->prprio;
	}else{
		prptr->prprio = newprio;
	}
	

	restore(mask);
	return oldprio;
}


/* void assignPrio(pid32 pid){
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
} */