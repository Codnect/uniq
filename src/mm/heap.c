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

#define SYNCBLOCK_LINKABLE		0x1
#define SYNCBLOCK_HIGH			0x2
#define SYNCBLOCK_LOW			0x4
#define SYNCBLOCK_INSERT		0x8

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
void free(void *ptr){

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

static void sync_free_blk_list(heap_block_t *addr){


	/* debug_print(KERN_DUMP,"------------------------------------------------------");
	debug_print(KERN_DUMP,"entered -> sync_free_blk_list function..."); */
	
	heap_block_t *tmp_block = (void*)free_blk_list->first_block;
	heap_block_t *last_block = NULL;
	uint8_t flags = 0;	
	/* debug_print(KERN_DUMP,"tmp_addr : %p, addr : %p",tmp_block,addr); */

	while(tmp_block){
		
		if(tmp_block == (void*)addr - tmp_block->size - sizeof(heap_block_t)){
				flags |= SYNCBLOCK_LINKABLE;
				flags |= SYNCBLOCK_LOW;
				break;
		}
		
		if(tmp_block == (void*)addr + addr->size + sizeof(heap_block_t)){
				flags |= SYNCBLOCK_LINKABLE;
				flags |= SYNCBLOCK_HIGH;
				break;
		}

		if(tmp_block > addr){
				flags |= SYNCBLOCK_INSERT;
				break;		
		}	

		last_block = tmp_block;
		tmp_block = tmp_block->next_block;
		

	}
	
	/* eger tmp_block ve flags bossa liste sonuna */
	if(!tmp_block && !flags){
		/* debug_print(KERN_DUMP,"end of the list"); */
		last_block->next_block = addr;
		free_blk_list->blk_size += addr->size;
		addr->prev_block = last_block;
	}
	else{
		/* debug_print(KERN_DUMP,"entered -> SYNCBLOCK"); */
		
		if(flags & SYNCBLOCK_INSERT){
			/* debug_print(KERN_DUMP,"entered -> SYNCBLOCK_INSERT"); */
				
				heap_block_t *block = tmp_block;
				
				if(block->prev_block == free_blk_list){

					free_blk_list->first_block = addr;
					addr->prev_block = block->prev_block;
					block->prev_block = addr;
					addr->next_block = block;

				}else{

					block->prev_block = addr;
					addr->next_block =  block;
					block = last_block;
					block->next_block = addr;
					addr->prev_block = block;

				}

				free_blk_list->blk_size += addr->size;
			
		}
		else{
			/* debug_print(KERN_DUMP,"entered -> SYNCBLOCK_LINKABLE"); */
			
			if(flags & SYNCBLOCK_HIGH){

				/* debug_print(KERN_DUMP,"entered -> SYNCBLOCK_HIGH"); */
				uint32_t blk_size;
				heap_block_t *block = (heap_block_t*)tmp_block;
				blk_size = block->size + sizeof(heap_block_t);
				addr->prev_block = block->prev_block;
				addr->next_block = block->next_block;
				/* debug_print(KERN_DUMP,"block : %p , blk_size : %u byte",block,blk_size);
				debug_print(KERN_DUMP,"free_blk_size : %u byte",free_blk_list->blk_size); */
				free_blk_list->blk_size += (addr->size + sizeof(heap_block_t));
				addr->size += blk_size;
				/* debug_print(KERN_DUMP,"free_blk_size : %u byte",free_blk_list->blk_size);
				debug_print(KERN_DUMP,"addr size : %u byte",addr->size); */
				if(addr->prev_block == free_blk_list)
					free_blk_list->first_block = addr;
				else{
					((heap_block_t*)block->next_block)->prev_block = addr;
					((heap_block_t*)block->prev_block)->next_block = addr;
				}
				memset(block,0,sizeof(heap_block_t));

			}
			else if(flags & SYNCBLOCK_LOW){

				/* debug_print(KERN_DUMP,"entered -> SYNCBLOCK_LOW"); */
				uint32_t blk_size;
				heap_block_t *block = (heap_block_t*)tmp_block;
				blk_size = addr->size + sizeof(heap_block_t);
				/* debug_print(KERN_DUMP,"block : %p , blk_size : %u byte",block,blk_size);
				debug_print(KERN_DUMP,"free_blk_size : %u byte",free_blk_list->blk_size); */
				free_blk_list->blk_size += blk_size;
				/* debug_print(KERN_DUMP,"free_blk_size : %u byte",free_blk_list->blk_size); */
				block->size += blk_size;
				/* debug_print(KERN_DUMP,"blk_size : %u byte",block->size); */
				memset(addr,0,sizeof(heap_block_t));

			}
			
		}

	}
	
	
}


/*
 * link_free_block
 */
static void link_free_block(void *addr,uint32_t size){

	/*
	 * eger adres null ise bos blok listesine bos bir blogun baglanmasi
	 * gerektigini belirtir.
	 */
	/* debug_print(KERN_DUMP,"entered -> link_free_block function..."); */
	if(!addr){

		uint32_t query_size = size + sizeof(heap_block_t);
		uint32_t page_count = query_size / PAGE_SIZE;
		
		
		if(query_size % PAGE_SIZE)
			page_count++;
		
		/* debug_print(KERN_DUMP,"query size : %u byte ,page_count : %u",query_size,page_count); */
		heap_block_t *new_free_block = (heap_block_t*)sbrk(PAGE_SIZE * page_count);

		if(!new_free_block)
			die("(null) block!");	

		new_free_block->magic = KHEAP_MAGIC;
		new_free_block->size = PAGE_SIZE * page_count - sizeof(heap_block_t);
		new_free_block->prev_block = NULL;
		new_free_block->next_block = NULL;
		
		/* eger bos blok listesinde ilk block bossa */
		if(!free_blk_list->first_block){
			free_blk_list->first_block = (heap_block_t*)new_free_block;
			new_free_block->prev_block = (heap_block_t*)free_blk_list;
		}
		else
			sync_free_blk_list(new_free_block);

	}

}

/*
 * check_size_in_free_list
 */
static bool check_size_in_free_list(uint32_t size){

	heap_block_t *cntrl_block = free_blk_list->first_block;
	/* debug_print(KERN_DUMP,"enter -> check_size_in_free_list function..."); */	
	if(!cntrl_block)
		return false;

	while(cntrl_block){
		if(cntrl_block->size == size)
			return true;

		if(cntrl_block->size - size > sizeof(heap_block_t))	
			return true;

		cntrl_block = cntrl_block->next_block;

	}	
	return false;

} 

/*
 * unlink_free_block
 */
static void *unlink_free_block(void *addr,uint32_t size){

	heap_block_t *return_block = NULL;	
	/* debug_print(KERN_DUMP,"enter -> unlink_free_block function..."); */

	if(!addr){
		
		if(free_blk_list->blk_size < size || !check_size_in_free_list(size)){
			/* debug_print(KERN_DUMP,"free_blk_list->blk_size : %u byte, size : %u byte",free_blk_list->blk_size,
												  size); */
			link_free_block(NULL,size);
		}
		
		heap_block_t *block = free_blk_list->first_block;
			
		while(block){
			/* debug_print(KERN_DUMP,"block_size : %u byte",block->size); */
			if(block->size - size > sizeof(heap_block_t)){
				heap_block_t *move_block = (void*)block + sizeof(heap_block_t) + size;
			/*
				debug_print(KERN_DUMP,"------------------------------------------------------");
				debug_print(KERN_DUMP,"[enter 0]");
				debug_print(KERN_DUMP,"move_block : %p",move_block);
				debug_print(KERN_DUMP,"free_blk_list : %p, free first block : %p",free_blk_list,
												  free_blk_list->first_block);
				debug_print(KERN_DUMP,"free block size : %u byte",free_blk_list->blk_size);
			*/
				move_block->magic = KHEAP_MAGIC;
				move_block->prev_block = block->prev_block;
				move_block->next_block = block->next_block;
				move_block->size = block->size - sizeof(heap_block_t) - size;
				free_blk_list->blk_size -= sizeof(heap_block_t) + size;
			/*
				debug_print(KERN_DUMP,"------------------------------------------------------");
				debug_print(KERN_DUMP,"move_block : %p",move_block);
				debug_print(KERN_DUMP,"move_block->magic : %p",move_block->magic);
				debug_print(KERN_DUMP,"move_block->prev_block : %p",move_block->prev_block);
				debug_print(KERN_DUMP,"move_block->next_block : %p",move_block->next_block);
				debug_print(KERN_DUMP,"move_block->size : %u byte",move_block->size);
				debug_print(KERN_DUMP,"------------------------------------------------------");
			*/	
				heap_block_t *alloc_block = block;
				alloc_block->magic = KHEAP_MAGIC;
				alloc_block->prev_block = NULL;
				alloc_block->next_block = NULL;
				alloc_block->size = size;
			/*
				debug_print(KERN_DUMP,"alloc_block : %p",alloc_block);
				debug_print(KERN_DUMP,"alloc_block->magic : %p",alloc_block->magic);
				debug_print(KERN_DUMP,"alloc_block->prev_block : %p",alloc_block->prev_block);
				debug_print(KERN_DUMP,"alloc_block->next_block : %p",alloc_block->next_block);
				debug_print(KERN_DUMP,"alloc_block->size : %u byte",alloc_block->size);
			*/
				if(move_block->prev_block == free_blk_list){
					free_blk_list->first_block = move_block;
					((heap_block_t*)move_block->next_block)->prev_block = move_block;
				}
				else{
					((heap_block_t*)move_block->prev_block)->next_block = move_block;
					((heap_block_t*)move_block->next_block)->prev_block = move_block;
				}
			
			/*
				debug_print(KERN_DUMP,"------------------------------------------------------");
				debug_print(KERN_DUMP,"free_blk_list : %p, free first block : %p",free_blk_list,
												  free_blk_list->first_block);
				debug_print(KERN_DUMP,"free block size : %u byte",free_blk_list->blk_size);
			*/	
	
				return_block = block;
				break;
			}
			else if(block->size == size){
				/* debug_print(KERN_DUMP,"[enter 1]"); */
				if(block->prev_block == free_blk_list){
					free_blk_list->first_block = block->next_block;	
					/* eger blok dugumu varsa */
					if(block->next_block){
						/* debug_print(KERN_DUMP,"[enter 1.0]"); */
						((heap_block_t*)block->next_block)->prev_block = (heap_block_t*)free_blk_list;
					}
				}else{
					((heap_block_t*)block->prev_block)->next_block = block->next_block;
					((heap_block_t*)block->next_block)->prev_block = block->prev_block;
				}
				free_blk_list->blk_size -= block->size;
				block->prev_block = NULL;
				block->next_block = NULL;
				/*
					debug_print(KERN_DUMP,"------------------------------------------------------");
					debug_print(KERN_DUMP,"block : %p",block);
					debug_print(KERN_DUMP,"block->magic : %p",block->magic);
					debug_print(KERN_DUMP,"block->prev_block : %p",block->prev_block);
					debug_print(KERN_DUMP,"block->next_block : %p",block->next_block);
					debug_print(KERN_DUMP,"block->size : %u byte",block->size);
					debug_print(KERN_DUMP,"------------------------------------------------------");
					debug_print(KERN_DUMP,"free_blk_list : %p, free first block : %p",free_blk_list,
													  free_blk_list->first_block);
					debug_print(KERN_DUMP,"free block size : %u byte",free_blk_list->blk_size);
				*/
				return_block = block;
				break;
			}
			block = block->next_block;
		}
		
	}

	if(return_block){
		link_use_block(return_block);
		return (void*)return_block + sizeof(heap_block_t);
	}

	return (void*)NULL;

}


/*
 * __kmalloc
 *
 * @param size :
 */
void *__kmalloc(uint32_t size){

	/* boyut 0'sa gerek yok */
	if(!size)
		return (void*)NULL;

	return (void*)unlink_free_block(NULL,size);

}


/*
 * link_first_free_block
 */
static void link_first_free_block(void){

	/* bos blok tahsisi */
	void *first_alloc_addr = sbrk(PAGE_SIZE);
	heap_info.first_block_addr = first_alloc_addr;	/* fonksiyonlarda adres kontrolu icin */
	heap_block_t *first_heap_block = (heap_block_t*)first_alloc_addr;
	debug_print(KERN_DUMP,"first alloc addr : %p, first heap block addr %p",first_alloc_addr,first_heap_block);
	first_heap_block->magic = KHEAP_MAGIC;
	first_heap_block->size = PAGE_SIZE - sizeof(heap_block_t);
	first_heap_block->prev_block = (heap_block_t*)free_blk_list;
	first_heap_block->next_block = NULL;
	memset(first_heap_block + sizeof(heap_block_t),0,first_heap_block->size);

	/* bos blok listesine ilk bos blogu bagliyoruz */
	free_blk_list->first_block = first_heap_block;
	free_blk_list->blk_size += first_heap_block->size;
#if 0
	__first_free_block_test();
#endif
	 	
}

/*
 * __heap_test
 */
void __heap_test(void){

	debug_print(KERN_DUMP,"heap test...");

#if 0 /* test */ 
	debug_print(KERN_DUMP,"---> test (1) <---");
	uint32_t *x = (uint32_t*)__kmalloc(4078);
	debug_print(KERN_DUMP,"allocation(x) : %p",x);
	uint32_t *t = (uint32_t*)__kmalloc(20);
	debug_print(KERN_DUMP,"allocation(t) : %p",t);
	uint32_t *z = (uint32_t*)__kmalloc(6000);
	debug_print(KERN_DUMP,"allocation(z) : %p",z);
#endif 

#if 0 /* test */
	debug_print(KERN_DUMP,"---> test (1) <---");
	uint32_t *x = (uint32_t*)__kmalloc(4063);
	debug_print(KERN_DUMP,"allocation(x) : %p",x);
	debug_print(KERN_DUMP,"---> test (2) <---");
	uint32_t *y = (uint32_t*)__kmalloc(1);
	debug_print(KERN_DUMP,"allocation(z) : %p",y);
	debug_print(KERN_DUMP,"---> test (3) <---");
	uint32_t *z = (uint32_t*)__kmalloc(10);
	debug_print(KERN_DUMP,"allocation(z) : %p",z);
#endif

}

/*
 * heap_init
 */
void heap_init(void){
	
	debug_print(KERN_INFO,"Initializing the heap.");
	/* kullanilan ve bos heap blok listelerini ayarliyoruz */
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
#if 0
	__heap_test();
#endif

}
