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
#include <fs/fs.h>

/*
 * vfs_init,
 */
void vfs_init(void){
 
	debug_print(KERN_INFO,"Initializing the vfs.");
 
}
 
/*
 * fs_read,
 *
 * @param node :
 * @param offset :
 * @param size :
 * @param buf :
 */
int32_t fs_read(fs_node_t *node,uint32_t offset,uint32_t size,uint8_t *buf){

	if(!node)
		return -1;

	if(node->read)
		return node->read(node,offset,size,buf);

	return -1;

}

/*
 * fs_write,
 *
 * @param node :
 * @param offset :
 * @param size :
 * @param buf :
 */
int32_t fs_write(fs_node_t *node,uint32_t offset,uint32_t size,uint8_t *buf){

	if(!node)
		return -1;

	if(node->read)
		return node->write(node,offset,size,buf);

	return -1;

}

/*
 * fs_open,
 *
 * @param node :
 * @param flags :
 */
void fs_open(fs_node_t *node,uint32_t flags){

	if(!node)
		return;

}

/*
 * fs_close,
 *
 * @param node :
 */
void fs_close(fs_node_t *node){

	if(!node)
		return;

}

/*
 * fs_readdir,
 *
 * @param node :
 * @param index :
 */
struct dirent *fs_readdir(fs_node_t *node,uint32_t index){

	if(!node)
		return NULL;

}

/*
 * fs_finddir,
 *
 * @param node :
 * @param name :
 */
fs_node_t *fs_finddir(fs_node_t *node,char *name){

	if(!node)
		return NULL;

}


MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
