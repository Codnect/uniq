/*
 *  Copyright(C) 2014 Codnect Team
 *  Copyright(C) 2014 Burak Köken
 *
 *  This file is part of Uniq.
 *  
 *  Uniq is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2 of the License.
 *
 *  Uniq is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Uniq.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <uniq/module.h>
#include <uniq/kernel.h>
#include <uniq/time.h>
#include <drivers/cmos.h>

static tm_t kern_start_time;

/*
 * time_init,
 */
void time_init(void){
 

}

/*
 * __dump_time_test,
 */
void __dump_time_test(void){

	debug_print(KERN_INFO,"time test...");
	tm_t time;
	get_time(&time);

	debug_print(KERN_DUMP,"second : %u, minute : %u, hour : %u, year : %u",time.tm_sec,
									       time.tm_min,
									       time.tm_hour,
									       time.tm_year);
	debug_print(KERN_DUMP,"month : %u, mday : %u, wday : %d, yday : %u",time.tm_mon,
								 	    time.tm_mday,
								 	    time.tm_wday,
								 	    time.tm_yday);


}

/*
 * gettimeofday,
 *
 * @param timeval :
 * @param tz :
 */
int32_t gettimeofday(timeval_t *timeval,void *tz){

	tm_t time;
	get_time(&time);
	time.tm_mon++;
	time.tm_yday++;

	int32_t time_sec,days,years;
	time_sec = days = 0;
	

	years = 1900 + time.tm_year - 1;
	while(years > 1969){
		
		days += 365;

		if(is_leap_year(years))
			days++;

		years--;

	}

	time_sec += days * TIME_DAY;
	time_sec += (time.tm_yday - 1) * TIME_DAY;
	time_sec += time.tm_hour * TIME_HOUR;
	time_sec += time.tm_min * TIME_MINUTE;
	time_sec += time.tm_sec;
	
	timeval->tv_sec = time_sec;
	timeval->tv_usec = 0;

 	return 0;

}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
