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
#include <uniq/task.h>
#include <mm/mem.h>
#include <mm/heap.h>
#include <uniq/kernel.h>
#include <string.h>

uint32_t next_pid = 0;

extern page_dir_t *kernel_dir;
extern page_dir_t *current_dir;

/*
 * goto_user_mode, kernel moddan kullanici moduna gecerir.
 */
void goto_user_mode(void){

	debug_print(KERN_DUMP,"Hello user world :)");
 
}

/*
 * get_pid, su an gecerli olan process'in id'sini
 * dondurur.
 */
uint32_t get_pid(void){

	return 0;

}

/*
 * page_table_clone, sayfa tablosunu klonlar.
 *
 * @param src_table :
 * @param physical_addr :
 */
page_table_t *page_table_clone(page_table_t *src_table,uint32_t *physical_addr){

	page_table_t *clone_table = (page_table_t*)kmalloc_aphysic(sizeof(page_table_t),physical_addr);
	memset(clone_table,0,sizeof(page_table_t));

	for(uint32_t i = 0;i < PAGE_MAX;i++){

		if(!src_table->pages[i].frame)
			continue;

		/*
		 * klon icin yeni frame
		 */
		alloc_frame(&clone_table->pages[i],0,0);

		/*
 		 * flaglari ayarliyoruz
		 */	
		clone_table->pages[i].present 	= src_table->pages[i].present;
		clone_table->pages[i].rw 	= src_table->pages[i].rw;
		clone_table->pages[i].user 	= src_table->pages[i].user;
		clone_table->pages[i].accessed 	= src_table->pages[i].accessed;
		clone_table->pages[i].dirty 	= src_table->pages[i].dirty;

		copy_page_phys(src_table->pages[i].frame * FRAME_SIZE_BYTE,clone_table->pages[i].frame * FRAME_SIZE_BYTE);
	
	}

	return clone_table;

}

/*
 * page_directory_clone, sayfa dizinini kopyalar.
 *
 * @param dir : klonlanicak sayfa dizini adresi
 */
page_dir_t *page_directory_clone(page_dir_t *src_directory){

	uint32_t physical;
	page_dir_t *clone_directory = (page_dir_t*)kmalloc_aphysic(sizeof(page_dir_t),&physical);
	memset(clone_directory,0,sizeof(page_dir_t));
	clone_directory->physical_addr = physical;

	for(uint32_t i = 0; i < PAGE_TABLE_MAX;i++){

		if(!src_directory->tables[i] || (uint32_t)src_directory->tables[i] == MAX_LIMIT)
			continue;

		if(kernel_dir->tables[i] == src_directory->tables[i]){

			clone_directory->tables[i] = src_directory->tables[i];
			clone_directory->physical_tables[i] = src_directory->physical_tables[i];

		}
		else{

			uint32_t physic;
            		clone_directory->tables[i] = page_table_clone(src_directory->tables[i],&physic);
            		clone_directory->physical_tables[i] = physic | 0x7;

		}

	}

	return clone_directory;

}

/*
 * page_directory_free, belirtilen sayfa dizini bosa cikarir.(free)
 *
 * @param directory : sayfa dizini adresi(isaretcisi)
 */
void *page_directory_free(page_dir_t *directory){


}

/*
 * switch_task,
 */
void switch_task(void){


}

/*
 * switch_next_task,
 */
void switch_next_task(void){


}

/*
* multitasking_init,
*/
void multitasking_init(void){

	debug_print(KERN_INFO,"Initializing the multitasking...");

}
	
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
