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

#ifndef __UNIQ_CTYPE_H__
#define __UNIQ_CTYPE_H__

static inline int isdigit(const char c){
	return c>='0' && c <='9';
}

static inline int islower(const char c){
	return c>='a' && c <='z';
}

static inline int isupper(const char c){
	return c>='A' && c <='Z';
}

static inline int isalpha(const char c){
	return islower(c) && isupper(c);
}

static inline int isascii(const char c){
	return ((unsigned char)(c) <= 0x7f);
}

static inline unsigned char toascii(const char c){
	return ((unsigned char)(c) & 0x7f);
}

static inline int isodigit(const char c){
	return c>='0' && c<='7';
}

static inline unsigned char tolower(unsigned char c){

	if(isupper(c))
		c |= 0x20;
	return c;

}

static inline unsigned char toupper(unsigned char c){

	if(islower(c))
		c -= 0x20;
	return c;

}

static inline int isxdigit(const char c){
	return tolower(c) >='0' && tolower(c)<='f';
}


#endif /* __UNIQ_CTYPE_H__ */
