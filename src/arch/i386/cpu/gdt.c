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

#include <uniq/types.h>

/*
 * -Genel bilgiler-
 * --------------------------------------------------------------------------------------------
 *
 * İşlemci ilk başlarken gerçek modda başlar. Gerçek modda 1 MiB'lik bellek
 * adresleyebilirken, korumali modda 32 bitlik yani 4 GiB'lik bellegi adresler.
 * Korumali modda bellek erisimi GDT,LDT tablolariyla gerceklesir.
 *
 * GDTR : Global Descriptor Table Register
 * -----  (GDT)-Global tanimlayici tablosuna isaret eder.
 *
 * LDTR : Local Descriptor Table Register
 * ----- LDTR, GDTR gibi direk tabloyu isaret etmez. GDT tablosundaki bir tanimlayiciya 
 *	 isaret eder. (LDT)-Yerel tanimlayici tablosunun adresi bu tanimlayici icindedir.
 *
 * IDTR : Interrupt Descriptor Table Register
 * -----  IDT tablosuna isaret eder ve kesmeleri yonetecek fonksiyonlarin adreslerini tutar.
 *
 * TR : Task Register
 * ---	TSS yapisina isaret eder. TSS yapisi surec ile ilgili tum bilgileri icerir.
 *
 * !Not : GDT ve LDT tablolari en fazla 8192 tanimlayici icerebilir.
 * -----
 *
 * CR0 : Control Register 0
 * ----	Korumalı mod ve sayfalama mekanizmasi icin
 *	CR0'in PG biti(31.bit) = 1 yapılırsa sayfalama aktif olur.
 *	CR0'in PE biti(1.bit)  = 1 yapılırsa gercek moddan,korumali mod'a gecilir
 *
 * CR1 : Control Register 1
 * ----	intel tarafinda rezerve edilmistir.
 *
 * CR2 : Control Register 2
 * ----	sayfalama hatasi olustugunda olusan sayfa adresini tutar.
 * 	!dikkat : sayfalama aktifse.
 *
 * CR3 : Control Register 3
 *	sayfa dizin tablosunun fiziksel adresini tutar.
 *
 * CR4 : Control Register 4
 * ----	intel icin.
 *
 * --------------------------------------------------------------------------------------------
 *
 */
 
struct gdt_entry{
	u16 limit_low;
	u16 base_low;
	u8 base_middle;
	u8 access;
	u8 granularity;
	u8 base_high;
} __attribute__((packed));


struct gdt_ptr {
	u16 limit;
	u32 base;
} __attribute__((packed));
