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

#ifndef __UNIQ_VERSION_H__
#define __UNIQ_VERSION_H__

#include <uniq/types.h>

extern char * __kern_name;
extern char * __kern_version_fmt;

extern uint8_t __kern_version_major;
extern uint8_t __kern_version_minor;
extern uint8_t __kern_version_lower;
extern char * __kern_version_suffix;
extern char * __kern_version_codename;

extern char * __kern_arch;

extern char * __kern_build_date;
extern char * __kern_build_time;

extern char * __kern_compiler_version;


#endif /* __UNIQ_VERSION_H__ */
