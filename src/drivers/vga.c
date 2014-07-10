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
 

#include <drivers/vga.h>
#include <uniq/port.h>
#include <uniq/types.h>

/*
 * grub onyukleyicisi kerneli 80x25 text modunda baslatir.
 * bu text modundaki karakter formati
 *
 * 15       12 11          8 7                       0
 * ====================================================
 * = Arkaplan =   Karakter  =      Karakter Kodu      =
 * =   Rengi  =     Rengi   =	      (ASCII)         =
 * ====================================================
 */



#define VGA_VRAM_START		0xB8000
#define VGA_VRAM_END		0xC0000
#define VGA_CWIDTH		80
#define VGA_CHEIGHT		25
#define VGA_CSIZE		(VGA_CWIDTH * VGA_CHEIGHT)
#define VGA_IPORT		0x3D4
#define VGA_DPORT		0x3D5
#define VGA_CLHIGH		14
#define VGA_CLLOW		15

#define TAB_SIZE		8
#define BLANK			0x20



static uint16_t * vga_vram = null;
static uint8_t csr_x,csr_y;
static uint16_t csr_loc;
static uint8_t save_x,save_y,attr_c;



static void refresh_csr(void){
	csr_x = csr_loc % VGA_CWIDTH;
	csr_y = csr_loc / VGA_CWIDTH;
}

static void restore_csr(void){
	csr_x = save_x;
	csr_y = save_y;
	goto_xy(csr_x,csr_y);
}

static void save_csr(void){

	refresh_csr();
	save_x = csr_x;
	save_y = csr_y;

}

static void scrollup(){

	if(csr_loc < VGA_CSIZE-VGA_CWIDTH)
		return;

	for(int i=0;i<VGA_CSIZE-VGA_CWIDTH;i++)
		vga_vram[i] = vga_vram[i+VGA_CWIDTH];

	del_line(VGA_CHEIGHT-1);
	csr_loc -= VGA_CWIDTH;
}

void del_line(int line_no){
	
	if(line_no < 0 || line_no > (VGA_CHEIGHT-1))
		return;

	save_csr();
	csr_x = 0;
	csr_y = line_no;
	goto_xy(csr_x,csr_y);

	for(int i=csr_x;i<VGA_CWIDTH;i++)
		vga_vram[csr_loc +i] = make_vga_entry(BLANK,D_ATTR);

	restore_csr();
 	
}

static void move_csr(void){

	outb(VGA_IPORT,VGA_CLHIGH);
	outb(VGA_DPORT,csr_loc >> 8);

	outb(VGA_IPORT,VGA_CLLOW);
	outb(VGA_DPORT,(csr_loc & 0xFF));
	
	refresh_csr();
}

void goto_xy(uint8_t new_x,uint8_t new_y){
	
	if(new_x > (VGA_CWIDTH-1) || (VGA_CHEIGHT-1) < new_y)
		return;

	csr_loc = new_y * VGA_CWIDTH + new_x;
	move_csr();

}

void putchar(char c, uint8_t attr){

	switch(c){
		/* bir alt satir */
		case '\n':

			while(csr_loc % VGA_CWIDTH)
				vga_vram[csr_loc++] = make_vga_entry(BLANK,D_ATTR);
			break;
		/* imleci sola kaydir(backspace) */
		case '\b':
			vga_vram[csr_loc--] = make_vga_entry(BLANK,D_ATTR);
			break;
		/* satir basi */
		case '\r':
			goto_xy(0,csr_y);
			break;
		/* yatay tab */
		case '\t':
			for(int i=0;i<8;i++)
				vga_vram[csr_loc++] = make_vga_entry(BLANK,D_ATTR);
			break;
		/* dikey tab */
		case '\v':
			putstr("\n\t",D_ATTR);
			break;
		/* cift tirnak */
		case '\"':
			vga_vram[csr_loc++] = make_vga_entry(0x22,attr);
			break;
		/* tek tirnak */
		case '\'':
			vga_vram[csr_loc++] = make_vga_entry(0x27,attr);
			break;
		case '\\':
			vga_vram[csr_loc++] = make_vga_entry(0x5C,attr);
			break;
		default:
			vga_vram[csr_loc++] = make_vga_entry(c,attr);
			break;
			
	}

	scrollup();
	move_csr();
}

void putstr(char *string,uint8_t attr){

	int i = 0;
	while(string[i])
		putchar(string[i++],attr);

}

void reset_console(void){
	
	csr_x = csr_y = csr_loc = 0;
	
	for(int i=0;i<VGA_CHEIGHT;i++)
		del_line(i);
	move_csr();
	
}

void init_vga_console(void){

	vga_vram = (uint16_t*)(VGA_VRAM_START);
	reset_console();
	
}
