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
#include <uniq/spin_lock.h>
#include <compiler.h>
#include <uniq/multiboot.h>
#include <arch.h>

#define PAGE_FAULT_INT		14		/* page fault kesme numarasi */
/* page fault flags */
#define PF_PRESENT		0x1
#define PF_WOP			0x2		/* wop = write operation :) */
#define PF_USRMODE		0x4
#define PF_RESERVED		0x8
#define PF_INSTRUCTION		0x10
#define BITS_PER_BYTE		8		/* byte basina bit sayisi */
#define PAGING_ENABLE		0x80000000
#define PAGING_DISABLE		0x7fffffff

#if 0
	#define MEM_NORMAL_USE
#endif

typedef struct{
	uint32_t total_mem;			/* toplam bellek (multiboot'tan aldigimiz bellek miktari) */
	uint32_t nframe;			/* frame sayisi */
	uint32_t *frame_map;			/* frame map adresi */
	uint32_t aframe_size;			/* frame map icin tahsis edilmis bellek miktari (byte olarak) */
	int8_t remaining;			/* bunun amacini kod ustunde incelemeniz daha faydali olur ;),
					 	* mod islemleri sonrasi kalan frame sayisi tuttugunu soyliyim.
					 	*/
}mp_info_t;

static mp_info_t mp_info;
extern uint32_t last_addr;			/* linker "end" adresi */
extern heap_info_t heap_info;
static volatile uint32_t alloc_flock = 0;

page_dir_t *kernel_dir = NULL;
page_dir_t *current_dir = NULL;

static void set_frame(uintptr_t frame_addr);

#define MMAP_AVAILABLE		0x1
#define MMAP_RESERVED		0x2
#define PAGE_MASK		0xfff
#define MMAP_LIMIT		0xFFFFFFFF

/*
 * sync_mmap,
 *
 * @param addr :
 */
void sync_mmap(mboot_info_t *mboot_info){


	mboot_memmap_t *memmap = (mboot_memmap_t*)mboot_info->mmap_addr;
	debug_print(KERN_NOTICE,"synchronizing the memory map");
	debug_print(KERN_DUMP,"memmap : %p, mmap_addr : %p, mmap_length : %u byte",memmap,mboot_info->mmap_addr,
										   mboot_info->mmap_length);
	debug_print(KERN_DUMP,"type1 : available memory, type2 : reserved(system ROM, memory-mapped device, etc.)\n");

	/*
	 * type1 bizim kullanibilecegimiz bellek bolumudur.
	 */	
	while((uint32_t)memmap < mboot_info->mmap_addr + mboot_info->mmap_length){

		/*
		 * debug_print,printf fonksiyonunu cagirarak yazma islemi yapar. fakat bu
		 * cekirdekteki printf 32bit desteklidir. eger yan yana 64 bit sayilari
		 * yazmaya kalkarsaniz ortalik karisir ;).
		 */
		debug_print(KERN_DUMP,"-> memmap : %p , size : %u byte, type : %u",memmap,memmap->size,memmap->type);
		debug_print(KERN_DUMP,"base_addr : %p",memmap->base_addr);
		debug_print(KERN_DUMP,"length : %p byte",memmap->length);

		if(memmap->type == MMAP_RESERVED){

			for(uint32_t i = 0; i < memmap->length; i += PAGE_SIZE){
				
				if(memmap->base_addr + i > MMAP_LIMIT)	
					break;
				#if 0
					debug_print(KERN_DUMP,"synchronizing %p",(uint32_t)(memmap->base_addr + i));
				#endif				
				set_frame((memmap->base_addr + i) & ~PAGE_MASK);
				
			}

		}
		
		memmap = (mboot_memmap_t*)((uint32_t)memmap + memmap->size + sizeof(uint32_t));
		

	}
	
}

#if (__kern_arch__ > __arch_i386__)

	#define __invlpg_supported__
	/*
	 * invlpg,
	 *
	 * @param addr:
	 */	
	void invlpg(uint32_t addr){
		
		__asm__ volatile("movl %0,%%eax\n\t"
				 "invlpg (%%eax)\n\t"
				 :
				 : "r"(addr)
				 : "%eax"
				 );		
		
	}

	/*
	 * invlpg_tables,
	 */
	void invlpg_tables(void){

		__asm__ volatile("movl %%cr3,%%eax\n\t"
				 "movl %%eax,%%cr3\n\t"
				 :
				 :
				 : "%eax"
				 );

	}
