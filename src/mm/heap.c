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
#include <uniq/spin_lock.h>
#include <string.h>

extern uintptr_t end;
uintptr_t last_addr = (uintptr_t)&end;
static volatile uint32_t mlock = 0;
heap_info_t heap_info;

/*
 * malloc
 *
 * @param size :
 */
__malloc void *malloc(uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr;
	spin_unlock(&mlock);
	return ret_addr;

}

/*
 * realloc
 *
 * @param ptr :
 * @param size :
 */
__malloc void *realloc(void *ptr,uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr;
	spin_unlock(&mlock);
	return ret_addr;
	
}

/*
 * calloc
 *
 * @param n :
 * @param size :
 */
__malloc void *calloc(uint32_t n,uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr;
	spin_unlock(&mlock);
	return ret_addr;
	
}

/*
 * valloc
 *
 * @param size :
 */
__malloc void *valloc(uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr;
	spin_unlock(&mlock);
	return ret_addr;
	
}

/*
 * free
 *
 * @param ptr :
 */
void free(void *ptr) {

}


/*
 * kmalloc_org
 *
 * @param size :
 * @param align :
 * @param physic_adddr :
 */
uint32_t kmalloc_orig(uint32_t size,bool align,uint32_t *physic_addr){

	if(align && (last_addr & ALIGN_LIMIT)){
		last_addr &= ALIGN_LIMIT;
		last_addr += FRAME_SIZE_BYTE;
	}

	if(physic_addr)
		*physic_addr = last_addr;

	uint32_t addr = last_addr;
	last_addr += size;

	return addr;

}


/*
 * kmalloc_align
 *
 * @param size :
 */
uint32_t kmalloc_align(uint32_t size){

	return kmalloc_orig(size,true,NULL);

}

/*
 * kmalloc_physic
 *
 * @param size :
 * @param physic_addr :
 */
uint32_t kmalloc_physic(uint32_t size,uint32_t *physic_addr){

	return kmalloc_orig(size,false,physic_addr);

}

/*
 * kmalloc_aphysic
 *
 * @param size :
 * @param physic_addr :
 */
uint32_t kmalloc_aphysic(uint32_t size,uint32_t *physic_addr){

	return kmalloc_orig(size,true,physic_addr);

}

/*
 * kmalloc
 *
 * @param size :
 */
uint32_t kmalloc(uint32_t size){

	return kmalloc_orig(size,false,NULL);

}

/*
 * __first_free_block_test
 */
void __first_free_block_test(void){
	
	debug_print(KERN_DUMP,"first free block test...");

	if(free_blk_list->first_block)
		debug_print(KERN_DUMP,"free block addr : %p",free_blk_list->first_block);
	else
		die("Not found the free block addr.");

	heap_block_t *tmp_block = free_blk_list->first_block;
	
	if(tmp_block->magic == KHEAP_MAGIC)
		debug_print(KERN_DUMP,"valid the heap magic number.");
	else
		die("invalid the heap magic number.");

	if(tmp_block->size)
		debug_print(KERN_DUMP,"block size : %u byte",tmp_block->size);
	else
		die("Not enough block size!");

	if((void*)tmp_block->prev_block == (void*)free_blk_list)
		debug_print(KERN_DUMP,"(free list header block!) prev_block : %p",tmp_block->prev_block);
	else
		die("(bad block!) prev_block : %p",tmp_block->prev_block);

	if(!tmp_block->next_block)
		debug_print(KERN_DUMP,"(null block!) next_block : %p",tmp_block->next_block); 


}

/*
 * link_first_free_block
 */
static void link_first_free_block(void){

	void *first_alloc_addr = sbrk(PAGE_SIZE);
	heap_block_t *first_heap_block = (heap_block_t*)first_alloc_addr;
	debug_print(KERN_DUMP,"first alloc addr : %p, first heap block addr %p",first_alloc_addr,first_heap_block);
	first_heap_block->magic = KHEAP_MAGIC;
	first_heap_block->size = PAGE_SIZE - sizeof(heap_block_t);
	first_heap_block->prev_block = (heap_block_t*)free_blk_list;
	first_heap_block->next_block = NULL;

	free_blk_list->first_block = first_heap_block;
	free_blk_list->blk_size += first_heap_block->size;
#if 0
	__first_free_block_test();
#endif
	 	
}


/*
 * heap_init
 */
void heap_init(void){
	
	debug_print(KERN_INFO,"Initializing the heap.");

	used_blk_list = (heap_block_header_t*)kmalloc(sizeof(heap_block_header_t));
	used_blk_list->magic = KHEAP_MAGIC;
	used_blk_list->blk_size = 0;
	used_blk_list->first_block = NULL;
	
	free_blk_list = (heap_block_header_t*)kmalloc(sizeof(heap_block_header_t));	
	free_blk_list->magic = KHEAP_MAGIC;
	free_blk_list->blk_size = 0;
	free_blk_list->first_block = NULL;
	debug_print(KERN_DUMP,"used_blk_list addr : %p, free_blk_list addr : %p",used_blk_list,free_blk_list);

	heap_info.current_end = (last_addr + FRAME_SIZE_BYTE) & ~0xFFF;
	debug_print(KERN_DUMP,"last_addr(end) : %p, heap current end addr : %p",last_addr,heap_info.current_end);
	link_first_free_block();

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
