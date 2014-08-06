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


#define STR(x)		#x
#define STRSTR		STR(x)

/* 
 * inline asm makrolari & diger
 */
#include <va_list.h>
#include <uniq/types.h>
#include <uniq/port.h>
#include <uniq/asm.h>
#include <compiler.h>

/*
 * drivers 
 */
#include <drivers/vga.h>
#include <drivers/pit.h>

/* 
 * kprintf.c 
 */
extern int vsnprintf(char *strbuf, size_t n , const char *fmt, va_list args);
extern int vsprintf(char *strbuf, const char *fmt, va_list args);
extern int sprintf(char *buf, const char *fmt, ...);
extern int snprintf(char *buf, size_t n, const char *fmt, ...);
extern int printf(const char *fmt, ...);

/*
 * panic.c & debug
 */
#include <uniq/kern_debug.h>
__noreturn void die(const char *fmt, ...);
__noreturn void _assert(const char *err);
#define assert(statement) 	(statement) ? ((void)0) : _assert(#statement)

/*
 * arch/i386
 */
#include <uniq/regs.h>
extern void gdt_init(void);
extern void gdt_set_gate(size_t num,uint32_t base,uint32_t limit,uint8_t access,uint8_t gran);
extern void idt_init(void);
extern void idt_set_gate(uint8_t num,void (*base)(void),uint16_t sel,uint8_t flags);
extern void isr_init(void);
extern void isr_add_handler(uint8_t isr_num, int_handler_t handler);
extern void isr_remove_handler(uint8_t isr_num);
extern void __int_test(void);
extern void irq_init(void);
extern void irq_eoi(uint8_t irq_num);
extern void irq_add_handler(uint8_t irq_num, int_handler_t handler);
extern void irq_remove_handler(uint8_t irq_num);


#endif /* __UNIQ_KERNEL_H__ */
