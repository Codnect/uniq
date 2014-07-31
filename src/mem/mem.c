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

/*
 * kmalloc_center
 */
uintptr kmalloc_center(size_t size,bool align,uintptr_t *physic_addr){
 
 return 0;
}

/*
 * kmalloc_aligned
 */
uintptr_t kmalloc_aligned(size_t size){

 return 0;
}

/*
 * kmalloc_physic
 */
uintptr_t kmalloc_physic(size_t size,uintptr_t *physic_addr){
 
 return 0;
}

/*
 * kmalloc_aphysic
 */
uintptr_t kmalloc_aphysic(size_t size,uintptr_t *physic_addr){
 
 return 0;
}

/*
 * kmalloc
 */
uintptr_t kmalloc(size_t size){
 
 return 0;
}
 
/*
 * page_fault
 */
void page_fault(struct registers_t *regs){
 
 
}

/*
 * paging_init
 */
void paging_init(void){
 
 
}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
