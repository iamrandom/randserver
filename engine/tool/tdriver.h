/*
 * driver.h
 *
 *  Created on: 2015年10月14日
 *      Author: Random
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include <stdint.h>
#include <time.h>

typedef struct
{
	uint64_t					UTC_base_seconds;
	uint32_t					UTC_pass_seconds;
	uint64_t					u_clock;
	uint32_t					sec_tick;
	uint32_t					msec100_tick;
	uint32_t					pass_sec;
	uint32_t					pass_100msec;
	struct tm					tm_time;
}time_driver;

void							time_driver_init(time_driver* td);
void							time_driver_update(time_driver* td);
void							time_driver_pass_msec(time_driver* td, unsigned int msec);
uint32_t						time_driver_get_new_sec(time_driver* td);
uint32_t						time_driver_get_new_100msec(time_driver* td);


#endif /* DRIVER_H_ */
