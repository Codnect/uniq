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

#ifndef __UNIQ_REGS_H__
#define __UNIQ_REGS_H__

#include <uniq/types.h>

typedef struct registers{
	uint32_t gs, fs, es, ds;                          /* selektorler; ds = veri segmenti, es = extra segment,
	                                                  * gs, fs diger segment yazmaclari.
	                                                  */
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pusha ile stack'a atilir. */
	uint32_t int_num, err_code;                       /* int_num : kesme numarasi , err_code : hata kodu */
	uint32_t eip, cs, eflags, useresp, ss;            /* islemci tarafindan stack'a atilir. */
}registers_t;

void dump_regs(registers_t *regs);
typedef void (*int_handler_t) (registers_t *);

#endif /* __UNIQ_REGS_H__ */
