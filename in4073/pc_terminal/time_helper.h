#ifndef _TIME_HELPER_H_
#define _TIME_HELPER_H_

#include <time.h>
#include <stdbool.h>

#define NANO_SEC 1
#define MICRO_SEC (NANO_SEC * 1000)
#define MILLI_SEC (MICRO_SEC * 1000)
#define SECOND (MILLI_SEC * 1000)

struct timespec add_time_millis (const struct timespec * time, int millis);
bool compare_time(const struct timespec * time1, const struct timespec * time2);

#endif