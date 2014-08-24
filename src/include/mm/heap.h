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

#ifndef __UNIQ_HEAP_H__
#define __UNIQ_HEAP_H__

#include <uniq/types.h>
#include <compiler.h>

uint32_t kmalloc_align(uint32_t size);
uint32_t kmalloc_physic(uint32_t size,uint32_t *physic_addr);
uint32_t kmalloc_aphysic(uint32_t size,uint32_t *physic_addr);
uint32_t kmalloc(uint32_t size);

#define KHEAP_MAGIC		0xBAF01CDE
#define KHEAP_INIT		0x00800000

typedef struct{
	uint32_t alloc_point;
	uint32_t end_point;
	uint32_t current_end;
	uint32_t size;
}heap_info_t;

/*
 * small block
 */
typedef struct{
	struct heap_blk_header_t *next;		/* sonraki block header'nin adresi */
	void *point;				/* header'in bos blok parcasinin bellek adresi.
						 * bu adres tahsis edilmeden once kendisinden sonraki 
						 * blok parcasini adresinide barindirir. adresini
						 * tuttugu blok parcasida baska blok parcasinin
						 * adresini barindirarak bir dugum olustururlar.
						 */
	uint32_t size;				/* header blok boyutu */
	uint32_t magic;				/* header block magic */
}heap_blk_header_t;

typedef struct{
	heap_blk_header_t *first;		/* ilk block header'nin adresi*/
}heap_blk_t;

/*
 * big block
 */
typedef struct{
	struct heap_big_blk_header_t *prev;
	struct heap_big_blk_header_t *next;
	void *point;
	uint32_t size;
	uint32_t magic;
}heap_big_blk_header_t;


#endif /* __UNIQ_HEAP_H__ */
