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

#ifndef __UNIQ_TREE_H__
#define __UNIQ_TREE_H__

#include <uniq/types.h>
#include <list.h>

#define TREE_SIGNATURE			0xCCD499AA

typedef struct tree_branch{
	struct tree_branch *branch;
	list_t *sub_branches;
	void *item;	
}tree_branch_t;

typedef struct{
	tree_branch_t *root_branch;
	uint32_t signature;
	uint32_t nbranch;	
}tree_t;

#endif /* __UNIQ_TREE_H__ */
