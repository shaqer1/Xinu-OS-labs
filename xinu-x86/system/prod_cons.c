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

sid32 mutex, slot, empty_buff, done, insertMutexCons, insertMutexProds;

int32 productionCount, consumptionCount;

int head;
int tail;
pid32 pIDs[NCONSUMERS+NPRODUCERS];

void updateProdCounts(uint8 item){
    //kprintf("adding count to prods\n");
    for(int i =0; i< NPRODUCERS; i++){
        if(producer_tags[i]==item){
            wait(insertMutexProds);
            prod_item_count[i]++;
            signal(insertMutexProds);
            return;
        }
    }
    //kprintf("added count to prods\n");
}

void updateConsCount(uint8 consumer_tag, uint8 val){
    //kprintf("adding count to cons\n");
    for(int j = 0; j < NCONSUMERS; j++){
        if(consumer_tags[j] == consumer_tag){
            for(int i =0; i< NPRODUCERS; i++){
                if(producer_tags[i]==val){
                    wait(insertMutexCons);
                    cons_item_count[j][i]++;
                    signal(insertMutexCons);
                    return;
                }
            }
        }
    }
    //kprintf("added count to cons\n");
}

int32 sumArr(int32  arr [], int size){
    int32 sum =0;
    for (int i =0; i < size; i++){
        sum+= arr[i];
    }
    return sum;
}

/*------------------------------------------------------------------------
 * Function used by producer to insert item into buffer
 *------------------------------------------------------------------------
 */
void insert_buffer(uint8 item)
{
    //kprintf("IN INSERT\n");
    wait(slot);

    wait(mutex);

    buffer[tail] = item;
    tail = (tail+1)% BUFFERSIZE;
    //kprintf("INSERTED\n");

    signal(mutex);
    signal(empty_buff);

} /* insert_buffer */

/*------------------------------------------------------------------------
 * Function used by consumer to remove an item from buffer
 *------------------------------------------------------------------------
 */
uint8 remove_buffer()
{
    //kprintf("IN remove\n");
    //kprintf("sem empty_buff count in cons: %d\n",semcount(empty_buff));
    wait(empty_buff);
    //kprintf("WAIT empty_buff DONE\n");
    wait(mutex);
    signal(done);
    //kprintf("sem done count post signal: %x\n",semcount(done));
    //kprintf("WAIT MUTEX DONE\n");
    uint8 val = buffer[head];
    head = (head+1)%BUFFERSIZE;
    //kprintf("removed\n");
    signal(mutex);
    //kprintf("MUTEX unlocked\n");
    signal(slot);
    //kprintf("SIGNALED slot\n");
    return val;
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
    //kprintf("INPRODUCER\n");

    for(int i =0; i<prod_count; i++){
        insert_buffer(producer_tag);
            updateProdCounts(producer_tag);
    }
    //kprintf("going to sleep in prod\n");

    sleepms(prod_sleep_time);
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
    //kprintf("INCONSUMER\n");

    for(int i =0; i<cons_count; i++){
        uint8 val = remove_buffer();
        updateConsCount(consumer_tag, val);
    }
    //kprintf("sleeping INCONSUMER\n");

    sleepms(cons_sleep_time);
} /* consumer */


/*------------------------------------------------------------------------
 *  start_prod_con  -  Initializes and starts the producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void start_prod_con(void)
{
    mutex = semcreate(1);
    slot = semcreate(BUFFERSIZE);
    empty_buff = semcreate(0);
    insertMutexCons = semcreate(1);
    insertMutexProds = semcreate(1);
    /* counts how many consumtions and productions there will be so it can stop 
    accoridingly when no more changes will take place in simulation. */
    productionCount =sumArr(producer_counts, NPRODUCERS);
    consumptionCount = sumArr(consumer_counts, NCONSUMERS);
    //kprintf("%x %x %x\n",productionCount, consumptionCount, (productionCount <= consumptionCount?productionCount:consumptionCount));
    done = semcreate(0);
    //kprintf("sem init count: %x\n",-1*(productionCount <= consumptionCount?productionCount:consumptionCount));
    //wait(done);
    tail =0;
    head =0;
    int j = 0;
    for(int i =0; i< NPRODUCERS; i++){
        pIDs[j++] = resume(create(producer, 2000, 20, "prod",3,producer_tags[i],producer_counts[i],producer_sleep_times[i]));
    }
    for(int i =0; i< NCONSUMERS; i++){
        pIDs[j++] = resume(create(consumer, 2000, 1, "cons",3,consumer_tags[i],consumer_counts[i],consumer_sleep_times[i]));
    }
} /* start_prod_con */

/*------------------------------------------------------------------------
 *  stop_prod_con  -  Stops the currently executing producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void stop_prod_con(void)
{
    //kprintf("sem done count after simubefore wait: %x\n",semcount(done));
    //wait(done);
    while((productionCount <= consumptionCount?productionCount:consumptionCount) > semcount(done)){
        /* kprintf("waiting");
        sleep(1); */
    }
    /* (productionCount <= consumptionCount?productionCount:consumptionCount) */
    //kprintf("sem done count after simu: %x\n",semcount(done));
  for(int i = 0; i< NPRODUCERS+NCONSUMERS;i++){
      kill(pIDs[i]);
  }
  semdelete(slot);
  semdelete(empty_buff);
  semdelete(mutex);
  semdelete(done);
  semdelete(insertMutexCons);
  semdelete(insertMutexProds);
} /* stop_prod_con */

/*------------------------------------------------------------------------
 *  print_report  -  Prints the final report for the producer/consumer
 *                           simulation
 *------------------------------------------------------------------------
 */
void print_report(void)
{
    for(int i =0; i< NPRODUCERS; i++){
        kprintf("Producer %c: created %d items\n",producer_tags[i],prod_item_count[i]);
    }
    
    kprintf("\n");

    for(int j = 0; j < NCONSUMERS; j++){
        kprintf("Consumer %c:",consumer_tags[j]);
        for(int i =0; i< NPRODUCERS; i++){
            kprintf(" deleted %d items from producer %c",cons_item_count[j][i],producer_tags[i]);
            if(i!=NPRODUCERS-1){
                kprintf(",");
            }
        }
        kprintf("\n");
    }

    kprintf("\nThe shared buffer contains:");
    int32 freqLeft[NPRODUCERS] = {0};
    int traversalSize = head > tail?(BUFFERSIZE+tail):(tail);
    //kprintf("%x %x %x head tail size repectively\n", head, tail, traversalSize);
    int i =head, difference =0;
    while( i<traversalSize){
        for(int j = 0; j< NPRODUCERS; j++){
            if(buffer[i-difference]==producer_tags[j]){
                freqLeft[j]++;
            }
        }
        i++;
        if(i<traversalSize && i == BUFFERSIZE){
            difference = BUFFERSIZE;
        }
    }

    for(int i =0; i<NPRODUCERS; i++){
        kprintf(" %d items from producer %c",freqLeft[i],producer_tags[i]);
        if(i!=NPRODUCERS-1){
            kprintf(",");
        }
    }
    kprintf("\n");

} /* print_report */

