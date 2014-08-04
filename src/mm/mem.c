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
#include <uniq/kernel.h>
#include <mm/mem.h>
#include <mm/heap.h>
#include <uniq/task.h>
#include <string.h>


uint32_t *frame_map;
uint32_t nframe;
extern uint32_t last_addr;

page_dir_t *kernel_dir = 0;
page_dir_t *current_dir = 0;

/*
 * enable_paging
 */
void enable_paging(void){

	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	__asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

}

/*
 * disable_paging
 */
void disable_paging(void){

	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 &= ~0x80000000;
	__asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
	
}


/*
 * set_frame
 *
 * @param frame_addr :
 */
void set_frame(uintptr_t frame_addr){

	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);
	frame_map[index] |= (0x1 << offset); 

}

/*
 * remove_frame
 *
 * @param frame_addr :
 */
void remove_frame(uintptr_t frame_addr){
	
	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);
	frame_map[index] &= ~(0x1 << offset);

}

/*
 * cntrl_frame
 *
 * @param frame_addr :
 */
uint32_t cntrl_frame(uintptr_t frame_addr){
	
	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);
	
	return (frame_map[index] & (0x1 << offset));

}

/*
 * find_free_frame
 */
uint32_t find_free_frame(void){

	uint32_t index,offset;
	
	for(index = 0; index < (nframe/32); index++){

		if(frame_map[index] != MAX_LIMIT){

			for(offset = 0; offset < 32; offset++){
				uint32_t cntrl = 0x1 << offset;
				if(!(frame_map[index] & cntrl))
					return index * 32 + offset;	

			}

		}
	}

}

/*
 * alloc_frame
 *
 * @param page :
 * @param rw :
 * @param user :
 */
void alloc_frame(page_t *page,bool rw,bool user){

	if(page->frame)
		return;

	uint32_t index = find_free_frame();

	if(index == MAX_LIMIT)
		printf("no free frames!");
	
	set_frame(index * FRAME_SIZE_BYTE);

	page->present = PAGE_PRESENT;
	page->rw      = (rw) ? PAGE_RWRITE : PAGE_RONLY;
	page->user    = (user) ? PAGE_USER_ACCESS : PAGE_KERNEL_ACCESS;
	page->frame   = index;
}

/*
 * free_frame
 *
 * @param page :
 */
void free_frame(page_t *page){

	uint32_t frame;

	if(!(frame = page->frame))
		return;

	remove_frame(frame);
	page->frame = 0x0;
}

/*
 * page_fault
 * 
 * @param regs :
 */
void page_fault(struct registers_t *regs){

	uint32_t fault_addr;
	__asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
	
	uint32_t present = !(regs->err_code & 0x1);
	
	printf("page fault !");
	if(present)
		printf(" (present) ");
	halt_system();
 
}

/*
 * get_page
 *
 * @param addr :
 * @param make :
 * @param dir :
 */
page_t *get_page(uint32_t addr,bool make,page_dir_t *dir){

	addr /= FRAME_SIZE_BYTE;
	uint32_t table_index = addr / 1024;

	if(dir->tables[table_index])
		return &dir->tables[table_index]->pages[addr % 1024];
	else if(make){

		uint32_t temp;
		dir->tables[table_index] = (page_table_t *)kmalloc_aphysic(sizeof(page_table_t),&temp);
		memset(dir->tables[table_index],0,FRAME_SIZE_BYTE);
		dir->tables_physic[table_index] = temp | 0x7;	/* present,rw,us */
		return &dir->tables[table_index]->pages[addr % 1024];
	}
	else
		return NULL;

}

/*
 * change_page_dir
 *
 * @param new_dir :
 */
void change_page_dir(page_dir_t *new_dir){

	current_dir = new_dir;
	__asm__ volatile("mov %0, %%cr3" :: "r"(&new_dir->tables_physic));
	uint32_t cr0;
   	__asm__ volatile("mov %%cr0, %0": "=r"(cr0));
   	cr0 |= 0x80000000; // Enable paging!
   	__asm__ volatile("mov %0, %%cr0":: "r"(cr0));

}

/*
 * paging_init
 *
 * @param mem_size :
 */
void paging_init(uint32_t mem_size){
	
	nframe = mem_size / FRAME_SIZE_KIB;
	frame_map = (uint32_t *)kmalloc(nframe / 8);
 	memset(frame_map,0,nframe / 8);
	
	kernel_dir = (page_dir_t *)kmalloc_align(sizeof(page_dir_t));
	memset(kernel_dir,0,sizeof(page_dir_t));
	current_dir = kernel_dir;

	uint32_t i = 0;
	while(i < last_addr){
		alloc_frame(get_page(i,1,kernel_dir),0,0);
		i += 0x1000;
	}

	isr_add_handler(14,page_fault);
	change_page_dir(kernel_dir);

}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
