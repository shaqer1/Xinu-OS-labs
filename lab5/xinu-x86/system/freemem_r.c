/* freemem_r.c - freemem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  freemem_r  -  Free a memory block and do book keeping, returning the block to the free list
 *------------------------------------------------------------------------
 */

void removeAlloc(struct memblk *, pid32);
syscall freemem_rPid(char *, uint32, pid32);

syscall	freemem_r(
	  char		*blkaddr,	/* Pointer to memory block	*/
	  uint32	nbytes		/* Size of block in bytes	*/
	)
{
	return freemem_rPid(blkaddr, nbytes, currpid);
}

syscall	freemem_rPid(
	  char		*blkaddr,	/* Pointer to memory block	*/
	  uint32	nbytes,		/* Size of block in bytes	*/
	  pid32 pid				/* pid of  free mem */
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	memblk	*next, *prev, *block;
	uint32	top;



	mask = disable();
	if ((nbytes == 0) || ((uint32) blkaddr < (uint32) minheap)
			  || ((uint32) blkaddr > (uint32) maxheap)) {
		restore(mask);
		return SYSERR;
	}
	struct procent *prptr = &proctab[pid];

	//kprintf("here: %s, %x, freeing %d\n",prptr->prname, prptr->memAlloc, nbytes);
	if(prptr->memAlloc >= 0){
		prptr->memAlloc -= nbytes;
	}

	nbytes = (uint32) roundmb(nbytes);	/* Use memblk multiples	*/
	block = (struct memblk *)blkaddr;

	prev = &memlist;			/* Walk along free list	*/
	next = memlist.mnext;
	while ((next != NULL) && (next < block)) {
		prev = next;
		next = next->mnext;
	}

	

	if (prev == &memlist) {		/* Compute top of previous block*/
		top = (uint32) NULL;
	} else {
		top = (uint32) prev + prev->mlength;
	}

	/* Ensure new block does not overlap previous or next blocks	*/

	if (((prev != &memlist) && (uint32) block < top)
	    || ((next != NULL)	&& (uint32) block+nbytes>(uint32)next)) {
		restore(mask);
		return SYSERR; 
	}

	removeAlloc(block, pid);

	memlist.mlength += nbytes;
	//kprintf("freeing addr: %x with next %x prev: %x   ", block, prev, next);

	/* Either coalesce with previous block or add to free list */

	if (top == (uint32) block) { 	/* Coalesce with previous block	*/
		prev->mlength += nbytes;
		block = prev;
	} else {			/* Link into list as new node	*/
		block->mnext = next;
		block->mlength = nbytes;
		prev->mnext = block;
	}

	/* Coalesce with next block if adjacent */

	if (((uint32) block + block->mlength) == (uint32) next) {
		block->mlength += next->mlength;
		block->mnext = next->mnext;
	}
	//kprintf("    exiting free: %s, %x, freed %x\n",prptr->prname, prptr->memAlloc, nbytes);
	restore(mask);
	return OK;
}


void removeAlloc(struct memblk * rem, pid32 pid){
	struct memblk *allocList = (&proctab[pid])->allocList;
    struct memblk *next = allocList->mnext, *prev = allocList;

    while(next != NULL && next != rem){
        prev = next;
        next = next->mnext;
    }
	
	if(next == rem){
		//kprintf("removed node %x from list and prev:%x new next %x \n",next, prev, next->mnext);
		prev->mnext = next->mnext;
	}
}
