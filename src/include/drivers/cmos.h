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

#ifndef __UNIQ_CMOS_H__
#define __UNIQ_CMOS_H__

#include <uniq/time.h>

#define CMOS_RTC_SEC		0x00			/* saniye */
#define CMOS_RTC_MIN		0x02			/* dakika */
#define CMOS_RTC_HOUR		0x04			/* saat */
#define CMOS_RTC_WDAY		0x06			/* haftanin kacinci gunu */
#define CMOS_RTC_MDAY		0x07			/* ayin kacinci gunu */
#define CMOS_RTC_MON		0x08			/* ay */
#define CMOS_RTC_YEAR		0x09			/* yil */

#define CMOS_ADDR		0x70
#define CMOS_DATA		0x71
#define CMOS_MAX_INDEX		0x7f
#define CMOS_MAX		CMOS_MAX_INDEX + 1

#define	RTC_YEAR		2000
#define bcd_to_bin(x)  		((x / 16) * 10 + (x & 0xf))
#define is_leap_year(x)		!(x % 4) && ((x % 100) || !(x % 400))
void cmos_read(uint8_t *cmos_index_arr);
void get_time(tm_t *time);

#endif /* __UNIQ_CMOS_H__ */
