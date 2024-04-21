#include "time_measurement.h"
#include <Windows.h>

static LARGE_INTEGER start_time;
static LARGE_INTEGER end_time;
static LARGE_INTEGER frecuency;

void init_time_measurement()
{
	QueryPerformanceFrequency(&frecuency);
	QueryPerformanceCounter(&start_time);
}

void finalize_time_measurement()
{
	QueryPerformanceCounter(&end_time);
}

float get_elapsed_time(Time_Unit unit)
{
	float time_in_s = (float)(end_time.QuadPart - start_time.QuadPart) / frecuency.QuadPart;

	if (unit == MEASURE_MICROSECONDS)
		return time_in_s * 1e6f;

	else if (unit == MEASURE_MILISECONDS)
		return time_in_s * 1e3f;

	else
		return time_in_s;
}
