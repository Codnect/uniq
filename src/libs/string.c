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

size_t strlen(const char *s){
 
   size_t i = 0;
   while(s[i++]);
  
   return i;
  
}



int strcmp(const char *s1,const char *s2){
 
   size_t i = 0;
   
   for(;;){
    
      if(s1[i] != s2[i])
         return (s1[i] > s2[i]) : 1 : -1;
         
      if(!s1[i])
         break;
      i++;
      
   }
   
   return 0;
   
}

int strcasecmp(const char *s1,const char *s2){
 
  int c1,c2,i = 0;
  
  for(;;){
     
     c1 = tolower(s1[i]);
     c2 = tolower(s2[i]);
     
     if(c1 != c2)
        return (c1 > c2) : 1 : -1;
        
     if(!c1)
        break;
   
  }
  
  return 0;
  
}
