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
 * tree_create, agac yapisi olusturur.
 */
tree_t *tree_create(void){

	tree_t *new_tree = malloc(sizeof(tree_t));
	new_tree->root_node = NULL;
	new_tree->node_count = 0;
	new_tree->signature = TREE_SIGNATURE;
	
	return new_tree;

}

/*
 * tree_node_free, verilen dugumden baslayarak bu
 * dugumun alt dugumleriyle birlikte bellekte bosa
 * cikarir.(free)
 *
 * @param node : agac dugumu
 */
void tree_node_free(tree_node_t *node){

	if(!node)
		return;

	node_t *child_list_node = node->child->first_node;
	
	for(;child_list_node; child_list_node = child_list_node->next)
		tree_node_free((tree_node_t*)child_list_node->item);
	
	free(node);

}

/*
 * tree_node_destroy, verilen dugumden baslayarak bu
 * dugumun alt dugumleriyle birlikte bulunan itemleri yada
 * diger bir tabirle icerikleri siler.(bellekte bosa cikarir
 * -free)
 *
 * @param node : agac dugumu
 */
void tree_node_destroy(tree_node_t *node){

	if(!node)
		return;

	node_t *child_list_node = node->child->first_node;
	
	for(;child_list_node; child_list_node = child_list_node->next)
		tree_node_destroy((tree_node_t*)child_list_node->item);
	
	free(node->item);

}

/*
 * tree_free, verilen agac yapisinda kok dugumden
 * baslayarak tum dugumleri siler yada diger bir
 * tabirle bellekte bosa cikarir diyebiliriz.(free)
 *
 * @param tree : agac yapisi
 */
void tree_free(tree_t *tree){

	if(!tree || !tree->root_node)
		return;
	
	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	tree_node_free(tree->root_node);

}

/*
 * tree_destroy, agac yapisinda kok dugumden baslayarak
 * tum dugumlerde bulunan icerikleri(itemleri) siler.
 * (bellekte bosa cikarir-free)
 *
 * @param tree : agac yapisi
 */
void tree_destroy(tree_t *tree){

	if(!tree || !tree->root_node)
		return;
	
	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	tree_node_destroy(tree->root_node);

}

/*
 * tree_total_node, agac yapisindaki toplam dugum
 * sayisini dondurur.
 *
 * @param tree : agac yapisi
 */
uint32_t tree_total_node(tree_t *tree){

	if(!tree || !tree->root_node)
		return 0;
	
	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	
	return tree->node_count;

}

/*
 * tree_child_count, verilen agac dugumundeki child
 * (cocuk) dugum sayisini dondurur.
 *
 * @param node : agac dugumu
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
 * tree_node_search_parent, verilen baslangic dugumunden
 * baslayarak buna bagli tum dugumlerde istenilen dugumu
 * bulmaya calisir, bulamazsa "NULL" doner.
 *
 * @param start_node : baslangic dugumu
 * @param search : aranan dugum
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
 * tree_search_parent, agac yapisinde istenilen dugumu
 * bulmaya calisir, bulamazsa "NULL" doner.
 *
 * @param tree : agac yapisi
 * @param search : aranan dugum
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
 * tree_node_create, verilen item icin agac dugumu 
 * olusturur.
 *
 * @param item : item
 */
tree_node_t *tree_node_create(void *item){

	tree_node_t *new_node = malloc(sizeof(tree_node_t));
	new_node->parent = NULL;
	new_node->child = list_create();
	new_node->item  = item;

	return new_node;

}