#endif

/*
 * total_memory_size,toplam bellegin framelere boyutlarina uygun olarak
 * ayarlanmasi sonrasi frame boyutlarina gore toplam bellegi hesaplar
 * ve geri dondurur. frame boyutlari  4 KiB boyutundadir. ve sonuc
 * KiB seklindedir!.
 */
uint32_t total_memory_size(void){
	
	return mp_info.nframe * FRAME_SIZE_KIB;

}

/*
 * use_memory_size, kullanilan bellek boyutunu dondurur. kullanilan
 * frame sayisinin boyutu hesaplar ve geri dondurulur. geri donen
 * boyut KiB seklindedir.
 */
uint32_t use_memory_size(void){

	uint32_t use = 0,index,max_index,offset;

#ifdef MEM_NORMAL_USE
	max_index = mp_info.nframe / 32;

	for(index = 0; index < max_index; index++){
				for(offset = 0; offset < 32; offset++){
					uint32_t cntrl = 0x1 << offset;
					if(mp_info.frame_map[index] & cntrl)
						use++;	
				}
	}
#else
	max_index = mp_info.aframe_size / 4;

	if(mp_info.aframe_size % 4)
		max_index++;

	for(index = 0; index < max_index; index++){

		if((index == max_index - 1) && mp_info.remaining >= 0){	
				for(offset = 0; offset < mp_info.remaining; offset++){
					uint32_t cntrl = 0x1 << offset;
					if(mp_info.frame_map[index] & cntrl)
						use++;
				}
		}
		else{
				for(offset = 0; offset < 32; offset++){
					uint32_t cntrl = 0x1 << offset;
					if(mp_info.frame_map[index] & cntrl)
						use++;	
				}
		}


	}
#endif

	return use * 4;	

}


#define HEAP_SIZE_1x	0x01000000	/* 16 MiB */
#define HEAP_SIZE_2x	0x02000000	/* 32 MiB */
#define HEAP_SIZE_4x	0x04000000	/* 64 MiB */
#define HEAP_SIZE_8x	0x08000000	/* 128 MiB */
#define HEAP_SIZE_16x	0x10000000	/* 256 MiB */
#define HEAP_SIZE_32x	0x20000000	/* 512 MiB */

/*
 * calc_heap_size, toplam bellek miktarina gore heap alaninin
 * boyutunu hesaplar. sistemin en az 64 MiB olmasi gerekiyor!
 */
static void calc_heap_size(void){
	
	uint32_t total_mem = total_memory_size();
	uint32_t n = total_mem / 8192;
	
	#define byte_to_kib(x)	(x / 1024)

	if(n >= 8 && n < 16) /* 64 MiB - 128 MiB */
		heap_info.size = HEAP_SIZE_1x;
	else if(n >= 16 && n < 32) /* 128 MiB - 256 MiB */
		heap_info.size = HEAP_SIZE_2x;
	else if(n >= 32 && n < 64) /* 256 MiB - 512 MiB */
		heap_info.size = HEAP_SIZE_4x;
	else if(n >= 64 && n < 128) /* 512 MiB - 1024 MiB */
		heap_info.size = HEAP_SIZE_8x;
	else if(n >= 128 && n < 256) /* 1024 MiB - 2048 MiB */
		heap_info.size = HEAP_SIZE_16x;
	else /* 2048 MiB > */
		heap_info.size = HEAP_SIZE_32x;

	if(n < 8)
		die("The size of memory is critically low for heap.");
	else if(n < 64)
		debug_print(KERN_INFO,"The size of memory is normal level for heap. Heap size : %u KiB",
										byte_to_kib(heap_info.size));
	else
		debug_print(KERN_INFO,"The size of memory is very good level for heap. Heap size : %u KiB",
										byte_to_kib(heap_info.size));

	#undef byte_to_kib

}

/*
 * enable_paging, sayfalamayi aktif hale getirir.
 */
static void enable_paging(void){

	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= PAGING_ENABLE;
	__asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

}

/*
 * disable_paging, sayfalamayi devre disi birakir.
 */
static void disable_paging(void){

	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 &= PAGING_DISABLE;
	__asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
	
}


/*
 * set_frame, frame_map'te frame'in kullanilmaya basladigina
 * dair frame ait bit '1' seklinde set edilir.
 *
 * @param frame_addr : frame adresi
 */
