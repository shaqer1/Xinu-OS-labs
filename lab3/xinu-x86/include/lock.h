/*  lock.h	*/



// declare variables, #defines, 

#define DELETED (-4)
#define READ 0
#define WRITE 1
#define USED 2
#define FREE 3

struct	lockent {
	//struct memberss
	byte lstate;
/* 	uint16 time;
 */	int32 readcount;
	qid16 readQueue;
	int32 rWaitCount;
	int32 wWaitCount;
	qid16 writeQueue;
};


struct	lockent locktab[NLOCKS];

#define	isbadlock(l)	((int32)(l) < 0 || (l) >= NLOCKS)


int32 lcreate (void);
syscall ldelete (int32 lockdescriptor);
syscall lock (int32 ldes1, int32 type, int32 lpriority);
syscall releaseall (int32 numlocks, ...);


/* Lab 3 lock table */

//extern struct lockent locktab[NLOCKS];
