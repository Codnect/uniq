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

#ifndef __UNIQ_TASK_H__
#define __UNIQ_TASK_H__

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

	/* page_table isaretci dizisi */
	page_table_t *tables[PAGE_MAX_LIMIT];
	/* tables fiziksel adres isaretcisi */
	uint32_t tables_physic[PAGE_MAX_LIMIT];
	/* tables_physic adresi */
	uint32_t physic_addr;
	
}page_dir_t;

#endif /* __UNIQ_TASK_H__ */
