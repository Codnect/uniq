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

#define BLOCK_MAGIC		0xBAF01CDE
#define BIG_BLOCK		10
#define SMALL_BLOCK		(BIG_BLOCK - 1)
#define PAGE_MASK		0xfff
#define BITS_PER_BYTE		8

#define find_bit_count(x){ \
	sizeof(uint32_t) * BITS_PER_BYTE \
	- __builtin_clzl(x)  \
	}

#define is_pow_two(x)		!(x & (x - 1))


void *__kmalloc(uint32_t size);



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

	spin_lock(&mlock);
	/* */
	spin_unlock(&mlock);

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
 * detect_heap_block_size,tahsis icin en uygun boyutu belirler.
 *
 * @param size : tahsis edilmek istenen boyut (bayt olarak). 
 */
static uint32_t detect_heap_block_size(uint32_t size){

	/*
	 * boyutun kac bitle ifade edildigi buluyoruz.ornegin 4
	 * sayisi ikili sistemde "100" dir. yani toplamda 3 bit
	 * ile ifade edilir.
	 */
	uint32_t block_size = find_bit_count(size);
	
	if(block_size <= 2)
		return 0;

	block_size -= (is_pow_two(size) + 2);

	if(block_size > BIG_BLOCK)
		return BIG_BLOCK;

	return block_size;

}

static heap_blk_t heap_small_blks[SMALL_BLOCK + 1];

/*
 * get_heap_blk_header,
 *
 * @param blk : 
 */
static heap_blk_header_t *get_heap_blk_header(heap_blk_t *blk){

	return blk->first;

}

/*
 * blk_list_update,
 *
 * @param blk :
 * @param node :
 */ 
static void blk_list_update(heap_blk_t *blk,heap_blk_header_t *node){
	
	node->next = blk->first;
	blk->first = node;

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
		
	uint32_t blk_type = detect_heap_block_size(size);

	if(blk_type >= BIG_BLOCK){
		
		/*
		 * big block
		 */
		debug_print(KERN_DUMP,"-> big block",blk_type);
	}
	else{

		/*
		 * small block
		 */
		debug_print(KERN_DUMP,"-> small block, [block type = %u]",blk_type);
		heap_blk_header_t *small_blk = get_heap_blk_header(&heap_small_blks[blk_type]);
		debug_print(KERN_DUMP,"small_blk : %p",small_blk);

		if(!small_blk){

				/*
				 * sayfa tahsisi
				 */
				small_blk = (heap_blk_header_t*)sbrk(PAGE_SIZE);
				assert(!((uint32_t)small_blk % PAGE_SIZE));
				debug_print(KERN_DUMP,"small_blk : %p",small_blk);
				small_blk->magic = BLOCK_MAGIC;

				/*
				 * bloktan tahsis icin adresi belirleyelim.
				 */
				small_blk->head = (void*)((uint32_t)small_blk + sizeof(heap_blk_header_t));
				debug_print(KERN_DUMP,"small_blk->head : %p",small_blk->head);

				/*
				 * small block list'i guncelleyelim.
				 */
				blk_list_update(&heap_small_blks[blk_type],small_blk);

				/*
				 * sayfayi belirlenen block boyutuna gore ayarlayalim.
				 */
				#define calc_blk_parts(x)	((PAGE_SIZE - sizeof(heap_blk_header_t)) >> x) - 1
				uint32_t true_pow = blk_type + 2; /* (2^0 ve 2^1) */
				uint32_t blk_parts = calc_blk_parts(true_pow);
				#undef calc_blk_parts
				debug_print(KERN_DUMP,"true_pow : %u, blk_parts : %u",true_pow,
										      blk_parts);

				for(uint32_t i = 0; i < blk_parts; i++){

				}
 
		}
		else{

		}
	}

	return (void*)NULL;

}

/*
 * __kfree
 *
 * @param ptr :
 */
void __kfree(void *ptr){



}



/*
 * __heap_test
 */
void __heap_test(void){


}

/*
 * heap_init
 */
void heap_init(void){
	
	debug_print(KERN_INFO,"Initializing the heap.");
	heap_info.current_end = (last_addr + FRAME_SIZE_BYTE) & ~PAGE_MASK;


}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
