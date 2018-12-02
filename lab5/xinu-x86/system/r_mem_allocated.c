/* r_mem_allocated.c - r_mem_allocated */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  r_mem_allocated  -  book keeping for aloc lists per proc
 *------------------------------------------------------------------------
 */
syscall	r_mem_allocated(
	  pid32 pid,	/* Pointer to memory block	*/
	  uint32*	buf		/* Size of block in bytes	*/
	)
{
    intmask mask = disable();
    struct	procent *prptr;		/* Ptr to process' table entry	*/

	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE
        || buf==NULL) {
		restore(mask);
		return SYSERR;
	}


	/* struct memblk *allocList = (&proctab[pid])->allocList;
    struct memblk *next = allocList->mnext, *prev = allocList;

    while(next != NULL){
        prev = next;
        next = next->mnext;
    } */
    
    *buf = prptr->memAlloc;
	//kprintf("buf: %d\n", *buf);
    restore(mask);
    return OK;

}