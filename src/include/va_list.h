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
 
#ifndef __VA_LIST_H__
#define __VA_LIST_H__

typedef __builtin_va_list  	va_list;
#define va_start(ap,last) 	__builtin_va_start(ap, last)
#define va_end(ap) 		__builtin_va_end(ap)
#define va_arg(ap,type) 	__builtin_va_arg(ap,type)
#define va_copy(dest, src) 	__builtin_va_copy(dest,src)

#endif /* __VA_LIST_H__ */
