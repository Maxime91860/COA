#ifndef __TIMER__H
#define __TIMER__H

#include <sys/time.h>

#define TIMER_INIT long t_start, t_end;
#define TIMER_START t_start=usecs();
#define TIMER_END t_end=usecs();
#define TIMER_RESULT_IN_SECONDS (double)(t_end-t_start)/1E6
#define TIMER_PRINT printf("ELAPSED TIME: %.2g s\n", TIMER_RESULT_IN_SECONDS);

static inline long usecs (void) {
	struct timeval t;
	gettimeofday(&t,NULL);

	return t.tv_sec*1000000+t.tv_usec;
}

#endif
