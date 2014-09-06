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

#include <uniq/types.h>
#include <ctype.h>
#include <uniq/module.h>
#include <uniq/kernel.h>

/*
 * strlen, verilen karakter dizisinin uzunlugu verir.
 * 
 * @param s : karakter dizisi 
 */
size_t strlen(const char *s){
 
	size_t i = 0;
	while(s[i++]);
	
	return --i;
}

/*
 * strnlen,verilen karakter dizisinin uzunlugu verir.
 * fakat belli bir maksimum karaktere kadar bakar.
 *
 * @param s : karakter dizisi
 * @param count : maksimum bakilacak karakter uzunlugu
 */
size_t strnlen(const char *s,size_t count){
	
	size_t i = 0;
	while(count--){
		if(!s[i++]){
			--i;
			break;
		}
	}
	
	return i;
}

/*
 * strcmp, verilen 2 karakter dizisini karsilastirir.
 * 
 * @param s1 : ilk karakter dizisi
 * @param s2 : diger karakter dizisi 
 */
int strcmp(const char *s1,const char *s2){
 
   	size_t i = 0;
   
   	for(;;){
    		if(s1[i] != s2[i])
         		return (s1[i] > s2[i]) ? 1 : -1;
         	if(!s1[i])
         		break;
      		i++;
   	}
   	
	return 0;
}

/*
 * strncmp, 2 karakter dizisini belirtilen karakter sayisina
 * karar karsilastirir.(buyuk ve kucuk harf'e duyarlidir)
 *
 * @param s1 : ilk karakter dizisi
 * @param s2 : diger karakter dizisi
 * @param count : maksimum karsilastirilacak karakter sayisi
 */
int strncmp(const char *s1,const char *s2,size_t count){
  
	size_t i = 0;
  
  	while(count--){
      		if(s1[i] != s2[i])
         		return (s1[i] > s2[i]) ? 1 : -1;
         	if(!s1[i])
         		break;
      		i++;
  	}

  	return 0;
}

/*
 * strcasecmp,verilen 2 karakter dizisini buyuk ve kucuk harf'e
 * duyarli olmadan karsilastirir.
 *
 * @param s1: ilk karakter dizisi
 * @param s2: diger karakter dizisi
 */
int strcasecmp(const char *s1,const char *s2){
 
  	int c1,c2,i = 0;
  
  	for(;;){
     		c1 = tolower(s1[i]);
     		c2 = tolower(s2[i]);
     		if(c1 != c2)
        		return (c1 > c2) ? 1 : -1;
    		if(!c1)
        		break;
     		i++;
  	}
  
  	return 0;
}

/*
 * strncasecmp, 2 karakter dizisini belirtilen karakter sayisina
 * buyuk ve kucuk harf'e duyarli olmadan karsilastirir.
 *
 * @param s1 : ilk karakter dizisi
 * @param s2 : diger karakter dizisi
 * @param count : maksimum karsilastirilacak karakter sayisi 
 */
int strncasecmp(const char *s1,const char *s2,size_t count){
 
  	int c1,c2,i = 0;
  
  	while(count--){
     		c1 = tolower(s1[i]);
     		c2 = tolower(s2[i]);
     		if(c1 != c2)
        		return (c1 > c2) ? 1 : -1;
        	if(!c1)
        		break;
    	 	i++;
  	}
  
  	return 0;
}

/*
 * strtoupper, verilen karakter dizisini buyuk harfe cevirir.
 *
 * @param s : karakter dizisi
 */
char* strtoupper(char* s){

	size_t i = 0;
	while(s[i]){
		s[i] = toupper(s[i]);
		i++;
	}
	return s;
}

/*
 * strtolower, verilen karakter dizisini kucuk harfe cevirir.
 *
 * @param s : karakter dizisi
 */
char* strtolower(char *s){

	size_t i = 0;
	while(s[i]){
		s[i] = tolower(s[i]);
		i++;
	}
	return s;
}

/*
 * strtoupper, verilen karakter dizisini belirtilen karakter
 * sayisina kadar buyuk harfe cevirir.
 *
 * @param s : karakter dizisi
 * @param count : maksimum karakter sayisi
 */
char* strntoupper(char* s,size_t count) {

	size_t i = 0;
	while(count--){
		if(!s[i])
			break;
		s[i] = toupper(s[i]);
		i++;
	}
	return s;
}

/*
 * strtolower, verilen karakter dizisini belirtilen karakter
 * sayisina kadar kucuk harfe cevirir.
 *
 * @param s : karakter dizisi
 * @param count : karakter sayisi
 */
char* strntolower(char *s,size_t count){

	size_t i = 0;
	while(count--){
		if(!s[i])
			break;
		s[i] = tolower(s[i]);
		i++;
	}
	return s;
}

/*
 * strcpy, bir karakter dizisini diger bir karakter
 * dizisine kopyalar.
 *
 * @param dest : hedef karakter dizisi
 * @param src : kaynak karakter dizisi
 */
