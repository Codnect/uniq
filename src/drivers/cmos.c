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
#include <uniq/types.h>
#include <drivers/cmos.h>
#include <uniq/kernel.h>
#include <uniq/time.h>

/*
 * http://www.walshcomptech.com/ohlandl/config/cmos_registers.html
 * http://www.bioscentral.com/misc/cmosmap.htm
 * http://wenku.baidu.com/view/8128da380912a21614792957
 * http://www.intel-assembler.it/portale/5/cmos-memory-map-123/cmos-memory-map-123.asp
 * http://wiki.osdev.org/CMOS
 * 
 * adreslerinden cmos ile ilgili detayli arastirmayi yapabilirsiniz.
 *
 */

/*
 * cmos_read, cmos bilgilerini verilen diziye doldurur.
 *
 * @param cmos_index_arr : cmos dizisi
 */
void cmos_read(uint8_t *cmos_index_arr){

	for(uint8_t index = 0;index < CMOS_MAX;index++){

		outbyte(CMOS_ADDR,index);
		cmos_index_arr[index] = inbyte(CMOS_DATA);				

	}

}

/*
 * get_tm_yday, yilin kacinci gunu oldugunu hesaplar.
 *
 * @param time : tm_t yapisi isaretcisi
 */
static void get_tm_yday(tm_t *time){

	uint32_t yday = 0;

	uint8_t month_days[] = { /* 0  */ 31, /* 1  */ 28, /* 2  */ 31,
				 /* 3  */ 30, /* 4  */ 31, /* 5  */ 30,
				 /* 6  */ 31, /* 7  */ 31, /* 8  */ 30,
				 /* 9  */ 31, /* 10 */ 30, /* 11 */ 31 };
 
	switch(time->tm_mon){

		case 11:	/* aralik */
			yday += month_days[11];		
		case 10:	/* kasim */
			yday += month_days[10];
		case 9:		/* ekim */
			yday += month_days[9];
		case 8:		/* eylul */
			yday += month_days[8];
		case 7:		/* agustos */
			yday += month_days[7];
		case 6:		/* temmuz */
			yday += month_days[6];
		case 5:		/* haziran */
			yday += month_days[5];
		case 4:		/* mayis */
			yday += month_days[4];
		case 3:		/* nisan */
			yday += month_days[3];
		case 2:		/* mart */
			yday += month_days[2];
		case 1:		/* subat */
			yday += is_leap_year(time->tm_year) ? 29 : month_days[1];			
		case 0:		/* ocak */
			yday += month_days[0];
		default:
			break;

	}

	if(is_leap_year(time->tm_year) && time->tm_mon == 1)
		time->tm_yday = yday - (29 - time->tm_mday);
	else
		time->tm_yday = yday - (month_days[time->tm_mon] - time->tm_mday);

}

/*
 * get_time, cmos bilgilerini okuyarak tm_t yapisina
 * zamanla ilgili verileri doldurur.
 *
 * @param time : tm_t yapisi isaretcisi
 */
void get_time(tm_t *time){

	uint8_t cmos_value[CMOS_MAX];
	cmos_read(cmos_value);

	time->tm_sec	= bcd_to_bin(cmos_value[CMOS_RTC_SEC]);
	time->tm_min	= bcd_to_bin(cmos_value[CMOS_RTC_MIN]);
	time->tm_hour	= bcd_to_bin(cmos_value[CMOS_RTC_HOUR]);
	time->tm_mday	= bcd_to_bin(cmos_value[CMOS_RTC_MDAY]);
	time->tm_mon	= bcd_to_bin(cmos_value[CMOS_RTC_MON]) - 1;
	time->tm_year	= bcd_to_bin(cmos_value[CMOS_RTC_YEAR]) + (RTC_YEAR - 1900);
	time->tm_wday	= bcd_to_bin(cmos_value[CMOS_RTC_WDAY]);
	get_tm_yday(time);

	time->tm_wday--;
	time->tm_yday--;
	time->tm_isdst	= -1;

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
