/* sendq.c - sendq */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  sendq  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	sendq(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
    intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}
    
    prptr = &proctab[pid];
	if (prptr->prstate == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

    if(prptr->prqcount < MSGQ_SIZE){
        prptr->prqmsgs[prptr->prqcount++] = msg;
        kprintf("message sent %d\n",msg);
        prptr->prhasmsg =TRUE;
        restore(mask);		/* Restore interrupts */
	    return OK;
    }

     /* set sender attr */
    struct procent *curr = &proctab[currpid];
    curr->prstate = PR_SNDBLK;
    curr->sendblkflag = TRUE;
    curr->sendblkmsg = msg;
    curr->sendblkrcp = pid;
    /* Set receiver attr */
    prptr->rcpblkflag = TRUE;
	/* kprintf("inserting %s to receiver %d queue with message: %d and counter:%d\n"
		, curr->prname, pid, msg, sendWaitcount); */
    insert(currpid, prptr->sendqueue, sendWaitcount--);
	//kprintf("this sender is waiting\n");
	resched();		/* Block until recv ready	*/    



    restore(mask);		/* Restore interrupts */
	return OK;
}