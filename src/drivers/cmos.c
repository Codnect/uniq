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
 * cmos_read,
 *
 * @param cmos_index_arr
 */
void cmos_read(uint8_t *cmos_index_arr){

	for(uint8_t index = 0;index < CMOS_MAX;index++){

		outbyte(CMOS_ADDR,index);
		cmos_index_arr[index] = inbyte(CMOS_DATA);				

	}

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
