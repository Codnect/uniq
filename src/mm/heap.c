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
 * heap_init
 */
void heap_init(void){
	
	debug_print(KERN_INFO,"Initializing the heap.");
	heap_info.current_end = (last_addr + FRAME_SIZE_BYTE) & ~0xFFF;
	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p",last_addr);
	debug_print(KERN_DUMP,"heap current end addr : \033[1;37m%p",heap_info.current_end); 
 
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
