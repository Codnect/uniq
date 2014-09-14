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
 * tree_node_free,
 *
 * @param node :
 */
void tree_node_free(tree_node_t *node){



}

/*
 * tree_free,
 *
 * @param tree :
 */
void tree_free(tree_t *tree){



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
 * tree_child_count,
 *
 * @param node :
 */
uint32_t tree_child_count(tree_node_t *node){

	if(!node || !node->child)
		return 0;

	uint32_t count = node->child->size;
	node_t *child_list_node = node->child->first_node;
	
	for(;child_list_node; child_list_node = child_list_node->next)
		count += tree_child_count((tree_node_t*)child_list_node->item);

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
	node_t *child_list_node = start_node->child->first_node;

	for(;child_list_node;child_list_node = child_list_node->next){
		
		if(child_list_node->item == search)
			return start_node;

		node = tree_node_search_parent((tree_node_t*)child_list_node->item,search);

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

	if(!tree)
		return NULL;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	
	if(!tree->root_node || !search)
		return NULL;

	return tree_node_search_parent(tree->root_node,search);

}

/*
 * tree_node_create,
 *
 * @param item :
 */
tree_node_t *tree_node_create(void *item){

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
	
	if(!tree)
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
	
	if(!tree)
		return;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	if(!parent || !child)
		return;

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
	
	if(!tree)
		return NULL;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	if(!parent)
		return NULL;

	tree_node_t *child = tree_node_create(item);
	tree_push_child_node(tree,child,parent);
	
	return child;
	
}

/*
 * tree_node_remove_parent
 *
 * @param tree :
 * @param node :
 * @param parent :
 */
void tree_node_remove_parent(tree_t *tree,tree_node_t *node,tree_node_t *parent){

	if(!tree)
		return;
	
	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	
	if(!parent || !node)
		return;

}

/*
 * __tree_test
 */
void __tree_test(void){
	
	debug_print(KERN_INFO,"tree test...");
	tree_t *tree = tree_create();
	tree_set_root_node(tree,NULL);

	debug_print(KERN_DUMP,"tree addr : %p, root node addr : %p, node count : %u",tree,
										     tree->root_node,
										     tree_total_node(tree));
	debug_print(KERN_DUMP,"root; child : %p, item : %p, parent : %p",tree->root_node->child,
									 tree->root_node->item,
									 tree->root_node->parent);	
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
