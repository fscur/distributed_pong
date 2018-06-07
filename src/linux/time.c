#define _GNU_SOURCE

#include <sys/timeb.h>
#include <time.h>
#include "../game/types.h"
#include "../game/time.h"

typedef struct timeb Timespan;
typedef struct timespec Time_Spec;

double
to_seconds(Time_Spec* ts) {
  return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000000.0;
}

f64
time_now_seconds() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  return now.tv_sec + now.tv_nsec / 1000000000.0;
}

long
milliseconds_elapsed(Timespan start, Timespan end) {
  return (long)(1000.0 * (end.time - start.time) +
                (end.millitm - start.millitm));
}

void
spin(u32 milliseconds) {
  Timespan start, end;
  ftime(&start);
  ftime(&end);
  while (milliseconds_elapsed(start, end) < milliseconds) {
    ftime(&end);
  }
}