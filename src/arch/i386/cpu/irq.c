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
#include <uniq/types.h>
#include <uniq/regs.h>

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
static int_handler_t irq_handlers[MAX_IRQ] = { NULL };

/*
 * irq_add_handler
 *
 * @param irq_no :
 */
void irq_add_handler(uint8_t irq_no, int_handler_t handler) {
	
	
}

/*
 * irq_remove_handler
 *
 * @param irq_no :
 */
void irq_remove_handler(uint8_t irq_no) {

	
}

/*
 * irq_set_gates
 */
void irq_set_gates(void){
 
 
}

/*
 * irq_remap
 */
void irq_remap(void){
 
 
}

/*
 * irq_handler
 */
void irq_handler(struct registers_t *regs){


}

/*
 * init_irq
 */
void init_irq(void){
 
 
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
