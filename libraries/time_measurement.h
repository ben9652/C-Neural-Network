#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H

typedef enum {
	MEASURE_MICROSECONDS,
	MEASURE_MILISECONDS,
	MEASURE_SECONDS
} Time_Unit;

void init_time_measurement();
void finalize_time_measurement();
float get_elapsed_time(Time_Unit unit);

#endif