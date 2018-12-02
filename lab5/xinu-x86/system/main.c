/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>

#define ASSERT_TRUE(test, x) { if((x)) { kprintf("%s: PASS\n", test); } else { kprintf("%s: FAIL\n", test); } }

static uint32 memory_allocated = 0;


bool8 initial_active[NPROC];

/* ************************************
 * Tests basic functionality
 *          
 *           Expected Result
 * test0: PASS
 * test1: PASS
 * test2: PASS
 * ************************************
 */
void test0() {
	uint32 blk_size = 100;

	char* buffer = NULL;
	ASSERT_TRUE("test0", 
			((r_mem_allocated(getpid(), &memory_allocated) == OK) &&
			 (memory_allocated == 0)));

	buffer = getmem_r(blk_size);
	kprintf("buff: %x\n", buffer);
	ASSERT_TRUE("test1",
			((buffer > 0 && buffer != (char*)SYSERR) &&
			 (r_mem_allocated(getpid(), &memory_allocated) == OK) &&
			 (memory_allocated == blk_size)));

	buffer = getmem_r(blk_size);
	kprintf("buff: %x\n", buffer);
	ASSERT_TRUE("test1",
			((buffer > 0 && buffer != (char*)SYSERR) &&
			 (r_mem_allocated(getpid(), &memory_allocated) == OK) &&
			 (memory_allocated == 2*blk_size)));

	buffer = getmem_r(blk_size);
	kprintf("buff: %x\n", buffer);
	ASSERT_TRUE("test1",
			((buffer > 0 && buffer != (char*)SYSERR) &&
			 (r_mem_allocated(getpid(), &memory_allocated) == OK) &&
			 (memory_allocated == 3*blk_size)));

	ASSERT_TRUE("test2",
			((freemem_r(buffer, blk_size) == OK) &&
			 (r_mem_allocated(getpid(), &memory_allocated) == OK) &&
			 (memory_allocated == 2*blk_size)));
}

/* ********************************************************************************
 * Testing infrastructure
 * ********************************************************************************
 */
void dispatch_test(void (*test)(void)) {
	test();
	send(proctab[getpid()].prparent, OK);
}

void run_test(char* test_name, void (*test)(void), int32 waittime) {
	pid32 test_pid;
	memory_allocated = 0;
	kprintf("===========================\n");
	test_pid = create(dispatch_test, 
			proctab[getpid()].prstklen, 
			getprio(getpid()) - 1, test_name, 1, test);
	if(test_pid == SYSERR) {
		ASSERT_TRUE(test_name, FALSE);
	} else {
		recvclr();
		kprintf("Running test %s\n", test_name);
		resume(test_pid);
		if(recvtime(waittime * 1000) == TIMEOUT) {
			ASSERT_TRUE(test_name, FALSE);
		}
	}
	kprintf("===========================\n\n");

	/* Clean up any processes that were created during this test */
	for(pid32 pid = 1; pid < NPROC; pid++) {
		if(initial_active[pid] == FALSE && pid != getpid()) {
			kill(pid);
		}
	}
}

process	main(void)
{
	intmask mask = disable();
	for(pid32 pid = 0; pid < NPROC; pid++) {
		if(proctab[pid].prstate == PR_FREE) {
			initial_active[pid] = FALSE;
		} else {
			initial_active[pid] = TRUE;
		}
	}
	restore(mask);

	kprintf("################## Testing Start ###################\n");
	resume(create(run_test, 2048, 30, "run_test0", 3, "test0", test0, 5));
	receive();recvclr(); 
	kprintf("################ Testing Complete ##################\n");
	return OK;
}
