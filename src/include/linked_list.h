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

#ifndef __UNIQ_LINKED_LIST_H__
#define __UNIQ_LINKED_LIST_H__

#include <uniq/types.h>
#include <compiler.h>

#define LINKED_LIST_SIGNATURE		0xEE384ACF

typedef struct node_t{
	void *item;			/* item(isaretci olduguna dikkat edin!) */
	struct node_t *prev; 		/* onceki dugum */
	struct node_t *next;		/* sonraki dugum */
	void *link_list;		/* dugumun hangi listeye bagli oldugu */
}node_t;

typedef struct linked_list_t{
	uint32_t signature;		/* liste imzasi */
	uint32_t size;			/* liste uzunlugu */
	node_t *first_node;		/* ilk dugum */
	node_t *last_node;		/* son dugum */
}linked_list_t;

int32_t linked_list_get_index(linked_list_t *linked_list,void *search);
node_t *linked_list_search(linked_list_t *linked_list,void *search);
void linked_list_remove(linked_list_t *linked_list,uint32_t node_index);
node_t *linked_list_pop(linked_list_t *linked_list);
void linked_list_unlink(linked_list_t *linked_list,node_t *node);
node_t *linked_list_push(linked_list_t *linked_list,void *item);
void linked_list_link(linked_list_t *linked_list,node_t *new_node);
void linked_list_clear(linked_list_t *linked_list);
void linked_list_free(linked_list_t *linked_list);
void linked_list_destroy(linked_list_t *linked_list);
linked_list_t *linked_list_merge(linked_list_t *dest,linked_list_t *src);
linked_list_t *linked_list_clone(linked_list_t *linked_list);
linked_list_t *linked_list_create(void);
void __linked_list_test(void);
node_t *linked_list_push_next(linked_list_t *linked_list,void *item,node_t *prev_node);
node_t *linked_list_push_prev(linked_list_t *linked_list,void *item,node_t *next_node);
void linked_list_link_prev(linked_list_t *linked_list,node_t *node,node_t *next_node);
void linked_list_link_next(linked_list_t *linked_list,node_t *node,node_t *prev_node);

#endif /* __UNIQ_LINKED_LIST_H__ */
