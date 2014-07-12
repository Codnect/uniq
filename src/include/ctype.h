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

static inline int isdigit(int c){
	return c>='0' && c <='9';
}

static inline int islower(int c){
	return c>='a' && c <='z';
}

static inline int isupper(int c){
	return c>='A' && c <='Z';
}

static inline int isalpha(int c){
	return islower(c) || isupper(c);
}

static inline int isascii(int c){
	return ((unsigned char)(c) <= 0x7f);
}

static inline int toascii(int c){
	return ((unsigned char)(c) & 0x7f);
}

static inline int isodigit(int c){
	return c>='0' && c<='7';
}

static inline int tolower(int c){
	if(isupper(c))
		c |= 0x20;
	return c;
}

static inline int toupper(int c){
	if(islower(c))
		c -= 0x20;
	return c;
}

static inline int isxdigit(int c){
	return tolower(c) >='0' && tolower(c)<='f';
}

static inline int iscntrl(int c){
	return (c >=0x0 && c<= 0x1f) || 0x7f; 
}

/*
 * ' '	(0x20)	space (SPC)
 * '\t'	(0x09)	horizontal tab (TAB)
 * '\n'	(0x0a)	newline (LF)
 * '\v'	(0x0b)	vertical tab (VT)
 * '\f'	(0x0c)	feed (FF)
 * '\r'	(0x0d)	carriage return (CR)
 */
static inline int isspace(int c){
	return (c == 0x20) || (c >= 0x09 && c <= 0xd);
}

static inline int isgraph(int c){
	return (c >= 0x21 && c <= 0x7e);
}

static inline int isprint(int c){
	return (c >= 0x20 && c <= 0x7e);
}

static inline int isalnum(int c){
	return isalpha(c) || isdigit(c);
}

static inline int ispunct(int c){
	return !isalpha(c) && !isdigit(c) && !(c < 0x21);
}

static inline int isblank(int c){
	return (c == 0x09 || c == 0x20);
}

#endif /* __UNIQ_CTYPE_H__ */
