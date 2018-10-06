/* initialize.c - nulluser, sysinit, sizmem */

/* Handle system initialization and become the null process */

#include <xinu.h>
#include <string.h>

extern	void	start(void);	/* Start of Xinu code			*/
extern	void	*_end;		/* End of Xinu code			*/

/* Lab2 */
extern	void daemon(void);
int32 loadBuffer[BUFFERSIZE];
int32 head;
int32 size;	
int32 aveload;

/* Function prototypes */

extern	void main(void);	/* Main is the first process created	*/
extern	void xdone(void);	/* System "shutdown" procedure		*/
static	void sysinit(); 	/* Internal system initialization	*/
extern	void meminit(void);	/* Initializes the free memory list	*/

/* Declarations of major kernel variables */

struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	memblk	memlist;	/* List of free memory blocks		*/

/* Active system status */

int	prcount;		/* Total number of live processes	*/
pid32	currpid;		/* ID of currently executing process	*/
pid32 DAEMON_PID;

/*------------------------------------------------------------------------
 * nulluser - initialize the system and become the null process
 *
 * Note: execution begins here after the C run-time environment has been
 * established.  Interrupts are initially DISABLED, and must eventually
 * be enabled explicitly.  The code turns itself into the null process
 * after initialization.  Because it must always remain ready to execute,
 * the null process cannot execute code that might cause it to be
 * suspended, wait for a semaphore, put to sleep, or exit.  In
 * particular, the code must not perform I/O except for polled versions
 * such as kprintf.
 *------------------------------------------------------------------------
 */

void	nulluser()
{	
	struct	memblk	*memptr;	/* Ptr to memory block		*/
	uint32	free_mem;		/* Total amount of free memory	*/
	
	/* Initialize the system */
		
	sysinit();

	kprintf("\n\r%s\n\n\r", VERSION);
	
	/* Output Xinu memory layout */
	free_mem = 0;
	for (memptr = memlist.mnext; memptr != NULL;
						memptr = memptr->mnext) {
		free_mem += memptr->mlength;
	}
	kprintf("%10d bytes of free memory.  Free list:\n", free_mem);
	for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
	    kprintf("           [0x%08X to 0x%08X]\r\n",
		(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
	}

	kprintf("%10d bytes of Xinu code.\n",
		(uint32)&etext - (uint32)&text);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)&text, (uint32)&etext - 1);
	kprintf("%10d bytes of data.\n",
		(uint32)&ebss - (uint32)&data);
	kprintf("           [0x%08X to 0x%08X]\n\n",
		(uint32)&data, (uint32)&ebss - 1);

	/* Enable interrupts */

	enable();

	/* Create a process to execute function daemon() */
	head = 0, size=-1;
	//	kprintf("start daemon\n");
	DAEMON_PID = create((void *)daemon, INITSTK, MAXEXTPRIO,
	 "SDaemon", 0,
           NULL);
	system(DAEMON_PID);
	//	kprintf("run daemon\n");
	resume(DAEMON_PID);

	/* Create a process to execute function main() */
	pid32 mainpid = create((void *)main, INITSTK, 0, "Main process", 0,
           NULL);
	
	resume (mainpid
	   );

	/* Become the Null process (i.e., guarantee that the CPU has	*/
	/*  something to run when no other process is ready to execute)	*/

	while (TRUE) {
		;		/* Do nothing */
	}

}



