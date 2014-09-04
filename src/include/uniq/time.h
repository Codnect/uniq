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

typedef struct{
	time_t tm_sec;		/* saniye */
	time_t tm_min;		/* dakika */
	time_t tm_hour;		/* saat */
	time_t tm_mday;		/* ayin kacinci gunu */
	time_t tm_mon;		/* ay */
	time_t tm_year;		/* yil */
	time_t tm_wday;		/* haftanin kacinci gunu */
	time_t tm_yday;		/* yilin kacinci gunu */
	time_t tm_isdst;	/* yaz saati uygulamasi */
}tm_t;

typedef struct{
	time_t tv_sec;		/* saniye */
	suseconds_t tv_usec;	/* mikrosaniye */
}timeval_t;

#endif /* __UNIQ_TIME_H__ */
