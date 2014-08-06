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

#ifndef __UNIQ_STRING_H__
#define __UNIQ_STRING_H__

extern size_t strlen(const char *s);
extern size_t strnlen(const char *s,size_t count);
extern int strcmp(const char *s1,const char *s2);
extern int strncmp(const char *s1,const char *s2,size_t count);
extern int strcasecmp(const char *s1,const char *s2);
extern int strncasecmp(const char *s1,const char *s2,size_t count);
extern char* strtoupper(char* s);
extern char* strtolower(char *s);
extern char* strntoupper(char* s,size_t count);
extern char* strntolower(char *s,size_t count);
extern char* strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t count);
extern int strtobool(const char *s, bool *r);
extern char *str_ltrim(char *s);
extern char *str_rtrim(char *s);
extern char *strim(char *s);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strtok(char *s,const char *delim);
extern char *strrtok(char *s,const char *delim);
extern int memcmp(const void *m1, const void *m2, size_t c);
extern void *memcpy(void *dest, const void *src, size_t c);
extern void *memset(void *s, int v, size_t c);
extern void *memscan(void *s, int v, size_t c);
extern char *strstr(const char *s1, const char *s2);
extern char *strnstr(const char *s1, const char *s2, size_t len);
extern char *strcasestr(const char *s1, const char *s2);
extern char *strncasestr(const char *s1, const char *s2, size_t len);
extern char *strcat(char *dest, const char *src);
extern char *strncat(char *dest, const char *src, size_t count);

#define strlchr(s,c)		strchr(s,c)
#define strltok(s,delim)	strtok(s,delim)

#endif /* __UNIQ_STRING_H__ */
