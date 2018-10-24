/*	releaseall.c - releaseall	*/
#include <xinu.h>
#include <stdarg.h>

/* Lab 3: Complete this function */

syscall releaseall (int32 numlocks, ...) {

	//your implementation goes here
	
	va_list ap;

	va_start(ap, numlocks);
	for(int32 i =0; i< numlocks; i++){
		va_arg(ap, int32);
		
	}
	va_end(ap);
	return OK;
}
