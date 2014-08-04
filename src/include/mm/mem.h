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

#ifndef __UNIQ_MEM_H__
#define __UNIQ_MEM_H__

	
#define FRAME_SIZE_BYTE		4096		/* 4096 Byte - 4 KiB - 0x1000 */
#define FRAME_SIZE_KIB		4		/* 4 KiB */
#define ALIGN_LIMIT		0xFFFFF000
#define MAX_LIMIT		0xFFFFFFFF	/* 4 GiB */

#define FRAME_INDEX_BIT(x)	(x / 32)
#define FRAME_OFFSET_BIT(x)	(x % 32)

#endif /* __UNIQ_MEM_H__ */
