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
 * blk_part_push,
 *
 * @param header :
 * @param ptr :
 */
static void *blk_part_push(heap_blk_header_t *header,void *ptr){

	uint32_t **addr = (uint32_t**)ptr;
	*addr = (uint32_t*)header->point;
	header->point = addr;

}

/*
 * blk_part_pop,
 *
 * @param header :
 */
static void *blk_part_pop(heap_blk_header_t *header){

	assert(header && header->point != NULL);	

	void *addr = header->point;
	uint32_t **point = header->point;
	header->point = *point;
	
	return addr;

}

/*
 * __kmalloc, heap alanindan istenen boyutta bellek ayrilir.
 *
 * @param size : heap alanindan ayrilacak boyut(bayt olarak) 
 */
static void *_kmalloc(uint32_t size){

	/*
	 * eger boyut sifirsa islem yapmaya gerek yok.
	 */
	if(!size)
		return NULL;

	uint32_t blk_type = detect_heap_block_size(size);

	if(blk_type >= BIG_BLOCK){
		
		/*
		 * big block
		 */
		#if 0
			debug_print(KERN_DUMP,"-> big block",blk_type);
		#endif
	}
	else{

		/*
		 * small block
		 */
		heap_blk_header_t *small_blk = get_heap_blk_header(&heap_small_blks[blk_type]);
		#if 0
			debug_print(KERN_DUMP,"-> small block, [block type = %u] ,small_blk : %p",blk_type,small_blk);
		#endif

		if(!small_blk){
				
				/*
				 * sbrk ile sayfa boyutu kadar tahsis islemi gerceklestiriyoruz.
				 */
				small_blk = (heap_blk_header_t*)sbrk(PAGE_SIZE);
				assert(!((uint32_t)small_blk % PAGE_SIZE));
				small_blk->magic = BLOCK_MAGIC;
				/*
				 * sayfanin hangi adresinden itibaren malloc fonksiyonlariyla
				 * tahsis islemlerinin yapilacagi adres noktasi belirliyoruz.
				 */
				small_blk->point = (void*)((uint32_t)small_blk + sizeof(heap_blk_header_t));
				#if 0				
					debug_print(KERN_DUMP,"small_blk : %p, small_blk->head : %p",small_blk,small_blk->head);
				#endif
				heap_blk_t *blk = (heap_blk_t*)(&heap_small_blks[blk_type]);
				/*
				 * sayfayi bulundugu small block tipine gore listeye bagliyoruz.
				 */
				small_blk->next = blk->first;
				blk->first = small_blk;

				/*
				 * sayfayi belirlenen blok boyutuna gore ayarlayiyoruz.
				 */
				#define calc_blk_parts(x)	((PAGE_SIZE - sizeof(heap_blk_header_t)) >> x) - 1
				uint32_t true_pow = blk_type + 2;	 /* (2^0 ve 2^1) */
				uint32_t blk_parts = calc_blk_parts(true_pow);
				#undef calc_blk_parts
				#if 0				
					debug_print(KERN_DUMP,"true_pow : %u, blk_parts : %u",true_pow,blk_parts);
				#endif				
				uint32_t **tmp_blk = small_blk->point;

				/*
				 * sayfadaki blok parcalarinin malloc ile tahsis edilen kadar
				 * birbirlerinin adreslerini tutmasini sagliyoruz.
				 */
				for(uint32_t i = 0; i < blk_parts; i++){
					#if 0
						debug_print(KERN_DUMP,"i << blk_type : %u (i+1) << blk_type : %u",i << blk_type,
											    		(i+1) << blk_type);
						debug_print(KERN_DUMP,"tmp_blk[i << blk_type] : %p",&tmp_blk[i << blk_type]);
					#endif
					tmp_blk[i << blk_type] = (uint32_t*)&tmp_blk[(i+1) << blk_type];
				
				}
				#if 0
					debug_print(KERN_DUMP,"&tmp_blk[blk_parts << blk_type] : %p",&tmp_blk[blk_parts << blk_type]);
				#endif 				
				tmp_blk[blk_parts << blk_type] = NULL;
				small_blk->size = blk_type;

		}
		
		/*
		 * istenilen small block tipinden bir parca istiyoruz.
		 */
		uint32_t **ret = blk_part_pop(small_blk);

		/*
		 * eger istenilen boyutta blok parcasi ayrildiktan sonra
		 * tahsis edilecek adres kalmamissa,bir sonraki dugumu
		 * tahsis isleminin yapildigi ilk blok dugumune atiyoruz.
		 * eger bu NULL ise bir dahaki tahsis isleminde ayni
	 	 * small block tipinden tahsis yapilmaya kalkilirsa usteki
		 * gordugumuz kontrol yapisindan iceri girilir ve tahsis icin 
		 * ayarlamalar yapilir.
		 */
		if(!small_blk->point){

			heap_blk_t *blk = (heap_blk_t*)(&heap_small_blks[blk_type]);
			blk->first = small_blk->next;
			small_blk->next = NULL;

		}

		/*
		 * ve adresi geri donduruyoruz, :) mutlu son.
		 */
		return ret;
		
	}

	return (void*)NULL;

}


/*
 * __kfree
 *
 * @param ptr :
 */
void __kfree(void *ptr){

	/*
	 * eger bos bir isaretci ise islemlere gerek yok
	 */
	if(!ptr)
		return;

	/*
	 * blk_header'larin adreslerinin hepsinin bir sayfa boyutunun katlarindan
	 * basladigini _kmalloc() fonksiyonun yapisini inceleyerek gorebilirsiniz.
	 */
	heap_blk_header_t *blk_header = (heap_blk_header_t*)((uint32_t)ptr & ~PAGE_MASK);

	/*
	 * eger sihirli numara eslemiyorsa!
	 */
	if(blk_header->magic != BLOCK_MAGIC)
		return;

	uint32_t blk_type = blk_header->size;

	if(blk_type >= BIG_BLOCK){

		/*
		 * big block
		 */
	}
	else{

		/*
		 * small block
		 */
		
		if(!blk_header->point){

			heap_blk_t *blk = (heap_blk_t*)(&heap_small_blks[blk_type]);
			blk_header->next = blk->first;
			blk->first = blk_header;	
	
		}
		
		/*
		 * blok parcasinin tekrar kullanilmasi amaciyla
		 * bosa cikariyoruz.
		 */
		blk_part_push(blk_header,ptr);
		
	}


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
