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


static void _kfree(void *ptr);
static void *_kcalloc(uint32_t nmem,uint32_t size);
static void *_kvalloc(uint32_t size);
static void *_krealloc(void *ptr,uint32_t size);
static void *_kmalloc(uint32_t size);



/*
 * malloc,istenilen boyutta yer tahsis eder.
 *
 * @param size : boyut (bayt olarak)
 */
__malloc void *malloc(uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr = _kmalloc(size);
	spin_unlock(&mlock);
	
	return ret_addr;

}

/*
 * realloc,eski tahsis edilen bolgenin boyutunu
 * degistirir.
 *
 * @param ptr : isaretci
 * @param size : boyut (bayt olarak)
 */
__malloc void *realloc(void *ptr,uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr = _krealloc(ptr,size);
	spin_unlock(&mlock);
	
	return ret_addr;
	
}

/*
 * calloc, istenilen eleman sayisi kadar verilen
 * eleman boyutuna gore yer tahsis edilir.
 *
 * @param n : eleman sayisi
 * @param size : eleman boyutu
 */
__malloc void *calloc(uint32_t n,uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr = _kcalloc(n,size);
	spin_unlock(&mlock);
	
	return ret_addr;
	
}

/*
 * valloc,sayfa hizalamasi yaparak bellek tahsisi yapar
 * zorunlu olmadigi surece kullanmamaya dikkat edin!
 * 
 * @param size : boyut (bayt olarak)
 */
__malloc void *valloc(uint32_t size){
	
	spin_lock(&mlock);
	void *ret_addr = _kvalloc(size);
	spin_unlock(&mlock);
	
	return ret_addr;
	
}

/*
 * free,belirtilen bellek bolgesini bos'a cikarir.
 *
 * @param ptr : isaretci
 */
void free(void *ptr){

	spin_lock(&mlock);
	
	if(last_addr < (uint32_t)ptr)
		_kfree(ptr);

	spin_unlock(&mlock);

}


/*
 * kmalloc_org, kmalloc ismiyle baslayan fonksiyonlarin ortak
 * cagirdigi bir alt fonksiyondur. cagiran fonksiyonlarin
 * verdigi parametreler dogrultusunda bellek tahsisi yapar.
 *
 * @param size : boyut
 * @param align : sayfa hizalamasi yapilsin mi?
 * @param physic_adddr : fiziksel adresin atilacagi adres.
 */
uint32_t kmalloc_orig(uint32_t size,bool align,uint32_t *physic_addr){

	/* eger heap baslatilmissa */
	if(heap_info.current_end){

		void *ret_addr;
		
		if(!align)
			ret_addr = malloc(size);
		else
			ret_addr = valloc(size);

		return ret_addr;
		
	}

	/* sayfa hizalamasi yapar,eger align 'true' ise  */
	if(align && (last_addr & 0xFFFFF000)){

		last_addr &= ~PAGE_MASK;
		last_addr += FRAME_SIZE_BYTE;

	}

	if(physic_addr)
		*physic_addr = last_addr;

	uint32_t addr = last_addr;
	last_addr += size;

	return addr;

}


/*
 * kmalloc_align,sayfa hizalamasi yaparak bellek tahsis eder.
 *
 * @param size : boyut(bayt olarak)
 */
uint32_t kmalloc_align(uint32_t size){

	return kmalloc_orig(size,true,NULL);

}

/*
 * kmalloc_physic,sayfa hizalamasi yapmadan bellek tahsisi
 * yapar ve verilen adrese de tahsis edilen bellek bolgesinin 
 * adresini atar.
 *
 * @param size : boyut(bayt olarak)
 * @param physic_addr : fiziksel adresin atilacagi adres.
 */
uint32_t kmalloc_physic(uint32_t size,uint32_t *physic_addr){

	return kmalloc_orig(size,false,physic_addr);

}

