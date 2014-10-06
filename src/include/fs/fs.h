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

#ifndef __UNIQ_FS_H__
#define __UNIQ_FS_H__

#define FS_FILE         	0x01
#define FS_DIRECTORY    	0x02
#define FS_CHAR_DEVICE  	0x04
#define FS_BLOCK_DEVICE 	0x08
#define FS_PIPE         	0x10
#define FS_SYMLINK      	0x20
#define FS_MOUNTPOINT   	0x40

struct fs_node;

typedef void (*fs_open_t)(struct fs_node *node,uint32_t flags);
typedef void (*fs_close_t)(struct fs_node *node);
typedef int32_t (*fs_read_t)(struct fs_node *node,uint32_t offset,uint32_t size,uint8_t *buf);
typedef int32_t (*fs_write_t)(struct fs_node *node,uint32_t offset,uint32_t size,uint8_t *buf);

typedef struct dirent *(*fs_readdir_t)(struct fs_node *node, uint32_t index);
typedef struct fs_node *(*fs_finddir_t)(struct fs_node *node,char *name);
typedef void (*fs_mkdir_t)(struct fs_node *node,char *name,uint16_t perm);


struct fs_node{
    	char  fname[256];           /* dosya ismi */
    	uint32_t mask;              /* izin maskeleri */
    	uint32_t uid;               /* kullanici sahip id*/
    	uint32_t gid;               /* grup sahip id*/
    	uint32_t flags;             /* node tipi, vs. */
    	uint32_t inode;             /* inode numarasi */
    	uint32_t size;              /* dosya boyutu (bayt olarak) */
    	uint32_t open_flags;        /* acmak icin flaglar (okuma-yazma-ekleme vs...) */

    	/* erisim/degistirme/olusturma zamanlari */
	uint32_t access_time;       /* erisim */
	uint32_t modify_time;       /* degistirme */
	uint32_t create_time;       /* olusturma  */
	
	/* dosya islemleri */
	fs_open_t open;
	fs_close_t close;
	fs_read_t read;
	fs_write_t write;

	fs_readdir_t readdir;
	fs_finddir_t finddir;
	fs_mkdir_t mkdir;
	
}fs_node_t;

struct dirent{
	uint32_t inode;
	char fname[256];
};

#endif /* __UNIQ_FS__ */
