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
#include <hashmap.h>
#include <string.h>
#include <uniq/kernel.h>

/*
 * hashmap_str_hashcode,
 *
 * @param hash_key :
 */
static uint32_t hashmap_str_hashcode(void *hash_key){

	uint32_t hash_code = 0;
	char *key = (char*)hash_key;
	uint8_t c;

	while(c = *key++)
		hash_code = c + hash_code + (hash_code << 5) + (hash_code << 14) - (hash_code << 7);

	return hash_code;

}

/*
 * hashmap_int_hashcode,
 *
 * @param hash_key :
 */
static uint32_t hashmap_int_hashcode(void *hash_key){

	return (uint32_t)hash_key;

}

/*
 * hashmap_strcmp,
 *
 * @param x :
 * @param y :
 */
static int32_t hashmap_strcmp(void *x,void *y){

	return strcmp(x,y);

}

/*
 * hashmap_intcmp,
 *
 * @param x :
 * @param y :
 */
static int32_t hashmap_intcmp(void *x,void *y){

	return (int32_t)x == (int32_t)y;

}

/*
 * hashmap_strdup,
 *
 * @param s :
 */
static void *hashmap_strdup(void *s){

	return strdup(s);

}

/*
 * hashmap_intdup,
 *
 * @param integer :
 */
static void *hashmap_intdup(void *integer){

	return integer;

}

/*
 * hashmap_destroy,
 *
 * @param hashmap :
 */
void hashmap_destroy(hashmap_t *hashmap){

	if(!hashmap)
		return;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

}

/*
 * hashmap_check,
 *
 * @param hashmap : 
 * @param hash_key :
 */
int32_t hashmap_check(hashmap_t *hashmap,void *hash_key){

	if(!hashmap)
		return -1;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	return -1;

}

/*
 * hashmap_get,
 *
 * @param hashmap :
 * @param hash_key :
 */
void *hashmap_get(hashmap_t *hashmap,void *hash_key){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	return NULL;

}

/*
 * hashmap_set,
 *
 * @param hashmap :
 * @param item :
 * @param hash_key :
 */
void *hashmap_set(hashmap_t *hashmap,void *item,void *hash_key){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	return NULL;

}

/*
 * hashmap_int_create,
 *
 * @param size :
 */
hashmap_t *hashmap_int_create(uint32_t size){

	if(!size)
		return NULL;

	hashmap_t *new_map = malloc(sizeof(hashmap_t));
	new_map->entries = malloc(sizeof(hashmap_entry_t*) * size);
	memset(new_map->entries,0,sizeof(hashmap_entry_t*) * size);
	
	new_map->size = size;
	new_map->signature = HASHMAP_SIGNATURE;

	/* fonksiyonlar */
	new_map->hash_code = &hashmap_int_hashcode;
	new_map->hash_cmp = &hashmap_intcmp;
	new_map->hash_free = &free; 
	new_map->hash_dup = &hashmap_intdup;

	return new_map;

}

/*
 * hashmap_str_create,
 *
 * @param size :
 */
hashmap_t *hashmap_str_create(uint32_t size){

	if(!size)
		return NULL;

	hashmap_t *new_map = malloc(sizeof(hashmap_t));
	new_map->entries = malloc(sizeof(hashmap_entry_t*) * size);
	memset(new_map->entries,0,sizeof(hashmap_entry_t*) * size);
	
	new_map->size = size;
	new_map->signature = HASHMAP_SIGNATURE;

	/* fonksiyonlar */
	new_map->hash_code = &hashmap_str_hashcode;
	new_map->hash_cmp = &hashmap_strcmp;
	new_map->hash_free = &free; 
	new_map->hash_dup = &hashmap_strdup;

	return new_map;

}

/*
 * hashmap_remove,
 *
 * @param hashmap :
 * @param hash_key :
 */
void *hashmap_remove(hashmap_t *hashmap,void *hash_key){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	return NULL;

}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
