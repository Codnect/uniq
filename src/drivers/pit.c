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

#define TIMER_IRQ_NUM	0
#define PIT_CHANNEL0	0x40	/* veri portu */
#define PIT_CHANNEL1 	0x41	/* veri portu */
#define PIT_CHANNEL2 	0x42	/* veri portu */
#define PIT_CNTRL	0x43
#define PIT_HZ		100

/*
 * timer_handler
 *
 * @param regs : kaydediciler.
 */
void timer_handler(struct registers_t *regs){
 
 
}

/*
 * timer_set_freq
 *
 * @param hz : hertz
 */
static void timer_set_freq(uint32_t hertz){
 
 
}

/*
 * init_timer
 */
void init_timer(void){
 
	debug_print(KERN_INFO,"Initializing the timer. Hz = \033[1;37m%u",PIT_HZ);
	irq_add_handler(TIMER_IRQ_NUM,timer_handler);
	/* 100 hz'e ayarla */
	timer_set_freq(PIT_HZ);
 
}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
