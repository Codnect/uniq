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

#include <uniq/task.h>

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
