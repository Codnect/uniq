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
#include <uniq/kernel.h>

/*
 * istisna kesmeleri mesaj listesi
 */
static const char *exception_msglist[] = {
	"Division by zero",				/* 0 */
	"Debug",					/* 1 */
	"Non-maskable interrupt",			/* 2 */
	"Breakpoint",					/* 3 */
	"Overflow",					/* 4 */
	"Bound Range Exceeded",				/* 5 */
	"Invalid opcode",				/* 6 */
	"No coprocessor",				/* 7 */
	"Double fault",					/* 8 */
	"Coprocessor segment overrun",			/* 9 */
	"Invalid TSS",					/* 10 */
	"Segment not present",				/* 11 */
	"Stack Segment Fault",				/* 12 */
	"General protection fault",			/* 13 */
	"Page fault",					/* 14 */
	"Unknown interrupt",				/* 15 */
	"Coprocessor fault",				/* 16 */
	"Alignment check",				/* 17 */
	"Machine check",				/* 18 */
	"Reserved",
	"Reserved",			
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

/*
 * istisna kesmeleri isleyicileri
 */
extern void _isr0 (void);
extern void _isr1 (void);
extern void _isr2 (void);
extern void _isr3 (void);
extern void _isr4 (void);
extern void _isr5 (void);
extern void _isr6 (void);
extern void _isr7 (void);
extern void _isr8 (void);
extern void _isr9 (void);
extern void _isr10(void);
extern void _isr11(void);
extern void _isr12(void);
extern void _isr13(void);
extern void _isr14(void);
extern void _isr15(void);
extern void _isr16(void);
extern void _isr17(void);
extern void _isr18(void);
extern void _isr19(void);
extern void _isr20(void);
extern void _isr21(void);
extern void _isr22(void);
extern void _isr23(void);
extern void _isr24(void);
extern void _isr25(void);
extern void _isr26(void);
extern void _isr27(void);
extern void _isr28(void);
extern void _isr29(void);
extern void _isr30(void);
extern void _isr31(void);

/*
 * fault_err
 *
 * @param regs : kaydediciler. hangi kaydedicileri icerdigini ogrenmek
 *		 icin registers_t yapisini inceleyin.
 */
void fault_err(struct registers_t *regs){

	
}

/*
 * fault_handler
 *
 * @param regs : kaydediciler. hangi kaydedicileri icerdigini ogrenmek
 *		 icin registers_t yapisini inceleyin.
 */
void fault_handler(struct registers_t *regs){

	
}

/*
 * init_isr
 */
void init_isr(void){
	
	
}

/*
 * int_test
 */
void __int_test(void){
	
	
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