char* strcpy(char *dest, const char *src){
	
	char *temp = dest;
	while ((*dest++ = *src++) != '\0');

	return temp;
}

/*
 * strcpy, bir karakter dizisini diger bir karakter
 * dizisine belirtilen karakter sayisi kadar kopyalar.
 *
 * @param dest : hedef karakter dizisi
 * @param src : kaynak karakter dizisi
 * @param count : maksimum kopyalanacak karakter sayisi
 */
char *strncpy(char *dest, const char *src, size_t count){
	
	char *temp = dest;
	while (count--) 
		if ((*dest++ = *src++) != '\0');

	return temp;
}

/*
 * strtobool, kullanici girisini boolean'a gore uyarlar
 *
 * @param s : karakter dizisi
 * @param r : sonuc
 */
int strtobool(const char *s, bool *r){
	
	switch (*s) {
		case '1':
		case 'y':
		case 'Y':
			*r = true;
			break;
		case '0':
		case 'n':
		case 'N':
			*r = false;
			break;
		default:
			return -1;
	}
	return 0;
}

/*
 * str_ltrim, karakter dizisinin solundaki bosluklari
 * kaldirir.
 *
 * @param s : karakter dizisi
 */
char *str_ltrim(char *s){
	while(isspace(*s))
		++s;
	return (char*)s;
}


/*
 * str_rtrim, karakter dizisinin sagindaki bosluklari
 * kaldirir.
 *
 * @param s : karakter dizisi
 */
char *str_rtrim(char *s){

	size_t len;
	char *end_s;
	
	len = strlen(s);
	if(!len)
		return s;
	end_s = s + len - 1;
	while(end_s >= s && isspace(*end_s))
		end_s--;

	*(++end_s) = '\0';

	return (char*)s;	
}

/*
 * strim, karakter dizisinin sagindaki ve solundaki 
 * bosluklari kaldirir.
 *
 * @param s : karakter dizisi
 */
char *strim(char *s){
	return str_ltrim(str_rtrim(s));
	
}

/*
 * strchr, karakter dizisinde soldan baslayarak belirtilen
 * karakteri arar ve adresini dondurur.
 *
 * @param s : karakter dizisi
 * @param c : karakter
 */
char *strchr(const char *s, int c){

	do{
		if(*s == (char)c)
			return (char*)s; 
	}while(*(++s));

	return NULL;
}


/*
 * strrchr, karakter dizisinde sagdan baslayarak belirtilen
 * karakteri arar ve adresini dondurur.
 *
 * @param s : karakter dizisi
 * @param c : karakter
 */
char *strrchr(const char *s, int c){

	const char *temp = s + strlen(s);
	do {
           if (*temp == (char)c)
               return (char *)temp;
        }while (--temp >= s);

	return NULL;
}

/*
 * strcat,bir karakter dizisine diger karakter dizisini
 * ekler.
 *
 * @param dest : hedef karakter dizisi
 * @param src : eklenecek karakter dizisi
 */
char *strcat(char *dest, const char *src){
	char *temp = dest;
	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0');

	return temp;
}

/*
 * strcat,bir karakter dizisine diger karakter dizisinin
 * belirtilen karakter adedi kadar ekler.
 *
 * @param dest : hedef karakter dizisi
 * @param src : eklenecek karakter dizisi
 * @param count : eklenecek karakter adedi
 */
char *strncat(char *dest, const char *src, size_t count){
	
	char *temp = dest;
	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++) != '\0') {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}

	return temp;
}

/*
 * strtok,karakter dizisini belirtilen ayraclara gore
 * soldan baslayarak parcalar.
 *
 * @param s : karakter dizisi
 * @param delim : ayraclar
 */
char *strtok(char *s,const char *delim){
	
	static char *last;
	char *ret;			/* geri donus */
	const char *temp_delim;		/* gecici ayraclar */

	if(s)	/* s != null ise */
		ret = last = s;
	else	/* s == null ise */
		ret = last;
		
	while(*last){
		temp_delim = delim;
		while(*temp_delim){
			if(*temp_delim == *last ){
				*last = '\0';
				last++;
				return (char*)ret;
			}
			temp_delim++;	
		}
		last++;
	}

	if(!(*last) && !(*ret))
		return NULL;
	
	return (char*)ret;	
}

/*
 * strtok,karakter dizisini belirtilen ayraclara gore
 * sagdan baslayarak parcalar.
 *
 * @param s : karakter dizisi
 * @param delim : ayraclar
 */
