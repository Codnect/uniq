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

#define TIMER_IRQ_NUM  0


/*
 * timer_handler
 *
 * @param regs : kaydediciler.
 */
void timer_handler(struct registers_t *regs){
 
 
}

/*
 * timer_frequency
 *
 * @param hz : hertz
 */
static void timer_frequency(uint32_t hertz){
 
 
}

/*
 * init_timer
 */
void init_timer(void){
 
	debug_print(KERN_INFO,"Initializing the timer.");
	irq_add_handler(TIMER_IRQ_NUM,timer_handler);
	/* 100 hz'e ayarla */
	timer_frequency(100);
 
}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
