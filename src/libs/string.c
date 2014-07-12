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

/*
 * strlen, verilen karakter dizisinin uzunlugu verir.
 * 
 * @param s : karakter dizisi 
 */
size_t strlen(const char *s){
 
	size_t i = 0;
	while(s[i++]);
	
	return i-1;
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
 * @param count : karsilastirilacak karakter sayisi 
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
 * @param count : karsilastirilacak karakter sayisi 
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
