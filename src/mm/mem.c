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

/*
 * memory mapping'de 2 yontem olusturdum. MEM_NORMAL_USE tanimlamasi
 * kullanilirsa bellek framelere ayrilmasinda frame sayisi hem 8 hemde
 * 32'ye tam bolunecek sekilde olusturulmustur. bunun nedenlerini 
 * aciklayacak olursak framelerin kontrolleri ve diger islemlerde
 * frame_map'in 32 bit olarak islemden gecmesi ve frame_map'e bellek
 * ayrilirken byte(8 bit) seklinde hesaplanmasi kolay ve hizli olmasidir,
 * fakat artik bellek miktari fazla olabilir. diger yontemde ise bellekte
 * en fazla 3 KiB artik bellek kalabilecegi sekilde tasarlanmistir yani
 * mecburen yinede kaliyor ama dedigim gibi en fazla 3 KiB :),bildiginiz
 * gibi sayfalamada 4 KiB gibi bir boyut kullaniliyordu. hiz olayina 
 * geldigimizde ornek vermek gerekirse 256 MiB bellegi tahsis islemine
 * tutalim. MEM_NORMAL USE tanimli bir cekirdekte 256 MiB bellek tahsis
 * islemini 4 sn gibi surede gerceklestiriyorsa,diger yontemle 5 sn
 * gibi surede gerceklestiriyor. eger bellegi sonuna kadar kullanmak
 * istiyorsaniz MEM_NORMAL_USE tanimini kullanmayabilirsiniz. ornegin
 * ben 128 MiB bellek icin bu olayi gerceklestirdim MEM_NORMAL_USE
 * tanimiyla kullanilan mapping isleminde 639 KiB bellek artik olarak
 * kalirken diger yontemde dedigimiz gibi en fazla 3 KiB artik olustu.
 * MEM_NORMAL_USE kullanilmayabilir her zaman yuksek bir miktarda bellek
 * tahsisi yapilmayacagi icin hiz faktoru es gecebiliriz,bellegi iyi
 * kullanalim yeter ki .).
 */
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
	uint32_t alloc_memf;			/* bellekten tahsis edilen frame sayisi */
}mp_info_t;

static mp_info_t mp_info;
extern uint32_t last_addr;			/* linker "end" adresi */
static volatile uint32_t alloc_flock = 0;

page_dir_t *kernel_dir = NULL;
page_dir_t *current_dir = NULL;

/*
 * total_memory_size,toplam bellegin framelere boyutlarina uygun olarak
 * ayarlanmasi sonrasi frame boyutlarina gore toplam bellegi hesaplar
 * ve geri dondurur. frame boyutlari  4 KiB boyutundadir. ve sonuc
 * KiB seklindedir!.
 */
