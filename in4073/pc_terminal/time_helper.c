#include "time_helper.h" 
 
// adds milliseconds to timespec struct 
struct timespec add_time_millis (const struct timespec * time, int millis) 
{ 
  struct timespec return_time = *time; 
  // 359755019 
  return_time.tv_nsec += millis * MILLI_SEC; 
  while(return_time.tv_nsec > (SECOND)) 
  { 
    return_time.tv_sec++; 
    return_time.tv_nsec -= SECOND; 
  } 
 
  return return_time; 
} 
 
// returns time 1 > time 2 
bool compare_time(const struct timespec * time1, const struct timespec * time2) 
{ 
  if (time1->tv_sec > time2->tv_sec) return true; 
  if (time1->tv_sec < time2->tv_sec) return false; 
  if (time1->tv_nsec > time2->tv_nsec) return true; 
  return false; 
}