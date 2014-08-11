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
#include <uniq/kernel.h>
#include <uniq/multiboot.h>

void kmain(struct mboot_t *mboot,uintptr_t mboot_magic){

	/* vga konsol */
	init_vga_console();

	if(mboot_magic != 0x2badb002)
		debug_print(KERN_WARNING,"Invalid the magic number. The magic number is \033[1;31m%P",mboot_magic);
	else
		debug_print(KERN_INFO,"Valid the magic number.The magic number is \033[1;37m%P",mboot_magic);
		
	/*
	 * gdt,idt,isr ve irq
	 */
	gdt_init();
	idt_init();
	isr_init();
	irq_init();
#if 0
	__int_test();
#endif
	/*
	 * drivers
	 */
	timer_init();

	/*
	 * memory
	 */
	 paging_init(mboot->mem_lower+mboot->mem_upper);
#if 0
	 __page_fault_test();
#endif

}
