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

#ifndef __UNIQ_APM_BIOS_H__
#define __UNIQ_APM_BIOS_H__

#include <uniq/types.h>

struct apm_bios_info_t{
	uint16_t	version;
	uint16_t	cseg;
	uint32_t	offset;
	uint16_t	cseg_16;
	uint16_t	dseg;
	uint16_t	flags;
	uint16_t	cseg_len;
	uint16_t	cseg_16_len;
	uint16_t	dseg_len;
};

#endif /* __UNIQ_APM_BIOS_H__ */