uint32_t total_memory_size(void){

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
	if(mp_info.remaining > 0)
		max_index = mp_info.aframe_size / 4 + 1;
	else
		max_index = mp_info.aframe_size / 4;

	for(index = 0; index < max_index; index++){

		if(mp_info.remaining >=0 && (index == (max_index-1))){	
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

/*
 * use_memory_size, kullanilan bellek boyutunu dondurur. kullanilan
 * frame sayisinin boyutu hesaplar ve geri dondurulur. geri donen
 * boyut KiB seklindedir.
 */
uint32_t use_memory_size(void){

	return mp_info.alloc_memf * FRAME_SIZE_KIB;	
	
}

/*
 * calc_heap_size,
 */
static void calc_heap_size(void){
	
	uint32_t total_mem = total_memory_size();
	uint32_t n = total_mem / 8192;
	
	#define byte_to_kib(x)	(x / 1024)

	if(n >= 8 && n < 16) /* 64 MiB - 128 MiB */
		heap_info.size = 16777216; /* 16 MiB */
	else if(n >= 16 && n < 32) /* 128 MiB - 256 MiB */
		heap_info.size = 33554432; /* 32 MiB */
	else if(n >= 32 && n < 64) /* 256 MiB - 512 MiB */
		heap_info.size = 67108864; /* 64 MiB */
	else if(n >= 64 && n < 128) /* 512 MiB - 1024 MiB */
		heap_info.size = 134217728; /* 128 MiB */
	else if(n >= 128 && n < 256) /* 1024 MiB - 2048 MiB */
		heap_info.size = 268435456; /* 256 MiB */
	else /* 2048 MiB > */
		heap_info.size = 536870912; /* 512 MiB */

	if(n < 8)
		die("The amount of memory is critically low for heap.");
	else if(n < 64)
		debug_print(KERN_INFO,"The amount of memory is normal level for heap. Heap size : %u KiB",
										byte_to_kib(heap_info.size));
	else
		debug_print(KERN_INFO,"The amount of memory is very good level for heap. Heap size : %u KiB",
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

	/* 
	 * eger ulasilacak frame'in index'i toplam frame sayisindan
	 * buyukse ugrasmaya gerek yok.
	 */
	if((index * 32 + offset) >= mp_info.nframe){
		debug_print(KERN_CRITIC,"Not found the frame, frame index = %u",index * 32 + offset);
		return;
	}
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

	/* 
	 * eger ulasilacak frame'in index'i toplam frame sayisindan
	 * buyukse ugrasmaya gerek yok.
	 */
	if((index * 32 + offset) >= mp_info.nframe){
		debug_print(KERN_CRITIC,"Not found the frame, frame index = %u",index * 32 + offset);
		return;
	}
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
	if(mp_info.remaining > 0)
		max_index = mp_info.aframe_size / 4 + 1;
	else
		max_index = mp_info.aframe_size / 4;

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
   @param rw : okuma/yazma izni ( false ise sadece okuma , true ise hem okuma hem yazma)
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
 * @param make : eger tablo index'i sayfalama dizininde bulunuyor ise
 *		 o bulunan tablo'daki belirtilen sayfa adresi dondurulur. eger
 *		 make true ise diger if yani ilk sayfa olusturma asamasina
 *		 giriliyor ve sayfanin adresi donduruluyor. eger hicbiri degilse
 *		 NULL olarak geri donduruluyor.
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
 * change_page_dir,sayfa dizini degistirir.
 *
 * @param new_dir : yeni sayfa dizini adresi.
 */
static void change_page_dir(page_dir_t *new_dir){

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
	uint32_t alloc_frame_byte = ((mp_info->nframe / BITS_PER_BYTE) / 32) * 32;
	mp_info->nframe = alloc_frame_byte * 8;
#else
	mp_info->nframe = mem_size / FRAME_SIZE_KIB;
	uint32_t alloc_frame_byte = mp_info->nframe / BITS_PER_BYTE;
	uint8_t alloc_frame_remaining = mp_info->nframe % BITS_PER_BYTE; 

	if(alloc_frame_remaining)
		alloc_frame_byte++;

	if(mp_info->nframe % 32 > 0)
		mp_info->remaining = mp_info->nframe % 32;
	else
		mp_info->remaining = -1;
#endif

	mp_info->aframe_size = alloc_frame_byte;
	mp_info->frame_map = (uint32_t *)kmalloc(alloc_frame_byte);
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
	debug_print(KERN_DUMP,"Frame map address is \033[1;37m%p\033[0m",mp_info->frame_map);
	debug_print(KERN_DUMP,"total memory size : %u KiB, total frame : %u",mp_info->total_mem,mp_info->nframe);
	debug_print(KERN_DUMP,"frame map size : %u Byte, available memory size : %u KiB",mp_info->aframe_size,
										    	 mp_info->nframe * FRAME_SIZE_KIB);

	debug_print(KERN_DUMP,"unused memory size : %u KiB",mp_info->total_mem - mp_info->nframe * FRAME_SIZE_KIB);

}

/*
 * paging_final,sayfalama icin son ayarlari yapar.
 */
void paging_final(void){

	debug_print(KERN_INFO,"Initializing the memory mapping.");
	
	/* ilk 1 MiB'i mapping islemi uygulayalim */
	debug_print(KERN_DUMP,"Memory mapping size : %u KiB",use_memory_size());
	debug_print(KERN_DUMP,"(0x00000000 - 0x00100000) mapping. %u KiB",0x00100000 / 1024);	
	for(uint32_t i = 0; i < 0x100000; i += FRAME_SIZE_BYTE)
		dma_frame(get_page(i,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS,i);

	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p\033[0m, last_addr + 0x4000 : \033[1;37m%p",
												last_addr,
												last_addr + 0x4000);

	uint32_t a_allocmem  = 0; /* kullanilabilir ayrilmis bellek,asagida
				  * ekstradan 16 KiB ayriliyor dikkat !.
				  * last_addr'in son kullanilan adres
				  * oldugunu unutmayin.
				  */
	/* 0x100000 adresinden, last_addr + 0x4000(16 KiB) adrese kadar bir mapping daha yapiyoruz */
	debug_print(KERN_DUMP,"(0x00100000 - %p) mapping. %u KiB ",last_addr + 0x4000,
								   (last_addr + 0x4000 - 0x100000)/1024); 
	for(uint32_t j = 0x100000; j < last_addr + 0x4000; j += FRAME_SIZE_BYTE)
		dma_frame(get_page(j,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS,j);
	a_allocmem += 0x4000;
	debug_print(KERN_DUMP,"--> available allocation memory size = %u Byte / %u KiB",a_allocmem,
											a_allocmem/1024);

	/* vga text-mode video bellegini remapping isleminden geciriyoruz */
	debug_print(KERN_DUMP,"(0xB8000-0xC0000) remapping vga text-mode dma. %u KiB",(0xC0000-0xB8000)/1024);
	for(uint32_t k = 0xB8000; k < 0xC0000; k += FRAME_SIZE_BYTE)
		dma_frame(get_page(k,false,kernel_dir),PAGE_RWRITE,PAGE_USER_ACCESS,k);
	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p\033[0m",last_addr);


	uint32_t tmp_heap_start = KHEAP_INIT;	/* 0x00800000,ekstra yada temel diyecegimiz heap'in baslangic adresi */
	/* eger bizim bu adresimiz last_addr + 0x4000'den kucuk ise bu adresi 1 MiB ileri tasiyoruz */
	if (tmp_heap_start <= last_addr + 0x4000){
		debug_print(KERN_ERROR, "Kernel heap init address error!");
		debug_print(KERN_DUMP,"tmp_heap_start : %p, last_addr + 0x4000 : %p", tmp_heap_start,last_addr + 0x4000);
		tmp_heap_start = last_addr + 0x100000;
		debug_print(KERN_DUMP,"new tmp_heap_start : %p",tmp_heap_start);
	}
	heap_info.alloc_point = tmp_heap_start;

	/* 
	 * ----------------------------------------------------------------------------------------------
	 * ekstra heap'in adresine kadar olan tum bellek alanini bizim su anlik temel heap
	 * alanimizi olusturuyoruz. eger kmallocla tahsisi yaparak bu adrese gelirsek bu
	 * ekstra heap bolgesine giriyoruz. ve artik buradan tahsis islemini gerceklestiriyoruz.
	 * eger bu ek artik bu bolgede yetmeze o zaman sikinti olusur :(. bu bolgenin bellek
	 * miktarini sahip oldugumuz bellek miktarina gore ayarliyoruz. sistemimizin en az 64 MiB
         * bellek kullanilacak  sekilde ayarlandi. 64 MiB bellekte 16 MiB'lik bir heap ayarliyoruz.
	 * daha fazla bilgi icin calc_heap_size'i inceleyin.
	 * ----------------------------------------------------------------------------------------------
	 */

	/*
	 * en son last_addr + 0x4000 adresine kadar mapping islemini yaptigimizi hatirlayin.
	 * burdan temel yada ekstra olarak adlandirdigimiz heap alanina kadar mapping islemi
	 * yapiyoruz. last_addr'nin degismedigine dikkat edin!
	 */
	debug_print(KERN_DUMP,"(%p - %p) mapping. %u Byte / %u KiB",last_addr + 0x4000,
								   tmp_heap_start,
								   tmp_heap_start - (last_addr+0x4000),
								   (tmp_heap_start - (last_addr+0x4000))/1024); 
	a_allocmem += (tmp_heap_start - (last_addr+0x4000));
	debug_print(KERN_DUMP,"--> available allocation memory size = %u Byte / %u KiB",a_allocmem,
											a_allocmem/1024);
	for (uint32_t i = last_addr + 0x4000; i < tmp_heap_start  ; i += FRAME_SIZE_BYTE)
		alloc_frame(get_page(i,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS);
	/* 
	 * sayfa tablolarinin bellekten tahsis edilmesinden dolayi last_addr'in arttigini
	 * farkedebilirsiniz.
	 */
	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p\033[0m",last_addr);

	
	/*  heap genislemesi icin. sayfalari sadece sayfalari ayarladigimiza dikkat edin. */
	heap_info.end_point = tmp_heap_start + heap_info.size;	
	debug_print(KERN_DUMP,"(%p - %p) preallocation. %u Byte / %u KiB",heap_info.alloc_point,
								   heap_info.end_point,
								   heap_info.size,
								   heap_info.size/1024); 
	for (uint32_t i = heap_info.alloc_point ; i < heap_info.end_point ; i += FRAME_SIZE_BYTE)
		get_page(i,true, kernel_dir);
	debug_print(KERN_DUMP,"last_addr(end) : \033[1;37m%p\033[0m",last_addr);

	debug_print(KERN_DUMP,"Memory mapping size : %u KiB",use_memory_size());
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
	
	kernel_dir = (page_dir_t *)kmalloc_align(sizeof(page_dir_t));
	memset(kernel_dir,0,sizeof(page_dir_t));
	current_dir = kernel_dir;
	
}

/*
 * sbrk
 *
 * @param inc :
 */
void *sbrk(uint32_t inc){
	
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
