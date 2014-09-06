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

#ifndef __UNIQ_TYPES_H__
#define __UNIQ_TYPES_H__

/*
 * temel veri tipleri
 */
typedef signed char 		s8;
typedef unsigned char 		u8;
typedef signed short	 	s16;
typedef unsigned short		u16;
typedef signed int 		s32;
typedef unsigned int 		u32;
typedef signed char 		int8_t;
typedef unsigned char 		uint8_t;
typedef signed short 		int16_t;
typedef unsigned short 		uint16_t;
typedef signed int 		int32_t;
typedef unsigned int 		uint32_t;

/* ... */
typedef long long int64_t;
typedef unsigned long long uint64_t;

/*
 * mimari veri tipleri
 */
typedef signed char 		sbyte_t;
typedef unsigned char 		byte_t;
typedef signed short 		sword_t;
typedef unsigned short 		word_t;
typedef signed int 		sdword_t;
typedef unsigned int 		dword_t;


/*
 * diger veri tipleri
 */
typedef signed int 		intptr_t;
typedef unsigned int 		uintptr_t;

typedef char bool;
typedef char boolean;
#define true  (1)
#define false (0)

#define NULL	((void*)0)
#define null	NULL

#define offsetof(type,member) 	((size_t) &((type*)0)->member)
#define array_size(x)		(sizeof(x) / sizeof((x)[0]))

/*
 * veri tipi limitleri
 */
#define UINT8_MAX		0xff
#define UINT8_MIN		0
#define INT8_MAX		0x7f
#define INT8_MIN		-(0x7f+1)

#define UINT16_MAX		0xffff
#define UINT16_MIN		0
#define INT16_MAX		0x7fff
#define INT16_MIN		-(0x7fff+1)

#define UINT32_MAX		0xffffffff
#define UINT32_MIN		0
#define INT32_MAX		0x7fffffff
#define INT32_MIN		-(0x7fffffff+1)

#include <uniq/posix_types.h>

#endif /* __UNIQ_TYPES_H__ */