void daemon(){
	while(1){
		int mask = disable();
		int32 sum =0, count =0;
		int curr =0;

		//curr = firstid(readylist);
		//kprintf("head: %x\n", head);
		while(curr < NPROC){
			if(proctab[curr].prstate == PR_READY && curr != NULLPROC){
				 //kprintf("daemon while looifp %s \n",proctab[curr].prname);
				count++;
			}
			//kprintf("daemon while loop %s state %x\n",proctab[curr].prname, proctab[curr].prstate);
			curr++;
		}
		//kprintf("in daemon count is %x\n", count);

		for(int32 i =0; i< (size<BUFFERSIZE?size:BUFFERSIZE); i++){
			//kprintf("loadbuffer [i] %x\n", loadBuffer[i]);
			sum+=loadBuffer[i];
		}

		//kprintf("in daemon sum is %x\n", sum);

		
		aveload = sum/((size>0)?size:1);
		loadBuffer[head] = count;
		//kprintf("aveload is %x size is %x sum is %x head is %x inst load is %x \n", aveload, size, sum, head, loadBuffer[head]);
		head = (head+1%BUFFERSIZE);
		size = (size+1 > 60)?60:size+1;

		int i=0;
		while(i<=prcount){
			//kprintf("in while %s \n", proctab[i].prname);
			if(proctab[i].prstate != PR_FREE && i != currpid && i != NULLPROC){
				//kprintf("in while's if\n");
				//kprintf("before process %s prrecnt %x extPri %x base %x prio %x quant: %x \n", proctab[i].prname, proctab[i].prrecent,proctab[i].prextprio, proctab[i].prbaseprio, proctab[i].prprio, proctab[i].prquantum);
				proctab[i].prrecent = (aveload * proctab[i].prrecent) / 
								(( 2 * aveload ) + 1) + proctab[i].prextprio;
				//kprintf("check prrecent %x \n",proctab[i].prrecent);
				if (proctab[i].prrecent < 0){
					proctab[i].prrecent = 0;
					//kprintf("prrecent < 0\n");
				}

				proctab[i].prprio = proctab[i].prbaseprio + 
								( 2 * proctab[i].prextprio ) + 
									proctab[i].prrecent;


				proctab[i].prprio = (proctab[i].prprio < 0)?0:
									(proctab[i].prprio > 127)?127
										:proctab[i].prprio;

				queuetab[i].qkey = proctab[i].prprio;

				proctab[i].prquantum = QUANTUM + proctab[i].prrecent;
				//kprintf("process %s prrecnt %x prio %x quant: %x \n", proctab[i].prname, proctab[i].prrecent, proctab[i].prprio, proctab[i].prquantum);
				insert(getitem(i), readylist, proctab[i].prprio);
			}

			i++;
		}
		//kprintf("out of while\n");

		/* struct	procent	*prptr;	

		for (int32 i = 0; i < NPROC; i++) {
			prptr = &proctab[i];
			if(prptr->prstate == PR_READY){
				
			}
		} */

		restore(mask);
		sleep(1);
	}

}

/*------------------------------------------------------------------------
 *
 * sysinit  -  Initialize all Xinu data structures and devices
 *
 *------------------------------------------------------------------------
 */
static	void	sysinit()
{
	int32	i;
	struct	procent	*prptr;		/* Ptr to process table entry	*/
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/

	/* Platform Specific Initialization */

	platinit();

	/* Initialize the interrupt vectors */

	initevec();
	
	/* Initialize free memory list */
	
	meminit();

	/* Initialize system variables */

	/* Count the Null process as the first process in the system */

	prcount = 1;

	/* Scheduling is not currently blocked */

	Defer.ndefers = 0;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	/* Initialize the Null process entry */	

	prptr = &proctab[NULLPROC];
	prptr->prstate = PR_CURR;
	prptr->prprio = 128;
	prptr->prextprio = MINEXTPRIO;
	strncpy(prptr->prname, "prnull", 7);
	prptr->prstkbase = getstk(NULLSTK);
	prptr->prstklen = NULLSTK;
	prptr->prstkptr = 0;
	currpid = NULLPROC;
	
	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	/* Initialize buffer pools */

	bufinit();

	/* Create a ready list for processes */

	readylist = newqueue();

	/* Initialize the real time clock */

	clkinit();

	for (i = 0; i < NDEVS; i++) {
		init(i);
	}
	return;
}

int32	stop(char *s)
{
	kprintf("%s\n", s);
	kprintf("looping... press reset\n");
	while(1)
		/* Empty */;
}

int32	delay(int n)
{
	DELAY(n);
	return OK;
}
