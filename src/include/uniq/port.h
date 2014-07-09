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

#ifndef __UNIQ_IOPORT_H__
#define __UNIQ_IOPORT_H__

#include <uniq/types.h>

typedef unsigned short		port_t;

/*
 * inline assembly
 *
 *  asm ( assembly kodu
 *	 : cikis listesi			 	(istege bagli)
 *	 : giris listesi			 	(istege bagli)
 *	 : geri donen kaydedicilerin listesi 	  	(istege bagli)
 *	 );
 *
 *  ------------
 *  | a | %eax |
 *  | b | %ebx |
 *  | c | %ecx |
 *  | d | %edx |
 *  | S | %esi |
 *  | D | %edi |
 *  ------------
 */


/*
 * temel i/o islemleri
 */
 
 
/*
 * port'a bir byte yaz
 *
 * @param port : veri yazilacak port
 * @param data : yazilacak veri
 */
static inline void outb(port_t port,uint8_t data){
	__asm__ volatile("outb %1,%0"
			 :				/* cikis yok */
			 : "dN"(port),"a"(data)		/* girisler  */
			 );
}

/*
 * porttan bir byte oku
 *
 * @param port : veri okunacak port
 */
static inline uint8_t inb(port_t port){
	uint8_t ret_val;
	__asm__ volatile("inb %1,%0"
			 : "=a"(ret_val)		/* cikislar */
			 : "dN"(port)			/* girisler */
			 ); 
	return ret_val;		 
}

/*
 * port'a bir word yaz
 *
 * @param port : veri yazilacak port
 * @param data : yazilacak veri
 */
static inline void outw(port_t port,uint16_t data){
	__asm__ volatile("outw %1,%0"
			 :				/* cikis yok */
			 : "dN"(port),"a"(data)		/* girisler  */
			 );

}

/*
 * porttan bir word oku
 *
 * @param port : veri okunacak port
 */
static inline uint16_t inw(port_t port){
	uint16_t ret_val;
	__asm__ volatile("inw %1,%0"
			 : "=a"(ret_val)		/* cikislar */
			 : "dN"(port)			/* girisler */
			 );	
	return ret_val;
}

#endif /* __UNIQ_IOPORT_H__ */
