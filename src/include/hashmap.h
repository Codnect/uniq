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

#ifndef __UNIQ_HASHMAP_H__
#define __UNIQ_HASHMAP_H__

#include <uniq/types.h>

#define HASHMAP_SIGNATURE			0x79FFC571

typedef void (*hashmap_free_t)(void *x);
typedef void *(*hashmap_dup_t)(void *x);
typedef int32_t (*hashmap_cmp_t)(void *x,void *y);
typedef uint32_t (*hashmap_hash_code_t)(void *key);

typedef struct _hashmap_entry_t{
	struct _hash_map_entry_t *next;
	char *hash_key;
	void *item;
}hashmap_entry_t;

typedef struct{
	hashmap_hash_code_t hash_code;
	uint32_t size;
	uint32_t signature;
	hashmap_entry_t **entries;
	hashmap_cmp_t  hash_cmp;
	hashmap_dup_t hash_dup;
	hashmap_free_t hash_free;
}hashmap_t;

#endif /* __UNIQ_HASHMAP_H__ */
