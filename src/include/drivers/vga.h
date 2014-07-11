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
 
#ifndef __UNIQ_VGA_H__
#define __UNIQ_VGA_H__

#include <uniq/types.h>

/*
 * vga 80x25 modu renkleri
 */
typedef enum{
	COLOR_BLACK = 0x0,
	COLOR_BLUE = 0x1,
	COLOR_GREEN = 0x2,
	COLOR_CYAN = 0x3,
	COLOR_RED = 0x4,
	COLOR_MAGENTA = 0x5,
	COLOR_BROWN = 0x6,
	COLOR_LIGHT_GREY = 0x7,
	COLOR_DARK_GREY = 0x8,
	COLOR_LIGHT_BLUE = 0x9,
	COLOR_LIGHT_GREEN = 0xA,
	COLOR_LIGHT_CYAN = 0xB,
	COLOR_LIGHT_RED = 0xC,
	COLOR_LIGHT_MAGENTA = 0xD,
	COLOR_LIGHT_BROWN = 0xE,
	COLOR_WHITE = 0xF
}vga_color;


/*
 * make_vga_entry , vga girisi icin karakteri hazirlar.
 * 
 * @param c : karakter
 * @param attr : karakter rengi
 */
static inline uint16_t make_vga_entry(char c,uint8_t attr){
	return ( attr << 8 ) | c;
} 

/*
 * make_vga_color, vga rengi hazirlar.
 *
 * @param fcolor : karakter rengi
 * @param bcolor : arkaplan rengi
 */
static inline uint8_t make_vga_color(uint8_t fcolor,uint8_t bcolor){
	return ( bcolor << 4 ) | ( fcolor & 0x0F); 
}

/* varsayilan karakter ozelligi */
#define DEFAULT_ATTR	make_vga_color(COLOR_LIGHT_GREY,COLOR_BLACK)

void goto_xy(uint8_t new_x,uint8_t new_y);
void goto_save_csr(void);
void init_vga_console(void);
void exit_vga_console(void);
void reset_console(void);
void putstr(const char *string,uint8_t attr);
void putchar(const char c, uint8_t attr);
void delete_line(uint8_t line_no);
void change_line(uint8_t line_no,const char *s);


#endif /* __UNIQ_VGA_H__ */