/*
 * kmalloc_aphysic,sayfa hizalamasi yaparak bellek tahsisi yapar
 * ve verien adrese de tahsis edilen bellek bolgesinin adresini
 * atar.
 *
 * @param size : boyut(bayt olarak)
 * @param physic_addr : fiziksel adresin atilacagi adres.
 */
uint32_t kmalloc_aphysic(uint32_t size,uint32_t *physic_addr){

	return kmalloc_orig(size,true,physic_addr);

}

/*
 * kmalloc,belirtilen boyut kadar bellek tahsis eder. heap
 * baslatilmadan tahsis yapilabilir fakat tahsis edilen
 * bellek bolgeleri bosa cikarilamaz. heap baslatildiktan
 * sonra tahsis edilen bellek bolgeleri bosa cikarilabilir.
 * heap baslatildiktan sonra diger malloc,valloc,realloc ve
 * calloc fonksiyonlarinin kullanilmasi daha yerinde olacaktir.
 * heap baslatildiktan sonra malloc yerine kmalloc'ta 
 * kullanilabilir.
 *
 * @param size : boyut (bayt olarak)
 */
uint32_t kmalloc(uint32_t size){

	return kmalloc_orig(size,false,NULL);

}

/*
 * detect_heap_block_type,tahsis icin en uygun tipi belirler.
 *
 * @param size : tahsis edilmek istenen boyut (bayt olarak). 
 */
static uint32_t detect_heap_block_type(uint32_t size){

	/*
	 * boyutun kac bitle ifade edildigi buluyoruz.ornegin 4
	 * sayisi ikili sistemde "100" dir. yani toplamda 3 bit
	 * ile ifade edilir.
	 */
	uint32_t blk_type = find_bit_count(size);
	
	if(blk_type <= 2)
		return 0;

	blk_type -= (is_pow_two(size) + 2);

	if(blk_type > BIG_BLOCK)
		return BIG_BLOCK;

	return blk_type;

}

/* small blocks */
static heap_blk_t heap_small_blks[SMALL_BLOCK + 1];
/* big root block */
static heap_big_root_blk_t heap_big_root;

/*
 * get_heap_blk_header,small blocklar icin ilk header'i dondurur.
 *
 * @param blk : small block isaretcisi
 */
static heap_blk_header_t *get_heap_blk_header(heap_blk_t *blk){

	return blk->first;

}


/*
 * blk_part_push, small block listesinden aldigimiz bir blok parcasini
 * small block listesine atmamiza yani bosa cikarmamizi saglar.
 *
 * @param header : small block header isaretcisi
 * @param ptr : small block parcasi isaretcisi
 */
static void *blk_part_push(heap_blk_header_t *header,void *ptr){

	assert(ptr && (uint32_t)header < (uint32_t)ptr);
	uint32_t *addr = ptr;
	*addr = (uint32_t*)header->point;
	header->point = addr;

}

/*
 * blk_part_pop, small block listesinden bir small block
 * parcasi alir ve geri dondurur.
 *
 * @param header : normal(small block) blok isaretcisi
 */
static void *blk_part_pop(heap_blk_header_t *header){

	assert(header && header->point);	
	void *addr = header->point;
	uint32_t *point = header->point;
	header->point = *point;
	
	return addr;

}

/*
 * detect_big_heap_type, big blocklar icin blok tipini belirler.
 * buna gore big block listelerine yerlestirilirler.
 *
 * @param size : boyut (bayt olarak)
 */
static uint32_t detect_big_heap_type(uint32_t size){

	uint32_t page_count = (size + sizeof(heap_big_blk_t)) / PAGE_SIZE + 1;
	uint32_t blk_type = find_bit_count(page_count);

	/*
	 * ===============
	 * 1-4 	    [0]
	 * 5-8      [1]
	 * 9-16	    [2]
	 * 17-32    [3]
	 * 31-64    [4]
	 * 63-128   [5]
	 * 127-256  [6]
	 * 255-512  [7]
	 * 511-1024 [8]
	 * 1025-... [9]
	 * ===============
	 */

	if(blk_type <= 2)
		blk_type = 0;
	else{
		
		blk_type -= (is_pow_two(page_count) + 2);
		
		if(blk_type >= BIG_MAX_TYPE)
			blk_type = BIG_MAX_TYPE;
			
	}

	return blk_type;

}

