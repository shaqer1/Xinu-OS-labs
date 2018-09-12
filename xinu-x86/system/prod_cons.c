/*------------------------------------------------------------------------
 *  prod_cons.c  - implementation of the producer/consumer simulation
 *------------------------------------------------------------------------
 */

#include <xinu.h>
#include "prod_cons.h"

/* the global shared buffer that contains the data */
uint8 buffer[BUFFERSIZE];	

/* array to keep track of number of items produced by each producer */
int32 prod_item_count[NPRODUCERS]={0};	

/* 2D array to keep track of number of items consumed for each producer-consumer pair */
int32 cons_item_count[NCONSUMERS][NPRODUCERS]={{0}};  


/* insert other global variables here */

/*------------------------------------------------------------------------
 * Function used by producer to insert item into buffer
 *------------------------------------------------------------------------
 */
void insert_buffer(uint8 item)
{
  
} /* insert_buffer */

/*------------------------------------------------------------------------
 * Function used by consumer to remove an item from buffer
 *------------------------------------------------------------------------
 */
uint8 remove_buffer()
{
  
} /* remove_buffer */

/*------------------------------------------------------------------------
 * Function to implement producer process. It has the following parameters:
 * producer_tag: a label that consists of a single character used to 
 * label items produced and in the output is used to identify the producer.
 * prod_count: An integer that specifies the number of items to 
 * insert during each time interval.
 * prod_sleep_time: An integer that specifies the number of milliseconds 
 * to delay between each interval.
 *------------------------------------------------------------------------
 */
void producer(uint8 producer_tag, int32 prod_count, int32 prod_sleep_time)
{
  
} /* producer */

/*------------------------------------------------------------------------
 * Function to implement consumer process. It has the following parameters:
 * consumer_tag: a label that consists of a single character and is used in 
 * the output to identify the consumer.
 * cons_count: An integer that specifies the number of items to extract 
 * during each time interval.
 * cons_sleep_time: An integer that specifies the number of milliseconds 
 * to delay between each interval.
 *------------------------------------------------------------------------
 */
void consumer(uint8 consumer_tag, int32 cons_count, int32 cons_sleep_time)
{
  
} /* consumer */


/*------------------------------------------------------------------------
 *  start_prod_con  -  Initializes and starts the producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void start_prod_con(void)
{
  
} /* start_prod_con */

/*------------------------------------------------------------------------
 *  stop_prod_con  -  Stops the currently executing producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void stop_prod_con(void)
{
  
} /* stop_prod_con */

/*------------------------------------------------------------------------
 *  print_report  -  Prints the final report for the producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void print_report(void)
{
  
} /* print_report */

