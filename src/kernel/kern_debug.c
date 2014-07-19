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
 
#include <uniq/kern_debug.h>
#include <uniq/kernel.h>
#include <va_list.h>
#include <uniq/module.h>

#ifdef KDEBUG_DEFAULT
	kern_debug_mode_t kdebug_mode = KERN_DEBUG_DEFAULT;
#else  /* KDEBUG_OOPS */
	kern_debug_mode_t kdebug_mode = KERN_DEBUG_OOPS;
#endif

/*
 * debug seviyesi mesajlari
 */
static char *level_msgs[]={
	"\033[1;32mINFO\033[0m",
	"\033[1;37mNOTICE\033[0m",
	"\033[1;33mWARNING\033[0m",
	"\033[1;31mERROR\033[0m",
	"\033[1;34mCRITICAL\033[0m",
	"\033[1;31mALERT\033[0m",
	"\033[1;36mEMERGENCY\033[0m"
};

/*
 * _debug_print,kernel debug modundayken ekrana log kayitlarini
 * yazdirmak icin kullanilir.
 *
 * @param file : dosya
 * @param line : dosya satir numarasi
 * @param level : kernel debug seviyesi
 * @param fmt : format
 * @param ... : argumanlar
 */
void _debug_print(char *file,uint32_t line,kern_levels_t level,const char *fmt,...){
	
	if((level == KERN_INFO) && (kdebug_mode == KERN_DEBUG_OOPS))
		return;

	char buffer[4096];
	va_list arg_list;
	va_start(arg_list,fmt);
	vsnprintf(buffer,sizeof(buffer)-1,fmt,arg_list);
	va_end(arg_list);
	
	char *lvl_text;
	
	lvl_text = level_msgs[level];
	printf("%-10s: [module = %.40s line = %.6d]\n%s %s\n",lvl_text,file,line,"->",buffer);
	
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
