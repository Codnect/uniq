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
 
#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <uniq/types.h>

typedef struct{
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint32_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
}__attribute__ ((packed)) mboot_info_t;

typedef struct{
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
}__attribute__ ((packed)) mboot_memmap_t;

typedef struct{
	uint16_t attributes;
	uint8_t  winA, winB;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t segmentA, segmentB;
	uint32_t realFctPtr;
	uint16_t pitch;

	uint16_t Xres, Yres;
	uint8_t  Wchar, Ychar, planes, bpp, banks;
	uint8_t  memory_model, bank_size, image_pages;
	uint8_t  reserved0;

	uint8_t  red_mask, red_position;
	uint8_t  green_mask, green_position;
	uint8_t  blue_mask, blue_position;
	uint8_t  rsv_mask, rsv_position;
	uint8_t  directcolor_attributes;
	
	uint32_t physbase;
	uint32_t reserved1;
	uint16_t reserved2;
}__attribute__ ((packed)) vbe_info_t;

#endif /* __MULTIBOOT_H__ */
