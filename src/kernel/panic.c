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

#include <va_list.h>
#include <uniq/kernel.h>
#include <uniq/types.h>
#include <uniq/module.h>

/*
 * die,kendisine verilen formatli yada formatsiz karakter
 * dizisini hata mesaji olarak ekrana yazdirir ve debug_print'i 
 * direk kullanirken sistem calismaya devam ederken 'die' ile
 * sistem durdurulur.
 *
 * @param fmt : format
 * @param ... : argumanlar
 */
void die(const char *fmt, ...){

	char err_msg[4096];
	va_list arg_list;
	va_start(arg_list,fmt);
	vsnprintf(err_msg,sizeof(err_msg)-1,fmt,arg_list);
	debug_print(KERN_EMERG,"%s",err_msg);
	va_end(arg_list);
	disable_interrupts();
	halt_system();

}

/*
 * _assert ile ilgili uniq/kernel.h'deki tanimlanan macroya bakildiginda
 * kullanimi hakkinda daha cok bilgiye sahip olabilirsiniz.(debug_print 
 * seklinde yazdirildigi icin satir numarasi ve dosya'ya ihtiyac yoktur.)
 *
 * @param err : hata mesaji
 */
void _assert(const char *err){
	
	debug_print(KERN_EMERG, "Kernel Fault : %s",err);
	disable_interrupts();
	halt_system();

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
