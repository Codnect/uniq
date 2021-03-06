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
#include <uniq/module.h>

extern void time_init(void);

void kmain(mboot_info_t *mboot_info,uint32_t mboot_magic,uint32_t stack_ptr){

	
	/* vga konsol */
	init_vga_console();
	time_init();

	debug_print(KERN_INFO,"Stack pointer : \033[1;7m%P",stack_ptr);

	if(mboot_magic != MULTIBOOT_LOADER_MAGIC)
		debug_print(KERN_WARNING,"Invalid the magic number. The magic number is \033[1;31m%P",mboot_magic);
	else
		debug_print(KERN_INFO,"Valid the magic number.The magic number is \033[1;37m%P",mboot_magic);
		
	/*
	 * gdt,idt,isr ve irq
	 */
	gdt_init();	/* global tanimlayici tablosu */
	idt_init();	/* kesme tanimlayici tablosu */
	isr_init();	/* kesme servis istekleri */
	irq_init();	/* donanim kesme istekleri */
#if 0
	__int_test();
#endif
	timer_init();

	/*
	 * memory
	 */
	paging_init(mboot_info->mem_lower + mboot_info->mem_upper);
	paging_final();	
#if 0
	 __page_fault_test();
#endif
	heap_init();
	multitasking_init();

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
