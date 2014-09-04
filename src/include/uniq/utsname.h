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

#ifndef __UNIQ_UTSNAME_H__
#define __UNIQ_UTSNAME_H__

#define UTSNAME_LENGTH		256

typedef struct{
	char  sysname[UTSNAME_LENGTH];
	char nodename[UTSNAME_LENGTH];
	char  release[UTSNAME_LENGTH];
	char  version[UTSNAME_LENGTH];
	char  machine[UTSNAME_LENGTH];
	char domainname[UTSNAME_LENGTH];
}utsname_t;

#endif /* __UNIQ_UTSNAME_H__ */
