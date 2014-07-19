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

#ifndef __UNIQ_KERN_DEBUG_H__
#define __UNIQ_KERN_DEBUG_H__

#include <uniq/kern_lvls.h>

typedef enum{
	KERN_DEBUG_DEFAULT = 0,	/* tum log durumlarini icerir. */
	KERN_DEBUG_OOPS		/* bilgilendirme log durumlari haric. */
}kern_debug_mode_t;

void _debug_print(char *file,int line,kern_levels_t level,const char *fmt,...);

/* modul ismi */
#ifndef MODULE_NAME
	#define MODULE_NAME	__FILE__
#endif

/* modul satir numarasi */
#ifndef MODULE_LINE
	#define MODULE_LINE	__LINE__
#endif

#if defined(KDEBUG_DEFAULT) || defined(KDEBUG_OOPS)
	#define debug_print(level,...)		_debug_print(MODULE_NAME,MODULE_LINE,level,__VA_ARGS__)
#else
	#define debug_print(level,...)
#endif

#endif /* __UNIQ_KERN_DEBUG_H__ */
