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

#ifndef __UNIQ_INLINE_ASM_H__
#define __UNIQ_INLINE_ASM_H__

/* kesmeleri devre disi birak */
static inline void cli(){
	__asm__ volatile("cli");
}

/* kesmeleri aktif hale getir */
static inline void sti(){
	__asm__ volatile("sti");
}

/* sistemi durdur */
static inline void hlt(){
	__asm__ volatile("hlt");
}

/* islem yapmayi birak */
static inline void relax_cpu(){
	__asm__ volatile("rep; nop");
}

#define disable_interrupts()		cli()
#define enable_interrupts()		sti()
#define halt_system()			hlt()


#endif /* __UNIQ_INLINE_ASM_H__ */
