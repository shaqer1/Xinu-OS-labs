/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	

	intmask mask = disable();
	/* If rescheduling is deferred, record attempt and return */
	//kprintf("in rescehd\n");

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		//kprintf("didn't differ\n");
		restore(mask);
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	/* update prprio */
	int resched = 0;
	if(currpid != NULLPROC){
		ptold->prprio = ptold->prbaseprio + 
									( 2 * ptold->prextprio ) + 
										ptold->prrecent;


		ptold->prprio = (ptold->prprio < 0)?0:
							(ptold->prprio > 127)?127
								:ptold->prprio;

		//insert(getitem(currpid), readylist, ptold->prprio);
		if(ptold->prstate == PR_CURR && ptold->quota != UINT_MAX && ptold->time > ptold->quota){
			kill(currpid);
			restore(mask);
			return;
		}
	}

	

	if (ptold->prstate == PR_CURR && !resched) {  /* Process remains eligible */
		if (ptold->prprio < lastkey(readylist)) {
			//kprintf("remains in current process %s", ptold->prname);
			restore(mask);
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */

	//currpid = dequeue(readylist);
	currpid = dequeueLast(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	//kprintf("switching to process %s prio %d\n", ptnew->prname, ptnew->prprio);
	preempt = ptnew->prquantum/* QUANTUM */;		/* Reset time slice for process	*/
	//kprintf("quantum in new currpid %x\n",preempt);
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */
	restore(mask);

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}

pid32	dequeueLast(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid32 tail = queuetail(q);
	pid = queuetab[tail].qprev;
	int prev = queuetab[pid].qprev;
	int next = queuetab[pid].qnext;

	queuetab[prev].qnext = next;
	queuetab[next].qprev = prev;
	return pid;
}
