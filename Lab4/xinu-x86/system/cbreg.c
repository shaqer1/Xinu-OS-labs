/* cbreg.c - cbreg */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  cbreg  -  reg cbreg
 *------------------------------------------------------------------------
 */
syscall	cbreg( int (* fnp)(void) ){
    intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/

	mask = disable();
    prptr = &proctab[currpid];
	if (prptr->prstate == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

    prptr->fptr = fnp;

    prptr->prhascb = TRUE;

    restore(mask);		/* Restore interrupts */

    return OK;
}
