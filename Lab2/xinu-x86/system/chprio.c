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
	prptr->prextprio = newprio;
	/* update prprio */
	if(currpid != NULLPROC){
		prptr->prprio = prptr->prbaseprio + 
									( 2 * prptr->prextprio ) + 
										prptr->prrecent;


		prptr->prprio = (prptr->prprio < 0)?0:
							(prptr->prprio > 127)?127
								:prptr->prprio;

		//insert(getitem(currpid), readylist, prptr->prprio);

	}

	if(proctab[pid].prstate == PR_READY && pid != NULLPROC){
		insert(getitem(pid), readylist, prptr->prprio);
	}

	//prptr->prprio = newprio;
	restore(mask);
	resched();
	return oldprio;
}

syscall system(pid32 pid){
	intmask	mask;
	struct	procent	*prptr;	

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return (pri16) SYSERR;
	}

	prptr = &proctab[pid];
	prptr->prbaseprio = 0;

	prptr->prprio = prptr->prbaseprio + 
						( 2 * prptr->prextprio ) + 
							prptr->prrecent;
	prptr->prprio = (prptr->prprio < 0)?0:
							(prptr->prprio > 127)?127
								:prptr->prprio;
	
	//kprintf("\nconverting to system after remove below %x %x %s \n", pid, prptr->prprio, proctab[pid].prname);
	if(proctab[pid].prstate == PR_READY && pid != NULLPROC){
		insert(getitem(pid), readylist, prptr->prprio);
	}
	
	restore(mask);

	return OK;
}

syscall setquota ( pid32 pid, uint32 milliseconds ){
	intmask	mask;
	struct	procent	*prptr;	

	mask = disable();
	if (isbadpid(pid) || pid == NULLPROC || pid == DAEMON_PID) {
		restore(mask);
		return (pri16) SYSERR;
	}

	prptr = &proctab[pid];

	prptr->quota = (int32) milliseconds;


	restore(mask);

	return OK;
}