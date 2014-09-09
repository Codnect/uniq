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

#ifndef __UNIQ_LIST_H__
#define __UNIQ_LIST_H__

#include <linked_list.h>

typedef linked_list_t 			list_t;

#define list_get_index(list,search)	linked_list_get_index(list,search)
#define list_search(list,search)	linked_list_search(list,search)
#define list_remove(list,search)	linked_list_remove(list,search)
#define list_pop(list)			linked_list_pop(list)
#define list_unlink(list,node)		linked_list_unlink(list,node)
#define list_push(list,item)		linked_list_push(list,item)
#define list_link(list,new_node)	linked_list_link(list,new_node)
#define list_clear(list)		linked_list_clear(list)
#define list_free(list)			linked_list_free(list)
#define list_destroy(list)		linked_list_destroy(list)
#define list_merge(dest,src)		linked_list_merge(dest,src)
#define list_clone(list)		linked_list_clone(list)
#define list_create			linked_list_create
#define __list_test			__linked_list_test

#endif /* __UNIQ_LIST_H__ */
