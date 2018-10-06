/* resume.c - resume */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  resume  -  Unsuspend a process, making it ready
 *------------------------------------------------------------------------
 */
pri16	resume(
	  pid32		pid		/* ID of process to unsuspend	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	pri16	prio;			/* Priority to return		*/
	mask = disable();
	if (isbadpid(pid)) {
  		//kprintf("\n in resume.c error returning bad pid %x\n", pid);
		restore(mask);
		return (pri16)SYSERR;
	}
	prptr = &proctab[pid];
	if (prptr->prstate != PR_SUSP) {
  		//kprintf("\n in resume.c error2 returning %s\n", prptr->prname);
		restore(mask);
		return (pri16)SYSERR;
	}
	prio = prptr->prprio;
	ready(pid);		/* Record priority to return	*/
	restore(mask);
	return prio;
}
