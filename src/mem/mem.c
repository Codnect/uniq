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

#define FRAME_SIZE 4

uint32_t *frames;
uint32_t total_frames;

/*
 * set_frame
 */
void set_frame(uintptr_t frame_addr){
 
 
}

/*
 * clear_frame
 */
void clear_frame(uintptr_t frame_addr){
 
 
}

/*
 * cntrl_frame
 */
uint32_t cntrl_frame(uintptr_t frame_addr){
 
  return 0;
  
}


/*
 * calc_total_mem
 */
uint32_t calc_total_mem(void){
 
  return total_frames * FRAME_SIZE;
 
}

/*
 * kmalloc_center
 */
uintptr_t kmalloc_center(size_t size,bool align,uintptr_t *physic_addr){
 
  return 0;
 
}

/*
 * kmalloc_aligned
 */
uintptr_t kmalloc_aligned(size_t size){

  return kmalloc_center(size,true,NULL);
 
}

/*
 * kmalloc_physic
 */
uintptr_t kmalloc_physic(size_t size,uintptr_t *physic_addr){
 
  return kmalloc_center(size,false,physic_addr);
 
}

/*
 * kmalloc_aphysic
 */
uintptr_t kmalloc_aphysic(size_t size,uintptr_t *physic_addr){
 
  return kmalloc_center(size,true,physic_addr);
 
}

/*
 * kmalloc
 */
uintptr_t kmalloc(size_t size){
 
  return kmalloc_center(size,false,NULL);
 
}
 
/*
 * page_fault
 */
void page_fault(struct registers_t *regs){
 
 
}

/*
 * paging_init
 */
void paging_init(uint32_t mem_size){
 
  total_frames = mem_size / FRAME_SIZE; 
 
}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
