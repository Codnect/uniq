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

/*
 * -Genel bilgiler-
 * --------------------------------------------------------------------------------------------
 *
 * IDTR : Interrupt Descriptor Table Register
 * -----  IDT tablosuna isaret eder ve kesmeleri yonetecek fonksiyonlarin adreslerini tutar.
 *
 * IDTR yazmaci asagidaki gibidir.
 *
 * 47								16 15			     0	
 * ===========================================================================================
 * =                         taban adres 			 =	   limit	     =
 * ===========================================================================================
 *
 * --------------------------------------------------------------------------------------------
 *
 * -Kesme ve istisnalarin yonetimi-
 *  ...
 *
 */

struct idt_entry_t{
	uint16_t base_low;	/* atlanicak fonksiyonunun taban adresinin ilk 16 biti */
	uint16_t sel;		/* kernel segment selektor */
	uint8_t zero;		/* her zaman sifir */
	uint8_t flags;		/* flaglar */
	uint16_t base_high;	/* atlanicak fonksiyonun taban adresini son 16 biti */
} __attribute__((packed));


struct idt_ptr_t{
	uint16_t limit;		/* idt'nin uzunlugu(bayt olarak) */
	uint32_t base;		/* idt'nin taban adresi */
} __attribute__((packed));


/*
 * idt_set_gate
 *
 * @param num :
 * @param base :
 * @param sel :
 * @param flags :
 */
void idt_set_gate(uint8_t num,void (*base)(void),uint16_t sel,uint8_t flags){
	
	
}



/*
 * init_idt
 */
void init_idt(void){
 

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
