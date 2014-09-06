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
 
#include <uniq/module.h>
#include <uniq/types.h>

/*
 * arg_parse, argumanlari ayristirir.
 *
 * @param boot_args : boot argumanlari
 */
void arg_parse(char *boot_args){
 

}

/*
 * arg_present, argumanin olup olmadigini kontrol
 * eder.
 *
 * @param kern_arg : arguman
 */
bool arg_present(char *kern_arg){
 
  return false;

}

/*
 * arg_get_values, argumanin degerlerini dondurur.
 * "arguman" = "arguman degerleri" seklindedir.
 * ornegin :
 * start=vga_mod gibi. 
 *
 * @param kern_arg : arguman
 */
char *arg_get_values(char *kern_arg){
 

 return NULL;
 
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
