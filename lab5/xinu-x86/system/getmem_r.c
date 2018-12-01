/* getmem_r.c - getmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address, keep book keeping 
 * of mem
 *------------------------------------------------------------------------
 */

void addAlloc(struct memblk *, uint32);

char  	*getmem_r(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*prev, *curr, *leftover;
	uint32 orgNbytes = nbytes; 

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}


	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/

	prev = &memlist;
	curr = memlist.mnext;
	while (curr != NULL) {			/* Search free list	*/

		if (curr->mlength == nbytes) {	/* Block is exact match	*/
			prev->mnext = curr->mnext;
			memlist.mlength -= nbytes;
            /* lab5 */
            addAlloc(curr, orgNbytes);
			restore(mask);
			return (char *)(curr);

		} else if (curr->mlength > nbytes) { /* Split big block	*/
			leftover = (struct memblk *)((uint32) curr +
					nbytes);
			prev->mnext = leftover;
			leftover->mnext = curr->mnext;
			leftover->mlength = curr->mlength - nbytes;
			memlist.mlength -= nbytes;
            /* lab5 */
            //curr->mlength = nbytes;
            addAlloc(curr, orgNbytes);
			restore(mask);
			return (char *)(curr);
		} else {			/* Move to next block	*/
			prev = curr;
			curr = curr->mnext;
		}
	}
	restore(mask);
	return (char *)SYSERR;
}

void addAlloc(struct memblk * curr, uint32 length){
    struct memblk *allocList = (&proctab[currpid])->allocList;
    struct memblk *next = allocList->mnext, *prev = allocList;

    while(next != NULL){
        prev = next;
        next = next->mnext;
    }

    next = curr;
    curr->mlength = length;
	//kprintf("curr size: %d\n", curr->mlength);
    prev->mnext = next;

}