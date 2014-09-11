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
#include <tree.h>

/*
 * tree_create,
 */
tree_t *tree_create(void){

	tree_t *new_tree = malloc(sizeof(tree_t));
	new_tree->root = NULL;
	new_tree->nbranch = 0;
	new_tree->signature = TREE_SIGNATURE;
	
	return new_tree;

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
