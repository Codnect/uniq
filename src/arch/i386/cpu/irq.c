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
#include <uniq/kernel.h>

/*
 * irq isleyicileri
 */
extern void _irq0(void);
extern void _irq1(void);
extern void _irq2(void);
extern void _irq3(void);
extern void _irq4(void);
extern void _irq5(void);
extern void _irq6(void);
extern void _irq7(void);
extern void _irq8(void);
extern void _irq9(void);
extern void _irq10(void);
extern void _irq11(void);
extern void _irq12(void);
extern void _irq13(void);
extern void _irq14(void);
extern void _irq15(void);

#define MAX_IRQ  16
#define INT_GATE_TYPE		0xE
#define INT_PRESENT		0x80
#define INT_GATE		INT_PRESENT | INT_GATE_TYPE	/* kesme kapisi */
#define KERN_CODE_SEGMENT	0x8

static int_handler_t irq_handlers[MAX_IRQ] = { NULL };

/*
 * irq_add_handler
 *
 * @param irq_no :
 */
void irq_add_handler(uint8_t irq_no, int_handler_t handler) {
	
	if(irq_no >= MAX_IRQ)
		return;
		
	irq_handlers[irq_no] = handler;
	
}

/*
 * irq_remove_handler
 *
 * @param irq_no :
 */
void irq_remove_handler(uint8_t irq_no) {

	if(irq_no >= MAX_IRQ)
		return;
		
	irq_handlers[irq_no] = NULL;
	
}

/*
 * irq_set_gates, donanim kesmelerini idt tablosuna ekler.
 */
void irq_set_gates(void){
	
	idt_set_gate(32, _irq0, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(33, _irq1, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(34, _irq2, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(35, _irq3, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(36, _irq4, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(37, _irq5, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(38, _irq6, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(39, _irq7, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(40, _irq8, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(41, _irq9, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(42, _irq10, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(43, _irq11, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(44, _irq12, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(45, _irq13, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(46, _irq14, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(47, _irq15, KERN_CODE_SEGMENT, INT_GATE); 
 
}

/*
 * irq_remap
 */
void irq_remap(void){
 
 
}

/*
 * irq_eoi
 * 
 * @param irq_num : irq numarasi
 */
void irq_eoi(uint8_t irq_num){

	
}

/*
 * irq_handler, irq isleyicisidir.
 *
 * @param regs : kaydediciler. hangi kaydedicileri icerdigini ogrenmek
 *		 icin registers_t yapisini inceleyin.
 */
void irq_handler(struct registers_t *regs){

	int_handler_t handler = NULL;
	
	if(regs->int_num > 31  && regs->int_num < 48)
		handler = irq_handlers[regs->int_num - 32];
	
	if(!handler)
		irq_eoi(regs->int_num - 32);
	handler(regs);
	
}

/*
 * init_irq
 */
void init_irq(void){
 
 
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
