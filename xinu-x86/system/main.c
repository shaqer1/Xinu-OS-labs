/*  main.c  - main */

#include <xinu.h>

/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT MIGHT BE REPLACED DURING TESTING                                 */
/*                                                                      */
/*  DEFINE ALL NECESSARY VARIABLES IN prod_cons.c                       */
/************************************************************************/

extern void start_prod_con(void);
extern void stop_prod_con(void);
extern void print_report(void);

/************************************************************************/
/*
/* main - main program that Xinu runs as the first user process
/*
/************************************************************************/

int main(int argc, char **argv)
{

	//kprintf("HERE1\n");
	/* Start the producer/consumer simulation */
	start_prod_con();

	//kprintf("HERE2\n");
	/* Sleep for 30 seconds to let the simulation run */
	//sleep(15);
	
	/* Stop the producer/consumer simulation */
	stop_prod_con();

	//kprintf("HERE3\n");

	/* Print the report */
	print_report();
	
	//kprintf("HERE4\n");
	return OK;
} /* main */


