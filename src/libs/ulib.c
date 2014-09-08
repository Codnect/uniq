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

/*
 * swap,
 *
 * @param x,y :
 */
void swap(int32_t *x,int32_t *y){
    
    int32_t z = *x;
    *x = *y;
    *y = z;
  
}

/*
 * abs,
 *
 * @param x :
 */
int32_t abs(int32_t x){
    return (x < 0) ? -x : x; 
}

/*
 * max,
 *
 * @param x,y :
 */
int32_t max(int32_t x,int32_t y){
    return (x > y) ? x : y; 
}

/*
 * min,
 *
 * @param x,y :
 */
int32_t min(int32_t x,int32_t y){
    return (x > y) ? y : x;
}

/*
 * atoi,
 *
 * @param s :
 */
int atoi(const char *s){

	int i = 0;
	while(isdigit(*s))
		i = i * 10 + *s++ - '0';

	return i;
}

/*
 * krand,
 */
uint32_t krand(void){
	static uint32_t x = 123456789;
	static uint32_t y = 332437884;
	static uint32_t z = 608931198;
	static uint32_t w = 45678156;

	uint32_t t;

	t = x ^ (x << 11);
	x = y; y = z; z = w;

	return w = w ^ (w >> 19) ^ t ^ (t >> 8);

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
