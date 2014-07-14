/*
 *  Copyright(C) 2014 Codnect Team
 *  Copyright(C) 2014 Burak Köken
 *
 *  This file is part of Nectlib.
 *  
 *  Nectlib is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2 of the License.
 *
 *  Nectlib is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Uniq.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <uniq/types.h>

void kmain(uintptr_t mboot_magic){


}

/*
 * ----------------------------------------------------
 * print fonksiyonlari, vga fonksiyonlarini kullanarak
 * daha basit bir kullanim icin hazirlanmistir.
 * (vga konsoluyla direk bir islem yapmak gerekmiyorsa
 * asagidaki fonksiyonlarin kullanimi onerilir)
 * ----------------------------------------------------
 */

/*
 * ekrana bir karakter dizisi yazdirmak icin.
 * 
 * @param s : karakter dizisi
 */
int print(const char *s){

	int i =	putstr(s,DEFAULT_ATTR);
	return i;
}

/*
 * ekrana bir karakter yazdirmak icin.
 * 
 * @param c :karakter
 */
void print_char(const char c){
	putchar(c,DEFAULT_ATTR);
}

/*
 * ekrana renkli bir cikti vermek icin kullanilir.
 *
 * @param s : karakter dizisi
 * @param attr : karakter ozelligi
 */
int print_color(const char *s,uint8_t attr){

	int i = putstr(s,attr);
	return i;
}

/*
 * ekrana bir karakter dizisi yazdiktan sonra alt
 * satira gider.
 * 
 * @param s : karakter dizisi
 */
int println(const char *s){

	int i = print(s);
	print_char('\n');
	i++;

	return i;
}