static void set_frame(uintptr_t frame_addr){

	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);

	mp_info.frame_map[index] |= (0x1 << offset);
	
}

/*
 * remove_frame, frame_map'te frame'in kaldirildigina dair
 * frame ait bit '0' haline getirilir.
 *
 * @param frame_addr : frame adresi
 */
static void remove_frame(uintptr_t frame_addr){
	
	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);

	mp_info.frame_map[index] &= ~(0x1 << offset);
	
}

/*
 * cntrl_frame, frame'in frame_map'te olup olmadigini kontrol eder.
 *
 * @param frame_addr : frame adresi
 */
static uint32_t cntrl_frame(uintptr_t frame_addr){
	
	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);
	
	return (mp_info.frame_map[index] & (0x1 << offset));
	
}

/*
 * find_free_frame, ilk bos frame'i bulur ve index numarasini
 * dondurur.
 */
static uint32_t find_free_frame(void){

	uint32_t index,offset,max_index;
	
#ifdef MEM_NORMAL_USE
	max_index = mp_info.nframe / 32;

	for(index = 0; index < max_index; index++){
			if(mp_info.frame_map[index] != MAX_LIMIT){
				for(offset = 0; offset < 32; offset++){
					uint32_t cntrl = 0x1 << offset;
					if(!(mp_info.frame_map[index] & cntrl))
						return index * 32 + offset;	
				}
			}
	}
#else
	max_index = mp_info.aframe_size / 4;

	if(mp_info.aframe_size % 4)
		max_index++;
	
	for(index = 0; index < max_index; index++){

		if(mp_info.remaining >=0 && (index == (max_index-1))){	
			if(mp_info.frame_map[index] != MAX_LIMIT){
				for(offset = 0; offset < mp_info.remaining; offset++){
					uint32_t cntrl = 0x1 << offset;
					if(!(mp_info.frame_map[index] & cntrl))
						return index * 32 + offset;	
				}
			}
		}
		else{
			if(mp_info.frame_map[index] != MAX_LIMIT){
				for(offset = 0; offset < 32; offset++){
					uint32_t cntrl = 0x1 << offset;
					if(!(mp_info.frame_map[index] & cntrl))
						return index * 32 + offset;	
				}
			}

		}

	}
#endif

	return MAX_LIMIT;

}

/*
 * alloc_frame, bellekten bir frame yada diger bir tabirle sayfa tahsis eder.
 *
 * @param page : sayfa adresi
 * @param rw : okuma/yazma izni ( false ise sadece okuma , true ise hem okuma hem yazma)
 * @param user : kullanici izni ( false ise kullanici moddan izin yok, true ise kullanici
 * 				  moddan da erisim izni var)
 */
void alloc_frame(page_t *page,bool rw,bool user){

	if(page->frame){
		
		page->present = PAGE_PRESENT;
		page->rw      = (rw) ? PAGE_RWRITE : PAGE_RONLY;
		page->user    = (user) ? PAGE_USER_ACCESS : PAGE_KERNEL_ACCESS;
		return;
		
	}

	spin_lock(&alloc_flock);
	/* bos frame bul */
	uint32_t index = find_free_frame();
#if 0
	debug_print(KERN_DUMP,"frame index %u",index);
#endif	
	if(index == MAX_LIMIT)
		die("Not found the free frame!");

	/* frame'i ayarla */
	set_frame(index * FRAME_SIZE_BYTE);

	page->present = PAGE_PRESENT;
	page->rw      = (rw) ? PAGE_RWRITE : PAGE_RONLY;
	page->user    = (user) ? PAGE_USER_ACCESS : PAGE_KERNEL_ACCESS;
	page->frame   = index;
	spin_unlock(&alloc_flock);
	
}

/*
 * dma_frame,belirtilen adresteki sayfayi direk ayarlariz. diger turlu
 * bos frame find_free_frame kendisi buluyor ve islemi yapiyordu.
 *
 * @param page : sayfa adresi
 * @param rw : okuma/yazma izni ( false ise sadece okuma , true ise hem okuma hem yazma)
 * @param user : kullanici izni ( false ise kullanici moddan izin yok, true ise kullanici
 * @param addr : bellek adresi
 */