char *strrtok(char *s,const char *delim){
	
	static char *start;		/* baslangic */
	char *last;			
	size_t len;
	static bool end;		/* karakter dizisinin basina gelinip 
					 * gelinmedigi icin
					 */
	const char *temp_delim;		/* gecici ayraclar */
	
	if(s){	/* s != null ise */
		last = start = s;
		end = false;
	}
	
	if(end)
		return NULL;		
	
	last = start + strlen(start) - 1;

	do{
		temp_delim = delim;
		while(*temp_delim){
			if(*temp_delim == *last ){
				*last = '\0';
				return (char*)(last+1);
			}
			temp_delim++;	
		}
		len--;

	}while(--last >= start);
	
	end = true;
	return (char*)start;
}

/*
 * memcmp, verilen 2 bellek bolgesini belirtilen alan
 * boyutu kadar karsilastirir.
 *
 * @param m1 : ilk bellek bolgesi
 * @param m2 : diger bellek bolgesi
 * @param c : alanin boyutu
 */
int memcmp(const void *m1, const void *m2, size_t c){
	
	const unsigned char *t1, *t2;
	int r = 0;

	t1 = m1;
	t2 = m2;	
	while(c--){
		if((r = *t1++ - *t2++) != 0)
			break;
	}

	return r;

}

/*
 * memcpy, bir bellek bolgesinden diger bolgeye belirtilen
 * alanin boyutu kadar kopyalar.
 *
 * @param m1 : ilk bellek bolgesi
 * @param m2 : diger bellek bolgesi
 * @param c : alanin boyutu
 */
void *memcpy(void *dest, const void *src, size_t c){

	char *tmp = dest;
	const char *s = src;

	while (c--)
		*tmp++ = *s++;
	return dest;
}

/*
 * memset,bir bellek alaninin doldurulucak degerle belirtilen
 * alan kadari doldurur.
 *
 * @param s : bellek bolgesi
 * @param v : doldurulacak deger
 * @param c : doldurulacak alan
 */
void *memset(void *s, int v, size_t c){
	char *tmp = s;

	while (c--)
		*tmp++ = v;
	return s;
}

/*
 * memscan,bir bellek alaninda belirlenen bir bayti 
 * belli bir alan boyutu kadar aranacak ve varsa
 * adresi dondurulecektir.
 *
 * @param s : bellek bolgesi
 * @param v : belirlenen bayt
 * @param c : aranacak boyut
 */
void *memscan(void *s, int v, size_t c){
	unsigned char *p = s;

	while (c--) {
		if (*p == v)
			break;
		p++;
	}
  	return (void *)p;
}

/*
 * strstr,bir karakter dizisi icerisinde diger bir
 * karakter dizisini arar ve buldugunda adresini dondurur.
 * buyuk ve kucuk harfe duyarlidir.
 *
 * @param s1 : hedef karakter dizisi
 * @param s2 : aranacak karakter dizisi
 */
char *strstr(const char *s1, const char *s2){
	
	size_t l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;

	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

/*
 * strcasestr,bir karakter dizisi icerisinde diger bir
 * karakter dizisini arar ve buldugunda adresini dondurur.
 * buyuk ve kucuk harfe duyarli degildir.
 *
 * @param s1 : hedef karakter dizisi
 * @param s2 : aranacak karakter dizisi
 */
char *strcasestr(char *s1,char *s2){
	
	size_t l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;

	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(strtolower(s1),strtolower(s2), l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

/*
 * strnstr,bir karakter dizisi icerisinde diger bir
 * karakter dizisini belli bir karakter sayisina kadar
 * arar ve buldugunda adresini dondurur.
 * buyuk ve kucuk harfe duyarlidir.
 *
 * @param s1 : hedef karakter dizisi
 * @param s2 : aranacak karakter dizisi
 * @param len : maksimum karakter sayisi
 */
char *strnstr(const char *s1, const char *s2, size_t len){

	size_t l2;

	l2 = strlen(s2);	
	if (!l2)
		return (char *)s1;
	
	while (len >= l2) {
		len--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

/*
 * strncasestr,bir karakter dizisi icerisinde diger bir
 * karakter dizisini belli bir karakter sayisina kadar
 * arar ve buldugunda adresini dondurur.
 * buyuk ve kucuk harfe duyarli degildir.
 *
 * @param s1 : hedef karakter dizisi
 * @param s2 : aranacak karakter dizisi
 * @param len : maksimum karakter sayisi
 */
char *strncasestr(char *s1,char *s2, size_t len){

	size_t l2;

	l2 = strlen(s2);	
	if (!l2)
		return (char *)s1;
	
	while (len >= l2) {
		len--;
		if (!memcmp(strtolower(s1),strtolower(s2), l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

/*
 * strdup, kendisine verilen karakter dizisini
 * yeni bir bellek alani tahsisi yaparak kopyalar.
 * kisacasi kopyasini yada diger bir tabirle klonunu
 * olusturur.
 *
 * @param strdup : karakter dizisi 
 */
char * strdup(const char *string){
	
	size_t length = strlen(string);
	char * new_string = malloc(length + 1);
	memcpy(new_string,string,len+1);
	
	return new_string;

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
