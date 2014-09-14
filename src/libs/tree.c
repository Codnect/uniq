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
#include <list.h>
#include <tree.h>


/*
 * tree_create,
 */
tree_t *tree_create(void){

	tree_t *new_tree = malloc(sizeof(tree_t));
	new_tree->root_node = NULL;
	new_tree->node_count = 0;
	new_tree->signature = TREE_SIGNATURE;
	
	return new_tree;

}

/*
 * tree_total_node,
 *
 * @param tree :
 */
uint32_t tree_total_node(tree_t *tree){

	if(!tree || !tree->root_node)
		return 0;
	
	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	
	return tree->node_count;

}

/*
 * tree_find_child_count,
 *
 * @param node :
 */
uint32_t tree_find_child_count(tree_node_t *node){

	if(!node || !node->child)
		return 0;

	uint32_t count = node->child->size;
	node_t *child = node->child->first_node;
	
	for(;child; child = child->next)
		count += tree_find_child_count((tree_node_t*)child->item);

	return count;

}


/*
 * tree_node_search_parent,
 *
 * @param node :
 * @param search :
 */
tree_node_t *tree_node_search_parent(tree_node_t *start_node,tree_node_t *search){

	tree_node_t *node = NULL;
	node_t *child = start_node->child->first_node;

	for(;child;child = child->next){
		
		if(child->item == search)
			return start_node;

		node = tree_node_search_parent((tree_node_t*)child->item,search);

		if(node)
			break;

	}

	return node;

}

/*
 * tree_search_parent,
 *
 * @param tree :
 * @param search :
 */
tree_node_t *tree_search_parent(tree_t *tree, tree_node_t *search){

	if(!tree->root_node || !search)
		return NULL;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	
	return tree_node_search_parent(tree->root_node,search);

}

/*
 * tree_node_create,
 *
 * @param item :
 */
tree_node_t *tree_node_create(void *item){

	if(!item)
		return NULL;

	tree_node_t *new_node = malloc(sizeof(tree_node_t));
	new_node->parent = NULL;
	new_node->child = list_create();
	new_node->item  = item;

	return new_node;

}

/*
 * tree_set_root_branch,
 *
 * @param tree :
 * @param item :
 */
tree_node_t *tree_set_root_node(tree_t *tree,void *item){
	
	if(!tree || !item)
		return NULL;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	tree_node_t *root = tree_node_create(item);
	tree->node_count = 1;
	tree->root_node = root;

	return root;

}

/*
 * tree_push_child_node,
 *
 * @param tree :
 * @param child :
 * @param parent :
 */
void tree_push_child_node(tree_t *tree,tree_node_t *child,tree_node_t *parent){
	
	if(!tree || !child || !parent)
		return;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	tree->node_count++;
	child->parent = parent;
	list_push(parent->child,child);
	
}

/*
 * tree_push_child,
 *
 * @param tree :
 * @param item :
 * @param parent :
 */
tree_node_t *tree_push_child(tree_t *tree,void *item,tree_node_t *parent){
	
	if(!tree || !item || !parent)
		return NULL;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	tree_node_t *child = tree_node_create(item);
	tree_push_child_node(tree,child,parent);
	
	return child;
	
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
