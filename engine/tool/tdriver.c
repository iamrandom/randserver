/*
 * tdriver.c
 *
 *  Created on: 2015年10月14日
 *      Author: Random
 */

#include <time.h>
#include "tdriver.h"

#define CLOCK_PER_MY_SEC CLOCKS_PER_SEC/((clock_t)1000)

static uint64_t
msec_clock()
{
#if defined(__WINDOWS_)|| defined( _WIN32 )|| defined( _WIN64 )|| defined(WIN32) || defined(WIN64)
	return (uint64_t)clock();
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
#endif
}

void
time_driver_init(time_driver* td)
{
	tzset();
	
	td->sec_tick = 0;
	td->msec100_tick = 0;
	td->pass_sec = 0;
	td->pass_100msec = 0;
	td->UTC_base_seconds = (unsigned int)(time(0));
	td->UTC_pass_seconds = 0;
	td->u_clock = msec_clock();

	time_driver_update(td);
}


void
time_driver_update(time_driver* td)
{
	uint64_t nowClock;
	uint64_t teampClock;
	uint32_t uTemp = 0;

	nowClock = msec_clock();
	teampClock = (uint64_t)nowClock;
	
	if (teampClock < td->u_clock)
	{
		nowClock = msec_clock();
		teampClock = (uint64_t)nowClock;
		if (teampClock < td->u_clock)
		{
			uTemp = nowClock;
		}
		else
		{
			uTemp = (uint32_t)(teampClock - td->u_clock);
		}
	}
	else
	{
		uTemp = (uint32_t)(teampClock - td->u_clock);
		
	}
	
	td->u_clock = teampClock;
	time_driver_pass_msec(td, uTemp);

}


void
time_driver_pass_msec(time_driver* td, uint32_t msec)
{
	time_t tt;
	td->sec_tick += msec;
	td->msec100_tick += msec;

	//update second tick
	td->pass_sec = (td->sec_tick / 1000);
	td->sec_tick %= 1000;

	// update 100msecond tick
	td->pass_100msec = (td->msec100_tick / 100);
	td->msec100_tick %= 100;

	td->UTC_pass_seconds += td->pass_sec;
	tt = (time_t)(td->UTC_pass_seconds + td->UTC_base_seconds);

//#ifdef _WIN32_WINNT
//	localtime_s(&td->tm_time, &tt);
//#else
	td->tm_time = *localtime(&tt);
//#endif
}

uint32_t
time_driver_get_new_sec(time_driver* td)
{
	return td->pass_sec;
}

uint32_t
time_driver_get_new_100msec(time_driver* td)
{
	return td->msec100_tick;
}


