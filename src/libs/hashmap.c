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
#include <list.h>

/*
 * hashmap_str_hashcode, karakter dizisi icin hash kodu
 * uretir.
 *
 * @param hash_key : hash anahtari
 */
static uint32_t hashmap_str_hashcode(void *hash_key){

	uint32_t hash_code = 0;
	char *key = (char*)hash_key;
	uint8_t c;

	while(c = *key++)
		hash_code = c + (hash_code << 6) + (hash_code << 16) - hash_code;

	return hash_code;

}

/*
 * hashmap_int_hashcode, integer hashmap icin hash kodu belirlenir.
 * (hash kodu olarak adresin geri donduruldugune dikkat edin!)
 *
 * @param hash_key : hash anahtari
 */
static uint32_t hashmap_int_hashcode(void *hash_key){

	return (uint32_t)hash_key;

}

/*
 * hashmap_strcmp, hashmap icin 2 karakter dizisini karsilastirir.
 * (hashmap icin hash key karsilastirilmasinda kullanilir.)
 *
 * @param x : ilk karakter dizisi
 * @param y : diger karakter dizisi
 */
static int32_t hashmap_strcmp(void *x,void *y){

	return !strcmp(x,y);

}

/*
 * hashmap_intcmp, verilen 2 adresi karsilastirir.
 * (hashmap icin hash key karsilastirilmasinda kullanilir.)
 *
 * @param x : ilk adres
 * @param y : diger adres
 */
static int32_t hashmap_intcmp(void *x,void *y){

	return (int32_t)x == (int32_t)y;

}

/*
 * hashmap_strdup, verilen karakter dizisini klonlar.(hashmap icin
 * hash key'in klonlanmasinda kullanilir.)
 *
 * @param s : karakter dizisi
 */
static void *hashmap_strdup(void *s){

	return strdup(s);

}

/*
 * hashmap_intdup ve hashmap_int_free fonksiyonlarinin kullanilmasi
 * gereksiz gibi durabilir. fakat hashmap yapisi incelediginizde
 * yapida fonksiyon isaretcilerinin oldugunu goreceksiniz. bu
 * fonksiyonlar string ve integer hashmaplerin ortak fonksiyonlar
 * tarafindan islenmesinde ayri ayri fonksiyonlarin yazilmamasi
 * konusunda icin gereklidir. bunu hashmap fonksiyonlarinin tamamini
 * incelediginizde goreceksiniz.
 */
static void *hashmap_intdup(void *integer){

	return integer;

}

static void hashmap_int_free(void *integer){

	return;

}

/*
 * hashmap_destroy, hashmap yapisini tum icerigiyle
 * birlikte siler.
 *
 * @param hashmap : hashmap yapisi
 */
void hashmap_destroy(hashmap_t *hashmap){

	if(!hashmap)
		return;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	for(uint32_t i = 0;i < hashmap->size;i++){

		hashmap_entry_t *entry = hashmap->entries[i],*last;

		for(;entry;entry = entry->next){

			last = entry;
			hashmap->hash_key_free(last->hash_key);
			hashmap->hash_item_free(last);
			

		}

	}
	
	free(hashmap->entries);

}

/*
 * hashmap_check, hashmap yapisindan hash key'i arar
 * bulunamazsa -1, bulunursa 1 doner.
 *
 * @param hashmap : hashmap yapisi 
 * @param hash_key : hash anahtari
 */
int32_t hashmap_check(hashmap_t *hashmap,void *hash_key){

	if(!hashmap)
		return -1;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");
	
	if(!hash_key)
		return -1;

	uint32_t index = hashmap->hash_code(hash_key) % hashmap->size;
	hashmap_entry_t *entry = hashmap->entries[index];

	if(entry){

		do{
			
			if(hashmap->hash_cmp(entry->hash_key,hash_key))
				return 1;

			entry = entry->next;
	
		}while(entry);

	}

	return -1;	

}

/*
 * hashmap_get_values, hashmap yapisindaki itemler yada
 * diger bir deyisle icerikleri bir liste olusturup, listeye
 * doldurur ve listenin adresini dondurur.
 *
 * @param hashmap : hashmap yapisi
 */
list_t *hashmap_get_values(hashmap_t *hashmap){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	list_t *list = list_create();

	for(uint32_t i = 0;i < hashmap->size;i++){

		hashmap_entry_t *entry = hashmap->entries[i];

		for(;entry;entry = entry->next)
			list_push(list,entry->item);

	}

	return list;	

}

/*
 * hashmap_get_keys, hashmap yapisindaki hash keyleri
 * bir liste olusturup, listeye doldurur ve listenin adresini
 * donudurur.
 *
 * @param hashmap : hashmap yapisi
 */
