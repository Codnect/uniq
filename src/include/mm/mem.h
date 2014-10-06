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

#ifndef __UNIQ_MEM_H__
#define __UNIQ_MEM_H__

#include <uniq/types.h>

typedef struct{
	uint32_t present    : 1;   /* sayfa bellekte ise set edilir */
   	uint32_t rw         : 1;   /* 0 ise sadece okuma, 1 ise okuma/yazma */
   	uint32_t user       : 1;   /* 0 ise kernel, 1 ise user moddan da erisim olur */
   	uint32_t accessed   : 1;   /* her erisimde set edilir */
   	uint32_t dirty      : 1;   /* her yazmada set edilir */
   	uint32_t reserved   : 7;   /* reserve edilmis */
   	uint32_t frame      : 20;  /* frame adresi */
}page_t;

#define PAGE_MAX_LIMIT		1024	/* sayfa ve tablo maksimum girdi sayisi */
#define PAGE_MAX		PAGE_MAX_LIMIT
#define PAGE_TABLE_MAX		1024
#define PAGE_KERNEL_ACCESS	0x0
#define PAGE_PRESENT		0x1
/* user icin flaglar */
#define PAGE_USER_ACCESS	0x1
#define PAGE_RONLY		0x0
#define PAGE_RWRITE		0x1


typedef struct{
	page_t pages[PAGE_MAX_LIMIT];
}page_table_t;

typedef struct{				

	/* sayfa tablolari isaretci dizisi */
	page_table_t *tables[PAGE_MAX_LIMIT];
	/*
	 * sayfa tablolarinin sayfa dizini girdisi halleri.
	 * kisaca sayfa dizin tablosudur. gdt.c'yi incelediyseniz 
	 * eger yabanci olmayacaksiniz. son 20 bit sayfa tablolarinin 
	 * kacinci 4 KiB'lik adreste oldugunu belirtir.diger bitler
	 * ise flaglardir.bu tablonun adresini cr3 kaydedicesine
	 * atacagiz.
	 */
	uint32_t physical_tables[PAGE_MAX_LIMIT];
	/* physical_tables adresi */
	uint32_t physical_addr;
	
}page_dir_t;

#define FRAME_SIZE_BYTE		4096		/* 4096 Byte - 4 KiB - 0x1000 */
#define FRAME_SIZE_KIB		4		/* 4 KiB */
#define MAX_LIMIT		0xFFFFFFFF	/* 4 GiB */

#define FRAME_INDEX_BIT(x)	(x / 32)
#define FRAME_OFFSET_BIT(x)	(x % 32)

void free_frame(page_t *page);
page_t *get_page(uint32_t addr,bool make,page_dir_t *dir);
void change_page_dir(page_dir_t *new_dir);
void alloc_frame(page_t *page,bool rw,bool user);
void dma_frame(page_t *page,bool rw,bool user,uintptr_t addr);
uint32_t use_memory_size(void);
uint32_t total_memory_size(void);

#endif /* __UNIQ_MEM_H__ */
