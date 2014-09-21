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
 * goto_user_mode
 */
void goto_user_mode(void){
 
 
}

/*
 * get_pid,
 */
uint32_t get_pid(void){

	return 0;

}

/*
 * page_table_clone,
 *
 * @param src_table :
 * @param physical_addr :
 */
page_table_t *page_table_clone(page_table_t *src_table,uint32_t *physical_addr){

	page_table_t *clone = (page_table_t*)kmalloc_aphysic(sizeof(page_table_t),physical_addr);
	memset(clone,0,sizeof(page_table_t));

	for(uint32_t i = 0;i < 1024;i++){


	}

	return clone;

}

/*
 * page_directory_clone,
 *
 * @param dir :
 */
page_dir_t *page_directory_clone(page_dir_t *src_directory){

	uint32_t physical;
	page_dir_t *clone = (page_dir_t*)kmalloc_aphysic(sizeof(page_dir_t),&physical);
	memset(clone,0,sizeof(page_dir_t));
	clone->physical_addr = physical;

	for(uint32_t i = 0; i < PAGE_MAX_LIMIT;i++){

		if(!src_directory->tables[i] || (uint32_t)src_directory->tables[i] == MAX_LIMIT)
			continue;

		if(kernel_dir->tables[i] == src_directory->tables[i]){

			clone->tables[i] = src_directory->tables[i];
			clone->physical_tables[i] = src_directory->physical_tables[i];

		}
		else{

			uint32_t physic;
            		clone->tables[i] = page_table_clone(src_directory->tables[i],&physic);
            		clone->physical_tables[i] = physic | 0x7;

		}

	}
	
	return clone;

}

/*
 * page_directory_free,
 *
 * @param directory :
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
 
 
}
	
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