/*
 * big_blk_list_insert,belirtilen big block'u listeye ekler.
 *
 * @param header : big block header isaretcisi
 */
static void big_blk_list_insert(heap_big_blk_t *header){

	uint32_t blk_type = detect_big_heap_type(header->size);
	
	if(!heap_big_root.node[blk_type]){

		heap_big_root.node[blk_type] = header;
		header->next = NULL;

	}
	else{

		header->next = heap_big_root.node[blk_type];
		heap_big_root.node[blk_type]->prev = header;

	}
	
	header->prev = heap_big_root.node[blk_type];

} 

/*
 * big_blk_find_best_size,aranan boyutta uygun bos big block var mi
 * diye kontrol eder.
 *
 * @param search_size : aranan boyut
 */
static heap_big_blk_t *big_blk_find_best_size(uint32_t search_size){


	uint32_t blk_type = detect_big_heap_type(search_size);
	heap_big_blk_t *big_blk = (heap_big_blk_t*)heap_big_root.node[blk_type];

	while(big_blk && big_blk->size >= search_size){

			if(!big_blk->next)
				break;	

			big_blk = big_blk->next;
	
	}

	if(big_blk->size < search_size)
		return NULL;	

	return big_blk;

}

/*
 * big_blk_list_delete,belirtilen big block'u listeden siler.
 *
 * @param header : big block header isaretcisi
 */
static void big_blk_list_delete(heap_big_blk_t *header){

	uint32_t blk_type = detect_big_heap_type(header->size);
	heap_big_blk_t *big_blk = (heap_big_blk_t*)heap_big_root.node[blk_type];

	while(big_blk){

			if((uint32_t)big_blk == (uint32_t)header)
				break;

			big_blk = big_blk->next;
	
	}

	
	if(big_blk->next)
			((heap_big_blk_t*)big_blk->next)->prev = big_blk->prev;
	
	if(!big_blk->prev)
		heap_big_root.node[blk_type] = big_blk->next;
	else{
		
		if(big_blk->prev)		
			((heap_big_blk_t*)big_blk->prev)->next = big_blk->next;

	}

}

/*
 * _kmalloc, heap alanindan istenen boyutta bellek ayrilir.
 *
 * @param size : heap alanindan ayrilacak boyut(bayt olarak) 
 */
