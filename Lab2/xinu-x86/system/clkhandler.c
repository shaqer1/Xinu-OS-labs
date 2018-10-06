/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/
	static uint32 count10s = 10;
	/* Decrement the ms counter, and see if a second has passed */

	if((--count10s) <= 0) {

		/* 10MS second has passed, so increment 10MS count */

		//kprintf("\nincrementing prrecent for %s \n",proctab[currpid].prname);

		proctab[currpid].prrecent++;

		/* Reset the local ms counter for the next second */

		count10s = 10;
	}

	if((--count1000) <= 0) {

		/* One second has passed, so increment seconds count */

		clktime++;

		/* Reset the local ms counter for the next second */

		count1000 = 1000;
	}

	proctab[currpid].time++;

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */

	if((--preempt) <= 0) {
		preempt = proctab[currpid].prquantum/* QUANTUM */;
		//kprintf("\nquantum reached for process %s with quantum %x\n", proctab[currpid].prname, proctab[currpid].prquantum);
		resched();
	}
}
