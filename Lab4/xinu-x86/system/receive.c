/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();
	prptr = &proctab[currpid];

	if (prptr->prhasmsg == FALSE && prptr->rcpblkflag == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}

	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/

	if(prptr->rcpblkflag == TRUE){
		//kprintf("recv from waiting senders\n");
		qid16 q = prptr->sendqueue;
		if (isbadqid(q)) {
			return SYSERR;
		} /* else if (isempty(q)) {
			return EMPTY;
		} */
		sendWaitcount++;
		pid32 senderPid = dequeue(q);
		struct procent *sendPtr = &proctab[senderPid];
		/* kprintf("popped %s from receiver %d queue with message: %d and counter:%d\n"
		, &proctab[senderPid].prname, currpid, sendPtr->sendblkmsg, sendWaitcount); */
		prptr->prmsg = sendPtr->sendblkmsg;		/* Deliver message		*/
		prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/
		msg = prptr->prmsg;
		//sendPtr->prstate = PR_READY;
		sendPtr->sendblkflag = FALSE;
		//sendPtr->sendblkmsg = 0;
		if(isempty(prptr->sendqueue)){
			prptr->rcpblkflag = FALSE;
			prptr->prhasmsg = FALSE;
		}

		/* make sender ready */
		ready(senderPid);			


	}
	restore(mask);
	return msg;
}
