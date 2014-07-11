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


uint32_t strlen(const char *s){
 
   uint32_t i = 0;
   while(s[i++]);
  
   return i;
  
}

int32_t strcmp(const char *s1,const char *s2){
 
   uint32_t i = 0;
   
   for(;;){
    
      if((!s1[i] ||  !s2[i]) && (s1[i] || s2[i]))
         return i;
         
      if(s1[i] != s2[i])
         return s1[i] - s2[i];
         
      if(!s1[i] && !s2[i])
         return 0;
    
      i++;
   }
   
}
