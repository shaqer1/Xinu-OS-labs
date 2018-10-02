/*  main.c  - main */

#include <xinu.h>

process printProc();
process loop(int32 think, char c);
process sleepingloop(int32 think, char c);

int32 think = 500000;

extern uint32 ctr1000;

process	main(void)
{
	pid32 prP;
	pid32 prA, prB, prC;

	prP = create(printProc, INITSTK, -10, "Print", 0);
	system(prP);
	resume(prP);

  kprintf("\nStarting A\n");
  prA = create(loop, INITSTK, 20, "Pr A", 2, think, 'A');
  resume(prA);
  sleep(2);

  kprintf("\nStarting B\n");
  prB = create(loop, INITSTK, 20, "Pr B", 2, think, 'B');
  resume(prB);
  //sleep(2);

  //kprintf("\nStarting C\n");
  //prC = create(loop, INITSTK, 25, "Pr C", 2, think, 'C');
  //resume(prC);
  //sleep(2);

	return OK;
}

process loop(int32 think, char c)
{
  int i;
  while(TRUE)
    {
      kprintf("%c",c);
      for (i = 0; i < think; i++);
    }
}

process sleepingloop(int32 think, char c)
{
  int i;
  while(TRUE)
    {
      for (i = 0; i < think; i++);
      kprintf("%c", c);
      sleepms(10);
    }
}

process printProc()
{
  int i;
  int t = 0;
  intmask mask;
  struct procent* pent;

  while(1)
  {
    ++t;
    mask = disable();
    kprintf("\nTime %d\n", t);
    kprintf("  name   prrecent   prprio    prquantum\n");
    kprintf("---------------------------------------\n");
    for(i=0; i<NPROC; ++i)
    {
      pent = proctab + i;
      if(pent->prstate != PR_FREE) kprintf("%s\t%d\t%d\t%d\n", pent->prname, pent->prrecent, pent->prprio, pent->prquantum);
    }
    kprintf("\n");
    restore(mask);
    sleep(1);
  }
}
