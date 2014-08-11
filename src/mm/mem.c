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

#define PAGE_FAULT_INT		14
#define PF_PRESENT		0x1
#define PF_WOP			0x2
#define PF_USRMODE		0x4
#define PF_RESERVED		0x8
#define PF_INSTRUCTION		0x10
#define BITS_PER_BYTE		8
#define MEM_NORMAL_USE

typedef struct{
	uint32_t total_mem;
	uint32_t nframe;
	uint32_t *frame_map;
	uint32_t aframe_size;
	int8_t remaining;
	uint32_t alloc_memf;
}mp_info_t;

static mp_info_t mp_info;
extern uint32_t last_addr;
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
	
	return mp_info.nframe * FRAME_SIZE_KIB;

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
 * enable_paging, sayfalamayi aktif hale getirir.
 */
void enable_paging(void){

	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 |= 0x80000000;
	__asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

}

/*
 * disable_paging, sayfalamayi devre disi birakir.
 */
void disable_paging(void){

	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
	cr0 &= 0x7fffffff;
	__asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
	
}


/*
 * set_frame, frame_map'te frame'in kullanilmaya basladigina
 * dair frame ait bit '1' seklinde set edilir.
 *
 * @param frame_addr : frame adresi
 */
void set_frame(uintptr_t frame_addr){

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
	mp_info.alloc_memf++;
	
}

/*
 * remove_frame, frame_map'te frame'in kaldirildigina dair
 * frame ait bit '0' haline getirilir.
 *
 * @param frame_addr : frame adresi
 */
void remove_frame(uintptr_t frame_addr){
	
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
	mp_info.alloc_memf--;
	
}

/*
 * cntrl_frame, frame'in frame_map'te olup olmadigini kontrol eder.
 *
 * @param frame_addr : frame adresi
 */
uint32_t cntrl_frame(uintptr_t frame_addr){
	
	uint32_t frame  = frame_addr/FRAME_SIZE_BYTE;
	uint32_t index  = FRAME_INDEX_BIT(frame); 
	uint32_t offset = FRAME_OFFSET_BIT(frame);
	
	return (mp_info.frame_map[index] & (0x1 << offset));
	
}

/*
 * find_free_frame, ilk bos frame'i bulur ve index numarasini
 * dondurur.
 */
uint32_t find_free_frame(void){

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

	if(page->frame)
		return;

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
 * dma_frame
 *
 * @param page :
 * @param rw :
 * @param user :
 * @param addr :
 */
void dma_frame(page_t *page,bool rw,bool user,uintptr_t addr){

	page->present = PAGE_PRESENT;
	page->rw      = (rw) ? PAGE_RWRITE : PAGE_RONLY;
	page->user    = (user) ? PAGE_USER_ACCESS : PAGE_KERNEL_ACCESS;
	page->frame   = addr / FRAME_SIZE_BYTE;

	if (addr > mp_info.nframe * FRAME_SIZE_BYTE)
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
 * change_page_dir,
 *
 * @param new_dir :
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
 * @param mem_size :
 */
static void set_mp_info(mp_info_t *mp_info,uint32_t mem_size){

	/* mp_info adresi bos ise */
	if(!mp_info)
		die("mp_info address is not found");

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
		die("mp_info address is not found");
	
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
 * paging_final
 */
void paging_final(void){


}

/*
 * paging_init, sayfalamayi baslatir.
 *
 * @param mem_size : bellek boyutu ( multiboot yapisindan aliyoruz)
 */
void paging_init(uint32_t mem_size){

	if(!mem_size)
		die("Memory size is not found.");
		
	debug_print(KERN_INFO,"Initializing the paging.");
	set_mp_info(&mp_info,mem_size);
	dump_mp_info(&mp_info);
	
	kernel_dir = (page_dir_t *)kmalloc_align(sizeof(page_dir_t));
	memset(kernel_dir,0,sizeof(page_dir_t));
	current_dir = kernel_dir;

	uint32_t i = 0;
	while(i < (mp_info.nframe)*FRAME_SIZE_BYTE){
		alloc_frame(get_page(i,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS);
		i += FRAME_SIZE_BYTE;		
	}
	debug_print(KERN_DUMP,"allocation size : %u KiB",use_memory_size());
	isr_add_handler(PAGE_FAULT_INT,page_fault_handler);
	change_page_dir(kernel_dir);
	
}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
