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

#ifndef __UNIQ_TREE_H__
#define __UNIQ_TREE_H__

#include <uniq/types.h>
#include <list.h>

#define TREE_SIGNATURE			0xCCD499AA

typedef struct _tree_node_t{
	struct _tree_node_t *parent;
	list_t *child;
	void *item;	
}tree_node_t;

typedef struct{
	tree_node_t *root_node;
	uint32_t signature;
	uint32_t node_count;	
}tree_t;

tree_t *tree_create(void);
void tree_node_free(tree_node_t *node);
void tree_free(tree_t *tree);
uint32_t tree_total_node(tree_t *tree);
uint32_t tree_child_count(tree_node_t *node);
tree_node_t *tree_node_search_parent(tree_node_t *start_node,tree_node_t *search);
tree_node_t *tree_search_parent(tree_t *tree, tree_node_t *search);
tree_node_t *tree_node_create(void *item);
tree_node_t *tree_set_root_node(tree_t *tree,void *item);
void tree_push_child_node(tree_t *tree,tree_node_t *child,tree_node_t *parent);
tree_node_t *tree_push_child(tree_t *tree,void *item,tree_node_t *parent);
void __tree_test(void);

#endif /* __UNIQ_TREE_H__ */
