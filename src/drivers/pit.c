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
 *
 *  Programmable Interrupt Timer
 */
 
#include <uniq/module.h>
#include <uniq/kernel.h>

#define TIMER_IRQ_NUM	0
#define PIT_CHANNEL0_DATA	0x40	/* veri portu - timer icin */
#define PIT_CHANNEL1_DATA 	0x41	/* veri portu - dinamik ram yenileme icin*/
#define PIT_CHANNEL2_DATA	0x42	/* veri portu - speaker icin*/
#define PIT_CNTRL		0x43
#define PIT_HZ			100	/* bizim icin ideal hz */
#define PIT_CLOCK		1193180	/* PIT calisma frekansi 1193180 Hz'dir. buna gore
					 * kendi frekansimizi ayarlariz.
				 	 */


/*
 *  asagidaki set byte yapisi
 *
 *  7         6 5       4 3       1    0
 *  =====================================
 *  = channel  =   rw    = mode   = BCD =
 *  =====================================
 *  channel : PIT kanallari
 *  rw(okuma/yazma): 0 1 = sadece low byte
 *       	     1 0 = sadece high byte
 *                   1 1 = ilk once  low byte, sonra high byte
 *  mode : 
 *	 0 0 0  mode 0
 *	 0 0 1  mode 1
 *	 0 1 0  mode 2
 *	 0 1 1  mode 3 -> timer isleyicisi duzenli olarak aktif oldugundan
 *			  mode 3'u seciyoruz. modlar  hakkinda daha detayli
 *			  bilgi icin netten arastirma yapabilirsiniz. ama bu 
 *			  bilgiler bize yeterli :).
 *	 1 0 0  mode 4
 *	 1 0 1  mode 5
 *  bcd  : 0 = 16-bit binary, 1 = four-digit BCD
 *
 */
 
/* channels */
#define PIT_CHANNEL0		0x00
#define PIT_CHANNEL1		0x40
#define PIT_CHANNEL2		0x80
/* rw */
#define PIT_RW_LOW		0x10
#define PIT_RW_HIGH		0x20
#define PIT_RW_HIGHLOW		0x30
/* mode */
#define PIT_MODE0		0x00
#define PIT_MODE1		0x02
#define PIT_MODE2		0x04
#define PIT_MODE3		0x06
/* bcd */
#define PIT_16BITBIN		0x00	/* 16 bitlik sayi seklinde verimizi gonderecegiz */
#define PIT_BCD			0x01
#define PIT_SET_BYTE		PIT_CHANNEL0 | PIT_RW_HIGH_LOW | PIT_MODE3 | PIT_16BITBIN			
#define GET_LOW_BYTE(x)		(x & 0xFF)
#define GET_HIGH_BYTE(x)	((x >> 8) & 0xFF)


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
	
	uint32_t divisor = PIT_CLOCK / hertz;
	/* set byte */
	outbyte(PIT_CNTRL, PIT_SET_BYTE);
	/* low byte */
	outbyte(PIT_CHANNEL0_DATA,GET_LOW_BYTE(divisor));
	/* high byte */
	outbyte(PIT_CHANNEL0_DATA,GET_HIGH_BYTE(divisor));
 
}

/*
 * init_timer
 */
void init_timer(void){
 
	debug_print(KERN_INFO,"Initializing the timer. Hz = \033[1;37m%u",PIT_HZ);
	irq_add_handler(TIMER_IRQ_NUM,timer_handler);
	/*
	 * 100 hz'e ayarla
	 * saniyede 100 cevrim
	 */
	timer_set_freq(PIT_HZ);
 
}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
