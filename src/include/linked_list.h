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
	void *item;			/* item */
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

node_t *linked_list_push(linked_list_t *linked_list,void *item);


#endif /* __UNIQ_LINKED_LIST_H__ */