void dma_frame(page_t *page,bool rw,bool user,uintptr_t addr){

	page->present = PAGE_PRESENT;
	page->rw      = (rw) ? PAGE_RWRITE : PAGE_RONLY;
	page->user    = (user) ? PAGE_USER_ACCESS : PAGE_KERNEL_ACCESS;
	page->frame   = addr / FRAME_SIZE_BYTE;

	if (addr < mp_info.nframe * FRAME_SIZE_BYTE)
		set_frame(addr);
	
}

/*
 * free_frame, frame(sayfa)'i bosa cikarir.
 *
 * @param page : sayfa adresi
 */
void free_frame(page_t *page){

	if(!page->frame)
		return;

	remove_frame(page->frame * FRAME_SIZE_BYTE);
	page->frame = 0;
	
}

/*
 * page_fault,sayfalama hatasi oldugunda calisicak fonksiyondur.
 * 
 * @param regs : kaydediciler
 */
void page_fault_handler(registers_t *regs){
	
	uint32_t fault_addr;
	__asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
	char err_desc[128];
	snprintf(err_desc,sizeof(err_desc)-1,"Page Fault ! \033[1;37m%p\033[0m \nError description :",fault_addr);

	if(!(regs->err_code & PF_PRESENT))
		strcat(err_desc," present ");
	if(regs->err_code & PF_WOP)
		strcat(err_desc," read-only ");
	if(regs->err_code & PF_USRMODE)
		strcat(err_desc," user mode ");
	if(regs->err_code & PF_RESERVED)
		strcat(err_desc," reserved ");
	if(regs->err_code & PF_INSTRUCTION);

	die(err_desc);
 
}

/*
 * get_page,bir sayfa ayarlamamizi saglar. 
 *
 * @param addr : adres
 * @param make : verilen adresin tablo index'i hesaplaninyor ve index'in
 * 		 oldugu tablo bellekte ise sorun yok tabloya gidip tablodaki
 *		 sayfa index'i hesaplanarak geri donduruluyor. eger ilk
 * 		 kontrol islemininde sayfaya ait sayfa tablosunun bellekte
 * 		 yoksa atliyor ve 2. kontrol'e geliyor eger biz make'i
 * 		 true yaparsak, o adresin sayfa tablosu yok ise olustur
 * 		 demek istiyoruz. make'in aciklmasini boyle yapmak daha iyi
 *		 oldu sanki :).
 * @param dir : sayfa dizini
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
	
	return NULL;

}

/*
 * change_page_dir,sayfa dizinini degistirir.
 *
 * @param new_dir : yeni sayfa dizini adresi.
 */
void change_page_dir(page_dir_t *new_dir){

	current_dir = new_dir;
	__asm__ volatile("mov %0, %%cr3" :: "r"(&new_dir->tables_physic));
	enable_paging();

}

/*
 * set_mp_info, mp_info(memory paging info) yapisini ayarlar.
 *
 * @param mp_info : mp_info yapisi adresi
 * @param mem_size : bellek miktari (byte olarak)
 */
static void set_mp_info(mp_info_t *mp_info,uint32_t mem_size){

	/* mp_info adresi bos ise */
	if(!mp_info)
		die("mp_info address is not found!");

	mp_info->total_mem = mem_size;	

#ifdef MEM_NORMAL_USE
	mp_info->nframe = mem_size / FRAME_SIZE_KIB;
	uint32_t alloc_frame_byte = mp_info->nframe / BITS_PER_BYTE;
#else
	mp_info->nframe = mem_size / FRAME_SIZE_KIB;
	uint32_t alloc_frame_byte = mp_info->nframe / BITS_PER_BYTE;
	uint8_t alloc_frame_remaining = mp_info->nframe % BITS_PER_BYTE; 
	alloc_frame_byte += !!(alloc_frame_remaining);

	if(mp_info->nframe % 32 > 0)
		mp_info->remaining = mp_info->nframe % 32;
	else
		mp_info->remaining = -1;

#endif

	mp_info->aframe_size = alloc_frame_byte;
	mp_info->frame_map = (uint32_t*)kmalloc(alloc_frame_byte);
 	memset(mp_info->frame_map,0,alloc_frame_byte);

}

/*
 * dump_mp_info, mp_info(memory paging info) yapisini ekrana yazdirir.
 *
 * @param mp_info : mp_info yapisi adresi
 */
