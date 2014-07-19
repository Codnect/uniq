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

#ifndef __UNIQ_KERNEL_H__
#define __UNIQ_KERNEL_H__

#include <va_list.h>

#define STR(x)		#x
#define STRSTR		STR(x)

/* inline asm makrolari & diger*/
#include <uniq/port.h>
#include <uniq/asm.h>

/* drivers */
#include <drivers/vga.h>

/* kprintf.c */
extern int vsnprintf(char *strbuf, size_t n , const char *fmt, va_list args);
extern int vsprintf(char *strbuf, const char *fmt, va_list args);
extern int sprintf(char *buf, const char *fmt, ...);
extern int snprintf(char *buf, size_t n, const char *fmt, ...);
extern int printf(const char *fmt, ...);

/* panic.c & debug */
#include <uniq/kern_debug.h>
void die(const char *fmt, ...);
void _assert(const char *err);
#define assert(statement) (statement) ? ((void)0) : _assert(#statement)

#endif /* __UNIQ_KERNEL_H__ */