list_t *hashmap_get_keys(hashmap_t *hashmap){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	list_t *list = list_create();

	for(uint32_t i = 0;i < hashmap->size;i++){

		hashmap_entry_t *entry = hashmap->entries[i];

		for(;entry;entry = entry->next)
			list_push(list,entry->hash_key);

	}

	return list;	

}

/*
 * hashmap_get, hashmap yapisinda belirtilen hash anahtarina
 * gore arama yapar ve hash anahtarina ait itemin-icerigin
 * adresini dondurur.
 *
 * @param hashmap : hashmap yapisi
 * @param hash_key : hash anahtari
 */
void *hashmap_get(hashmap_t *hashmap,void *hash_key){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	if(!hash_key)
		return NULL;

	uint32_t index = hashmap->hash_code(hash_key) % hashmap->size;
	hashmap_entry_t *entry = hashmap->entries[index];

	if(entry)

		do{

			if(hashmap->hash_cmp(entry->hash_key,hash_key))
				return entry->item;
			
			entry = entry->next;

		}while(entry);

	}

	return NULL;

}

/*
 * hashmap_set, hashmap yapisina hash anahtarina gore yeni
 * icerik eklenir yada belirtilen hash anahtari onceden
 * listede varsa icerigi guncellenir.
 *
 * @param hashmap : hashmap yapisi
 * @param item : icerik
 * @param hash_key : hash anahtari 
 */
void *hashmap_set(hashmap_t *hashmap,void *item,void *hash_key){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	if(!hash_key)
		return NULL;

	uint32_t index = hashmap->hash_code(hash_key) % hashmap->size;
	hashmap_entry_t *entry = hashmap->entries[index];

	if(!entry){

		hashmap_entry_t *new_entry = malloc(sizeof(hashmap_entry_t));
		new_entry->next = NULL;
		new_entry->item = item;
		new_entry->hash_key = hashmap->hash_dup(hash_key);
		hashmap->entries[index] = new_entry;

	}
	else{

		hashmap_entry_t *last = NULL;

		do{

			if(hashmap->hash_cmp(entry->hash_key,hash_key)){

				void *break_item = entry->item;
				entry->item = item;

				return break_item;

			}

			last = entry;
			entry = entry->next;

		}while(entry);

		hashmap_entry_t *new_entry = malloc(sizeof(hashmap_entry_t));
		new_entry->next = NULL;
		new_entry->item = item;
		new_entry->hash_key = hashmap->hash_dup(hash_key);
		last->next = new_entry;

	}

	return NULL;

}

/*
 * hashmap_int_create, integer hashmap olusturur.
 *
 * @param size : boyut
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
	new_map->hash_item_free = &free;
	new_map->hash_key_free = &hashmap_int_free;
	new_map->hash_dup = &hashmap_intdup;

	return new_map;

}

/*
 * hashmap_str_create, string hashmap olusturur.
 *
 * @param size : boyut
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
	new_map->hash_item_free = new_map->hash_key_free = &free; 
	new_map->hash_dup = &hashmap_strdup;

	return new_map;

}

/*
 * hashmap_remove, hashmap yapisindaki belirtilen hash
 * anahtarini kaldirir.
 *
 * @param hashmap : hashmap yapisi
 * @param hash_key : hash anahtari
 */
void *hashmap_remove(hashmap_t *hashmap,void *hash_key){

	if(!hashmap)
		return NULL;
	
	assert(hashmap->signature == HASHMAP_SIGNATURE && "Wrong! hashmap signature");

	if(!hash_key)
		return NULL;

	uint32_t index = hashmap->hash_code(hash_key) % hashmap->size;
	hashmap_entry_t *entry = hashmap->entries[index];

	if(entry){

		if(hashmap->hash_cmp(entry->hash_key,hash_key)){

			void *break_item = entry->item;
			hashmap->entries[index] = entry->next;
			hashmap->hash_key_free(entry->hash_key);
			hashmap->hash_item_free(entry);
			
			return break_item;

		}else{

			hashmap_entry_t *last = entry;
			entry = entry->next;

			do{

				if(hashmap->hash_cmp(entry->hash_key,hash_key)){

					void *break_item = entry->item;
					last->next = entry->next;
					hashmap->hash_key_free(entry->hash_key);
					hashmap->hash_item_free(entry);

					return break_item;

				}

				last = entry;
				entry = entry->next;

			}while(entry);

		}

	}

	return NULL;

}

/*
 * __hashmap_test
 */
void __hashmap_test(void){



}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
