/*------------------------------------------------------------------------
 *  prod_cons.h  - definitions for the producer/consumer simulation
 *------------------------------------------------------------------------
 */
/************************************************************************/
/*  DO NOT DEFINE DEPENDENT VARIABLES IN THIS FILE                      */
/*   IT WILL BE REPLACED DURING TESTING                                 */
/*                                                                      */
/*  DEFINE ALL NECESSARY VARIABLES IN prod_cons.c                       */
/************************************************************************/


/* NPRODUCERS : The number of producer processes. 
 * You can assume the value will always be greater than zero.	
 */
#define NPRODUCERS	5

/* NCONSUMERS: The number of consumer processes. 
 * You can assume the value will always be greater than zero.	
 */		
#define NCONSUMERS	4

/* BUFFERSIZE: The size of the shared buffer. 
 * You can assume the value will always be greater than zero. 
 */
#define BUFFERSIZE	200


/* Globals variables */

/* producer_tags: An array of tags (identifiers) for the producer processes. 
 * The tag for the first producer is producer_tags[0], 
 * The tag for the second producer is producer_tags[1], etc. 
 * This array will always be NPRODUCERS in size.
 */
uint8 producer_tags[NPRODUCERS] = { 'A', 'B', 'C', 'D', 'E' };	

/* producer_sleep_times: An array of values representing the time a 
 * producer sleeps between creating items. The values are specified
 * in milliseconds (ms). The sleep time for the first producer
 * is producer_sleep_times[0]. The sleep time for the second 
 * producer is producer_sleep_times[1], etc. The array will contain 
 * exactly NPRODUCERS locations.
 */
int32 producer_sleep_times[NPRODUCERS] = { 1000, 1000, 1000, 1000, 1000 };

/* producer_counts: an array of values representing the number
 * of items each producer produces each time interval. 
 * The number for the first producer is producer_counts[0]. 
 * The number for the second producer is producer_counts[1], etc. 
 * The array will contain exactly NPRODUCERS locations.
 */
int32 producer_counts[NPRODUCERS] = { 40, 40, 40, 40, 40 };


/* consumer_tags: An array of tags (identifiers) for the consumer processes. 
 * The tag for the first consumer is consumer_tags[0]. The tag
 * for the second consumer is consumer_tags[1], etc. This array 
 * will contain exactly NCONSUMERS locations.
 */
uint8 consumer_tags[NCONSUMERS] = { 'a', 'b', 'c', 'd' };

/* consumer_sleep_times: An array of values representing the 
 * time each consumer sleeps before deleting more items. The 
 * values specified are in milliseconds (ms). The sleep time 
 * for the first consumer is consumer_sleep_times[0]. The sleep time 
 * for the second consumer is consumer_sleep_times[1], etc. The array
 * will contain exactly NCONSUMERS locations.
 */
int32 consumer_sleep_times[NCONSUMERS] = { 1000, 1000, 1000, 1000 };

/* consumer_counts: an array of values representing the number 
 * of items each consumer consumes before returning to sleep. 
 * The value for the first consumer is consumer_counts[0]. The value 
 * for the second consumer is consumer_counts[1], etc. The array 
 * will contain exactly NCONSUMERS locations.
 */
int32 consumer_counts[NCONSUMERS] = { 50, 50, 50, 50 };

/* Declarations for functions used by producer/consumer simulation	
 * in prod_cons.c
 */
void start_prod_con(void);
void stop_prod_con(void);
void print_report(void);