static void dump_mp_info(mp_info_t *mp_info){

	/* mp_info adresi bos ise */
	if(!mp_info)
		die("mp_info address is not found!");
	
	/* frame adresi bos ise :/ */
	if(!mp_info->frame_map)
		die("Frame map address is not found!");

	debug_print(KERN_DUMP,"Dump the memory paging info :");
	debug_print(KERN_DUMP,"Frame map address is \033[1;37m%p\033[0m, Frame map end address is \033[1;37m%p",
												mp_info->frame_map,
												mp_info->frame_map + mp_info->aframe_size / 4);
	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p",last_addr);	
	debug_print(KERN_DUMP,"total memory size : %u KiB, total frame : %u",mp_info->total_mem,mp_info->nframe);

#ifdef MEM_NORMAL_USE
	debug_print(KERN_DUMP,"frame map size : %u Byte, available memory size : %u KiB",mp_info->aframe_size,
										    	 (mp_info->nframe / 32) * 32 * FRAME_SIZE_KIB);

	debug_print(KERN_DUMP,"unused memory size : %u KiB",mp_info->total_mem - (mp_info->nframe / 32) * 32 * FRAME_SIZE_KIB);
#else
	debug_print(KERN_DUMP,"frame map size : %u Byte, available memory size : %u KiB",mp_info->aframe_size,
										    	 mp_info->nframe * FRAME_SIZE_KIB);

	debug_print(KERN_DUMP,"unused memory size : %u KiB",mp_info->total_mem - mp_info->nframe * FRAME_SIZE_KIB);
#endif

}

/*
 * paging_final,sayfalama icin son ayarlari yapar.
 */
