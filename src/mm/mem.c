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

uint32_t *frame_map;				/* frame map */
uint32_t nframe;				/* frame sayisi */
extern uint32_t last_addr;			/* linker - end */
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
	
	return nframe * FRAME_SIZE_KIB;

}

/*
 * use_memory_size, kullanilan bellek boyutunu dondurur. kullanilan
 * frame sayisinin boyutu hesaplar ve geri dondurulur. geri donen
 * boyut KiB seklindedir.
 */
uint32_t use_memory_size(void){
	
	uint32_t use_mem = 0;
	uint32_t index,offset;
	
	for (index = 0; index < (nframe / 32); index++){
		
		for (offset = 0; offset < 32; offset++){
			uint32_t cntrl = 0x1 << offset;
			if (frame_map[index] & cntrl) {
				use_mem++;
			}
		}
		
	}
	
	return use_mem * FRAME_SIZE_KIB;	
	
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
	frame_map[index] |= (0x1 << offset); 

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
	frame_map[index] &= ~(0x1 << offset);

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
	
	return (frame_map[index] & (0x1 << offset));

}

/*
 * find_free_frame, ilk bos frame'i bulur ve index numarasini
 * dondurur.
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
	uint32_t index = find_free_frame();

	if(index == MAX_LIMIT)
		printf("no free frames!");
	
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
	debug_print(KERN_EMERG,"Page Fault ! \033[1;37m%p\033[0m",fault_addr);
	
	char err_desc[64] = "Error description :";
	if(!(regs->err_code & PF_PRESENT))
		strcat(err_desc," present ");
	if(regs->err_code & PF_WOP)
		strcat(err_desc," read-only ");
	if(regs->err_code & PF_USRMODE)
		strcat(err_desc," user mode ");
	if(regs->err_code & PF_RESERVED)
		strcat(err_desc," reserved ");
	if(regs->err_code & PF_INSTRUCTION);

	debug_print(KERN_DUMP," %s",err_desc);
	halt_system();
 
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
 * paging_init, sayfalamayi baslatir.
 *
 * @param mem_size : bellek boyutu ( multiboot yapisindan aliyoruz)
 */
void paging_init(uint32_t mem_size){
	
	debug_print(KERN_INFO,"Initializing the paging.");
	nframe = mem_size / FRAME_SIZE_KIB;
	frame_map = (uint32_t *)kmalloc(nframe / 8);
 	memset(frame_map,0,nframe / 8);
	
	kernel_dir = (page_dir_t *)kmalloc_align(sizeof(page_dir_t));
	memset(kernel_dir,0,sizeof(page_dir_t));
	current_dir = kernel_dir;

	uint32_t i = 0;
	while(i < last_addr){
		alloc_frame(get_page(i,true,kernel_dir),PAGE_RONLY,PAGE_KERNEL_ACCESS);
		i += FRAME_SIZE_BYTE;
	}

	isr_add_handler(PAGE_FAULT_INT,page_fault_handler);
	change_page_dir(kernel_dir);

}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
