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
#include <uniq/proc.h>
#include <tree.h>
#include <list.h>


process_t *current_process = NULL;			/* calistirilan surec */

list_t *process_list;					/* surec listesi */
list_t *process_ready_queue;				/* hazir olan surec listesi */		
list_t *process_sleep_queue;				/* beklemeye alinmis surec listesi */
tree_t *process_tree;					/* surec agaci (parent-child) */

char *process_default_name = "[unnamed process]";	/* varsayilan surec ismi */

/*
 * process_init,
 */
void process_init(void){

	process_tree = tree_create();
	process_list = list_create();
	process_ready_queue = list_create();
	process_sleep_queue = list_create();

}
 
MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
