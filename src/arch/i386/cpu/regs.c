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
#include <uniq/kernel.h>

/*
 * dump_regs, kaydedicilerin dokumunu ekrana yazdirir.
 * 
 * @param regs : kaydediciler yada yazmaclar hala hangi soylemi kullanicagimi
 *               kullansam bilmiyorum ama kaydedici demek kulaga daha hos
 *               geliyor ;).
 */
void dump_regs(struct registers_t *regs){
 	
	debug_print(KERN_EMERG, "Dump Registers");
	debug_print(KERN_DUMP,"General purpose registers:");
	debug_print(KERN_DUMP,"\teax = \033[1;37m%10P \033[0m ecx = \033[1;37m%10P",regs->eax,
										    regs->ecx);
	debug_print(KERN_DUMP,"\t\033[0medx = \033[1;37m%10P \033[0m ebx = \033[1;37m%10P",regs->edx,
											   regs->ebx);
	debug_print(KERN_DUMP,"\t\033[0mesp = \033[1;37m%10P \033[0m ebp = \033[1;37m%10P",regs->esp,
											   regs->ebp);			
	debug_print(KERN_DUMP,"\t\033[0mesi = \033[1;37m%10P \033[0m edi = \033[1;37m%10P",regs->esi,
											   regs->edi);
	debug_print(KERN_DUMP,"\033[0mSegment selectors:");
	debug_print(KERN_DUMP,"\tds  = \033[1;37m%10P \033[0m es  = \033[1;37m%10P",regs->ds,
										    regs->es);				
	debug_print(KERN_DUMP,"\t\033[0mfs  = \033[1;37m%10P \033[0m gs  = \033[1;37m%10P",regs->fs,
										    	   regs->gs);				
	debug_print(KERN_DUMP,"\t\033[0mcs  = \033[1;37m%10P \033[0m ss  = \033[1;37m%10P",regs->cs,
										    	   regs->ss);
	debug_print(KERN_DUMP,"\033[0mOther:");
	debug_print(KERN_DUMP,"\t\033[0merror code  = \033[1;37m%10P",regs->err_code);
	debug_print(KERN_DUMP,"\t\033[0muser esp    = \033[1;37m%10P",regs->useresp);
	debug_print(KERN_DUMP,"\t\033[0meip         = \033[1;37m%10P",regs->eip);
	debug_print(KERN_DUMP,"\t\033[0meflags      = \033[1;37m%10P",regs->eflags);

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