/*
 * tree_set_root_node, agac yapisindaki kok dugumu
 * ayarlar. 
 *
 * @param tree : agac yapisi
 * @param item : item(isaretci olduguna dikkat!)
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
 * tree_push_child_node, child dugumunu parent dugumunun
 * child listesine ekler.
 *
 * @param tree : agac yapisi
 * @param child : child(cocuk) dugum
 * @param parent : parent(ebeveyn) dugum
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
 * tree_push_child, yukaridaki tree_push_child_node 
 * fonksiyonundan farkli olarak verilen item icin
 * dugum olusturur ve daha sonra parent dugumunun
 * child listesine bu dugumu ekler. usteki fonksiyonda
 * child dugumu zaten parametre olarak veriyoruz.
 *
 * @param tree : agac yapisi
 * @param item : item(isaretci!)
 * @param parent : parent dugumu
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
 * tree_node_unlink, verilen dugum parent dugumunden ayrilir.
 *
 * @param node : dugum
 */
void tree_node_unlink(tree_node_t *node){

	 if(!node)
		return;

	tree_node_t *parent = node->parent;
	if(!parent)
		return;

	node_t *search = list_search(parent->child,node);
	list_unlink(parent->child,search);

}

/* 
 * tree_node_parent_merge, verilen dugumu kendi parent
 * child listesinde arastirir eger bulunursa dugum
 * parent dugumunun child listesinden ayrilir. ve bu
 * dugumun child listesi parent dugumunun child listesiyle
 * birlestirilir.
 *
 * @param tree : agac yapisi
 * @param node : dugum
 */
void tree_node_parent_merge(tree_t *tree,tree_node_t *node){

	 if(!tree || !node)
		return;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	tree_node_t *parent = node->parent;
	if(!parent)
		return;

	node_t *search = list_search(parent->child,node);
	list_unlink(parent->child,search);
	tree->node_count--;

	node_t *child_list_node = node->child->first_node;
	for(;child_list_node;child_list_node = child_list_node->next)
		((tree_node_t*)child_list_node->item)->parent = parent;

	list_merge(parent->child,node->child);
	free(node);

}

/* 
 * tree_parent_root, verilen dugumu kendi parent
 * child listesinde arastirir eger bulunursa dugum
 * parent dugumunun child listesinden ayrilir. ve
 * dugumun child listesi kok dugumun child listesiyle
 * birlestirilir.
 *
 * @param tree : agac yapisi
 * @param node : dugum
 */
void tree_parent_root(tree_t *tree,tree_node_t *node){

	 if(!tree || !node)
		return;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	tree_node_t *parent = node->parent;
	if(!parent)
		return;

	node_t *search = list_search(parent->child,node);
	list_unlink(parent->child,search);
	tree->node_count--;

	
	node_t *child_list_node = node->child->first_node;
	for(;child_list_node;child_list_node = child_list_node->next)
		((tree_node_t*)child_list_node->item)->parent = tree->root_node;

	list_merge(tree->root_node->child,node->child);
	free(node);

}

/*
 * tree_node_parent_remove, verilen dugumu parent dugumunun
 * child listesinde arar, eger bu listede varsa bu dugum 
 * kaldirilir.
 *
 * @param tree : agac yapisi
 * @param node : dugum
 * @param parent : parent dugumu
 */
void tree_node_parent_remove(tree_t *tree,tree_node_t *node,tree_node_t *parent){

	if(!tree)
		return;
	
	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");
	
	if(!parent || !node)
		return;
	
	node_t *search = list_search(parent->child,node);
	list_unlink(parent->child,search);

	/*
	 * +1 dugumun kendisi 
	 */
	tree->node_count -= 1 + tree_child_count(node);
	tree_node_free(node);

}

/*
 * tree_node_remove, agac yapisindaki verilen dugumu bellekte 
 * bosa cikarir(free) yada agac yapisindan kaldirir diyebiliriz.
 *
 * @param tree : agac yapisi
 * @param node : dugum
 */
void tree_node_remove(tree_t *tree,tree_node_t *node){

	 if(!tree || !node)
		return;

	assert(tree->signature == TREE_SIGNATURE && "Wrong! tree signature");

	if(node == tree->root_node){
		
		tree_node_free(node);
		tree->root_node = NULL;
		tree->node_count = 0;

		return;

	}

	tree_node_parent_remove(tree,node,node->parent);

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