void paging_final(void){

	/*
	 *  bellekte hangi sayfalarin ve sayfa tablolarinin oldugunu belirtelim.
	 */
	debug_print(KERN_INFO,"Initializing the memory mapping.");
	debug_print(KERN_DUMP,"Memory mapping size : %u KiB",use_memory_size());
	
	/* 
	 * ilk 1 MiB icin mapping islemi uygulayalim 
	 */
	debug_print(KERN_DUMP,"(0x00000000 - 0x00100000) mapping. %u KiB",0x00100000 / 1024);	
	for(uint32_t i = 0; i < 0x100000; i += FRAME_SIZE_BYTE)
		dma_frame(get_page(i,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS,i);

	/*
	 * 1 MiB'dan last_addr(linker "end") + 0x4000 'e kadar mapping islemi 
	 * gerceklestiriyoruz. + 0x4000'u ise asil heap alanimiz ile last_addr
	 * arasinda tampon bolge oldugunu dusunebilirsiniz.
	 */
	for(uint32_t j = 0x100000; j < last_addr + 0x4000; j += FRAME_SIZE_BYTE)
		dma_frame(get_page(j,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS,j);

	/* 
	* vga text-mode video bellegini remapping isleminden geciriyoruz.
	*/
	debug_print(KERN_DUMP,"(0xB8000-0xC0000) remapping vga text-mode dma. %u KiB",(0xC0000-0xB8000)/1024);
	for(uint32_t k = 0xB8000; k < 0xC0000; k += FRAME_SIZE_BYTE)
		dma_frame(get_page(k,false,kernel_dir),PAGE_RWRITE,PAGE_USER_ACCESS,k);

	/*
	 * asil heap alanimiz 8 MiB'tan basliyor!
	 */
	uint32_t tmp_heap_start = KHEAP_INIT;
	
	/*
	 * eger last_addr(linker "end") + 0x4000 asil heap alanimizin adresini asmissa heap
	 * baslangic adresini 1 MiB ileri tasiyoruz.
	 */
	if (tmp_heap_start <= last_addr + 0x4000){
		
		debug_print(KERN_ERROR, "Kernel heap init address error!");
		debug_print(KERN_DUMP,"tmp_heap_start : %p, last_addr + 0x4000 : %p", tmp_heap_start,last_addr + 0x4000);
		tmp_heap_start = last_addr + 0x100000;
		debug_print(KERN_DUMP,"new tmp_heap_start : %p",tmp_heap_start);
		
	}
	heap_info.alloc_point = tmp_heap_start;

	/*
	 * mapping isleminden en son kaldigimiz yerden asil heap alani baslangicina kadar
	 * sayfalari kullanmak icin ayarliyoruz.
	 */
	for (uint32_t i = last_addr + 0x4000; i < tmp_heap_start  ; i += FRAME_SIZE_BYTE)
		alloc_frame(get_page(i,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS);

	/*
	 * heap boyutu bellek boyutuna bagli olarak degiskenlik gosterecektir.
	 * yukaridaki calc_heap_size fonksiyonunu inceleyiniz.
	 */
	heap_info.end_point = tmp_heap_start + heap_info.size;
	debug_print(KERN_DUMP,"(%p - %p) preallocation for heap. %u Byte / %u KiB",heap_info.alloc_point,
								  		   heap_info.end_point,
								   	  	   heap_info.size,
								  		   heap_info.size/1024);
	/*
	 * yukaridaki heap baslangicina kadar olan mapping isleminden farkli olarak
	 * asil heap baslangic adresinden son adresine kadar bir preallocation
	 * yapiyoruz diyelibiliriz. sayfa ve sayfa tablolari ayarlaniyor sadece,
	 * yukarida alloc_frame fonksiyonuyla birlikte sayfalarin bellek kullanilabilir
	 * oldugunuda yani bellekte oldugunu belirtiyorduk. burada sadece gerekli on
	 * ayarlamayi yapiyoruz. gerekli oldugunda sbrk fonksiyonuyla bu sayfalari
	 * alloc_frame ile kullanilabilir hale getirecegiz.
	 */
	for (uint32_t i = heap_info.alloc_point ; i < heap_info.end_point ; i += FRAME_SIZE_BYTE)
		get_page(i,true,kernel_dir);
	
	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p\033[0m",last_addr);
	debug_print(KERN_DUMP,"Memory mapping size : %u KiB",use_memory_size());
	
	/*
	 * page fault kesme isleyicisini ayarlayip, sayfa dizinini
	 * degistiriyoruz.
	 */
	isr_add_handler(PAGE_FAULT_INT,page_fault_handler);
	change_page_dir(kernel_dir);

}

/*
 * paging_init, sayfalamayi baslatir.
 *
 * @param mem_size : bellek boyutu ( multiboot yapisindan aliyoruz)
 */
void paging_init(uint32_t mem_size){

	if(!mem_size)
		die("Memory size is not found!");
	
	debug_print(KERN_INFO,"Initializing the paging.");
	set_mp_info(&mp_info,mem_size);
	dump_mp_info(&mp_info);
	calc_heap_size();

	kernel_dir = (page_dir_t *)kmalloc_align(sizeof(page_dir_t));
	memset(kernel_dir,0,sizeof(page_dir_t));
	current_dir = kernel_dir;

}

/*
 * sbrk,heap bolgesinden bellek tahsisi yapmamizi saglar. memory
 * allocator fonksiyonlarininda sayfa tahsisi icin bu fonksiyon
 * kullanilir.
 *
 * @param inc : artim boyutu yada diger bir tabirle tahsis
 *              edilmek istenen boyut daha dogru olur. bu boyut
 *		sayfa boyutu katlarında olmalidir.
 */
void *sbrk(uint32_t inc){

	/* heap baslatilmamis */
	if(!heap_info.current_end)
		die("uninitialized heap!!!");
	
	/* istenilen artim boyutu sayfa boyutu katlarinda degil */
	if(inc % FRAME_SIZE_BYTE)
		die("heap increment size isn't such as page size. :/");

	/* heap dolu ise :( */
	if(heap_info.current_end + inc >= heap_info.end_point)
		die("heap space is full :/.");

	uint32_t addr = heap_info.current_end;
	
	/*
	 * eger extra olarak yada asil heap alanimiza girdiysek
	 * ordan tahsis islemi gerceklestirecegiz. paging_final'de
	 * sadece heap alaninin sayfalari icin preallocation yapmistik.
	 */
	if(heap_info.current_end + inc > heap_info.alloc_point){
		debug_print(KERN_INFO,"expanding the heap.!");
		
		for(uint32_t i = heap_info.current_end; i < heap_info.current_end + inc; i += FRAME_SIZE_BYTE){
			
			debug_print(KERN_DUMP,"frame addr : %p", i);
			alloc_frame(get_page(i,false,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS);
			
		}
		debug_print(KERN_DUMP,"successful!");
			
		#ifdef __invlpg_supported__
			invlpg_tables();
		#endif
		
	}

	/* heap'in son gecerli adresini yeniliyoruz */
	heap_info.current_end += inc;
	memset((void*)addr,0,inc);
	
	return (void*)addr;

}


/*
 * __page_fault_test
 */
void __page_fault_test(void){

	uint32_t *ptr = (uint32_t*)(0xb0000000);
  	uint32_t fault = *ptr;

}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
