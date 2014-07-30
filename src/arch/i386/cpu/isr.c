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
#include <uniq/regs.h>
#include <string.h>
#include <uniq/kernel.h>

/*
 * istisna kesmeleri mesaj listesi
 */
static const char *fault_msglist[] = {
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

#define MAX_ISR_HANDLER		256
#define INT_GATE_TYPE		0xE
#define INT_PRESENT		0x80
#define INT_GATE		INT_PRESENT | INT_GATE_TYPE	/* kesme kapisi */
#define KERN_CODE_SEGMENT	0x8

static int_handler_t isr_handlers[MAX_ISR_HANDLER] = { NULL };

/*
 * isr_add_handler
 *
 * @param isr_num : istisna kesme numarasi
 * @param handler : kesme isleyicisi
 */
void isr_add_handler(uint8_t isr_num, int_handler_t handler){
	
	isr_handlers[isr_num] = handler;
	
}

/*
 * isr_remove_handler
 *
 * @param isr_num : istisna kesme numarasi
 */
void isr_remove_handler(uint8_t isr_num){
	
	isr_handlers[isr_num] = NULL;
	
}

/*
 * isr_fault, kesme servislerinde hata olusursa kesme numarasi,hata aciklmasi
 * ve kaydediciler ekrana yazdirilir.
 *
 * @param regs : kaydediciler. hangi kaydedicileri icerdigini ogrenmek
 *		 icin registers_t yapisini inceleyin.
 */
static void isr_fault(struct registers_t *regs){
	
	debug_print(KERN_EMERG, "Unhandled exception: [interrupt number = %u] %s", regs->int_num
										 , fault_msglist[regs->int_num]);
	dump_regs(regs);
	halt_system();

}

/*
 * isr_handler, kesme servisi isleyicisidir.
 *
 * @param regs : kaydediciler. hangi kaydedicileri icerdigini ogrenmek
 *		 icin registers_t yapisini inceleyin.
 */
void isr_handler(struct registers_t *regs){
	
	int_handler_t handler = isr_handlers[regs->int_num];
	
	if(!handler)
		isr_fault(regs);
	else
		handler(regs);
		
}

/*
 * isr_load,kesme servisi isleyicilerini iddt tablosuna ekler.
 */
static void isr_load(void){
	
	idt_set_gate(0,  _isr0,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(1,  _isr1,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(2,  _isr2,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(3,  _isr3,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(4,  _isr4,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(5,  _isr5,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(6,  _isr6,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(7,  _isr7,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(8,  _isr8,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(9,  _isr9,  KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(10, _isr10, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(11, _isr11, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(12, _isr12, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(13, _isr13, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(14, _isr14, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(15, _isr15, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(16, _isr16, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(17, _isr17, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(18, _isr18, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(19, _isr19, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(20, _isr20, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(21, _isr21, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(22, _isr22, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(23, _isr23, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(24, _isr24, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(25, _isr25, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(26, _isr26, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(27, _isr27, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(28, _isr28, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(29, _isr29, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(30, _isr30, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(31, _isr31, KERN_CODE_SEGMENT, INT_GATE);
	
}

/*
 * init_isr, kesme servisi rutinlerini baslatir.
 */
void init_isr(void){
	
	debug_print(KERN_INFO,"Initializing the interrupt service routines.");
	memset(isr_handlers,0,sizeof(isr_handlers));
	isr_load();
}

/*
 * int_test
 */
void __int_test(void){
	int x = 0/0;
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
