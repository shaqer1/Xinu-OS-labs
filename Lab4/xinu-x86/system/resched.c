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

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */

	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	pid32 curr = currpid;
	//kprintf("urr pid switching to %s\n", ptnew->prname);
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	ptnew = &proctab[currpid];
	if(ptnew->prhascb && ptnew->rcpblkflag && !ptnew->princb){
		//kprintf("calling callback\n");
		//kprintf("urr pid in calling call back switching to %s\n", ptnew->prname);
		ptnew->prhascb = FALSE;
		ptnew->princb = TRUE;
		enable();
		ptnew->fptr();
		ptnew->princb = FALSE;
		while(ptnew->rcpblkflag && !ptnew->princb){
			ptnew->princb = TRUE;
			enable();
			ptnew->fptr();
			ptnew->princb = FALSE;
		}
		if(ptnew->rcpblkflag){
			ptnew->fptr = NULL;
		}
		//ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	}/* else if(ptnew->princb && ptnew->rcpblkflag){
		ptnew->prhascb = TRUE ;
		ready(currpid);//really?sure!
	} */

	/* Old process returns here when resumed */

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
