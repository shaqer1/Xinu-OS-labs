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
	/* Start the producer/consumer simulation */
	start_prod_con();

	/* Sleep for 30 seconds to let the simulation run */
	sleep(30);
	
	/* Stop the producer/consumer simulation */
	stop_prod_con();

	/* Print the report */
	print_report();
	
	return OK;
} /* main */


