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

#ifndef __UNIQ_TIME_H__
#define __UNIQ_TIME_H__

#include <uniq/types.h>

/*
 * saniye cinsinden zaman tanimlamalari
 */
#define TIME_MINUTE	60
#define TIME_HOUR	TIME_MINUTE * 60
#define TIME_DAY	TIME_HOUR * 24
#define TIME_YEAR	TIME_DAY * 365

typedef struct tm{
	time_t tm_sec;		/* saniye (0-61) */
	time_t tm_min;		/* dakika (0-59) */
	time_t tm_hour;		/* saat (0-23) */
	time_t tm_mday;		/* ayin kacinci gunu (1-31) */
	time_t tm_mon;		/* ay (0-11) */
	time_t tm_year;		/* yil (1900'dan itibaren) */
	time_t tm_wday;		/* haftanin kacinci gunu, pazar=0 (0-6) */
	time_t tm_yday;		/* yilin kacinci gunu (0-365) */
	time_t tm_isdst;	/* yaz saati uygulamasi */
}tm_t;

typedef struct timeval{
	time_t tv_sec;		/* saniye */
	suseconds_t tv_usec;	/* mikrosaniye */
}timeval_t;

void __dump_time_test(void);
int32_t gettimeofday(timeval_t *timeval,void *tz);

#endif /* __UNIQ_TIME_H__ */