static void *_kmalloc(uint32_t size){

	/*
	 * eger boyut sifirsa islem yapmaya gerek yok.
	 */
	if(!size)
		return NULL;

	uint32_t blk_type = detect_heap_block_type(size);

	if(blk_type >= BIG_BLOCK){
		
		/*
		 * big block
		 */

		/*
		 * tahsis edilecek boyut kadar bos bloklardan uygun yer
		 * ariyoruz.
		 */
		heap_big_blk_t *big_blk = big_blk_find_best_size(size);

		/*
		 * eger uygun yer bulunamamissa yani big_blk "null" ise.
		 */
		if(!big_blk){
			
			/*
			 * sbrk ile uygun boyutta sayfa tahsisi gerceklestiriyoruz.
			 */
			uint32_t page_count = (size + sizeof(heap_big_blk_t)) / PAGE_SIZE + 1;
			big_blk = (heap_big_blk_t*)sbrk(page_count * PAGE_SIZE);
			assert(!((uint32_t)big_blk % PAGE_SIZE));
			
			big_blk->magic = BLOCK_MAGIC;
			big_blk->size = page_count * PAGE_SIZE - sizeof(heap_big_blk_t);
			
		}
		/*
		 * eger uygun yer bulunmussa
		 */
		else
			big_blk_list_delete(big_blk);

		return (void*)big_blk + sizeof(heap_big_blk_t);

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
				 * sayfayi belirlenen blok boyutuna gore ayarliyoruz.
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
		void *ret = blk_part_pop(small_blk);

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

}


/*
 * _kfree,belirtilen bellek bolgesini bos'a cikarir.
 *
 * @param ptr : isaretci
 */
static void _kfree(void *ptr){

	/*
	 * eger bos bir isaretci ise islemlere gerek yok
	 */
	if(!ptr)
		return;

	/* sayfa hizalamasi yapilmis blok mu? */
	if(!((uint32_t)ptr % PAGE_SIZE))
		ptr = (void*)ptr - 1;
		
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
		 
		heap_big_blk_t *big_blk = (heap_big_blk_t*)(blk_header);
		big_blk_list_insert(big_blk);

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
 * _kcalloc, istenilen eleman sayisi kadar verilen
 * eleman boyutuna gore yer tahsis edilir.
 *
 * @param nmem : eleman sayisi
 * @param size : eleman boyutu
 */
static void *_kcalloc(uint32_t nmem,uint32_t size){

	void *ptr = _kmalloc(nmem * size);

	if(ptr)
		memset(ptr,0,nmem * size);

	return ptr;

}

/*
 * _kvalloc,sayfa hizalamasi yaparak bellek tahsisi yapar,
 * zorunlu olmadigi surece kullanmamaya dikkat edin!!!
 * 
 * @param size : boyut (bayt olarak)
 */
static void *_kvalloc(uint32_t size){

	if(!size)
		return (void*)NULL;

	uint32_t alloc_size = size + PAGE_SIZE - sizeof(heap_big_blk_t);
	void *ptr = _kmalloc(alloc_size);
	void *out_addr = (void*)((uint32_t)ptr + (PAGE_SIZE - sizeof(heap_big_blk_t)));
	
	assert((uint32_t)out_addr % PAGE_SIZE == 0);
	return out_addr;

}

/*
 * _krealloc,eski tahsis edilen bolgenin boyutunu
 * degistirir.
 *
 * @param ptr : isaretci
 * @param size : boyut (bayt olarak)
 */
static void *_krealloc(void *ptr,uint32_t size){


	/* eger isaretci null ise boyut kadar bellek ayir */
	if(!ptr)
		return _kmalloc(size);

	/* boyut bos ise isaretciyi serbest birak */
	if(!size){

		_kfree(ptr);
		return NULL;

	}

	heap_blk_header_t *blk =  (void*)((uint32_t)ptr & (uint32_t)~PAGE_MASK);

	/*
	 * sihirli numara gecersiz ise :/
	 */
	if(blk->magic != BLOCK_MAGIC){

		assert(0 && "bad heap block magic!");
		return NULL;

	}

	uint32_t old_size = blk->size;

	/*
	 * small block ise
	 */
	if(old_size < BIG_BLOCK){
		
		#define small_blk_size(x)	(1 << (2 + x))
		old_size = small_blk_size(old_size)
		#undef small_blk_size

	}
	
	/*
	 * eger eski boyut tahsis edilecek boyuttan buyukse
	 * eski isaretciyi geri dondur.
	 */
	if(old_size >= size)
		return ptr;

	/*
	 * yeni boyut icin bellek tahsisi
	 */
	void *out = _kmalloc(size);
	
	/*
	 * tahsis basariliysa
	 */
	if(out){

		/*
		 * yeni bellek bolgesine eski isaretcinin gosterdigi
		 * bellek bolgesindeki verileri kopyala
		 */
		memcpy(out,ptr,old_size);
		_kfree(ptr);	
	
		return out;

	}

	/*
	 * tahsis basarili degilse :S
	 */
	return (void*)NULL;

}

/*
 * __heap_test
 */
void __heap_test(void){


}

/*
 * heap_init, heap'i baslatir.
 */
void heap_init(void){
	
	debug_print(KERN_INFO,"Initializing the heap.");
	heap_info.current_end = (last_addr + FRAME_SIZE_BYTE) & ~PAGE_MASK;


}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
