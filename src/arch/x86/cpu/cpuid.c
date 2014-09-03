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
#include <uniq/types.h>
#include <uniq/cpuid.h>
#include <string.h>


/*
 * cpuid, kendisine verilen op_num'a gore eax,ebx,ecx ve edx'e parametrelerini doldurur.
 * ornegin op_num(operation number) 0 oldugunda eger islemci cpuid'yi destekliyorsa
 * vendor_id'yi elde ederiz. cpuid, i486 islemcilerle birlikte eklenmistir.
 *
 * @param eax,ebx,ecx,edx : cpuid regs
 */
static inline void cpuid(uint32_t op_num,uint32_t *eax,uint32_t *ebx,uint32_t *ecx,uint32_t *edx){

	__asm__ volatile("cpuid"
			 : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
			 : "a" (op_num)
			 );

}

/*
 * verify_intel_cpu, kendisine parametre olarak verilen
 * cpuid kaydedicilerine gore cpu'nun imzalarinin 
 * intel cpu imzalariyla eslesip eslesmedigini kontrol eder.
 *
 * @param regs : cpuid kaydedicileri
 */
static inline bool verify_intel_cpu(cpuid_regs_t regs){

	if(regs.ebx != INTEL_SIGNATURE_EBX ||
	   regs.ecx != INTEL_SIGNATURE_ECX ||
	   regs.edx != INTEL_SIGNATURE_EDX)
		return false;   

	return true;
}

/*
 * verify_amd_cpu, kendisine parametre olarak verilen
 * cpuid kaydedicilerine gore cpu'nun imzalarinin 
 * amd cpu imzalariyla eslesip eslesmedigini kontrol eder.
 *
 * @param regs : cpuid kaydedicileri
 */
static inline bool verify_amd_cpu(cpuid_regs_t regs){

	if(regs.ebx != AMD_SIGNATURE_EBX ||
	   regs.ecx != AMD_SIGNATURE_ECX ||
	   regs.edx != AMD_SIGNATURE_EDX)
		return false;

	return true;
}

/*
 * have_cpuid, islemcinin cpuid'yi destekleyip desteklemedigini
 * kontrol eder. eger destekliyorsa "true",desteklemiyorsa "false"
 * doner.
 */
static bool have_cpuid(void){

	cpuid_regs_t regs;
	
	__asm__ volatile("pushf\n\t"				/* eflags'i stack'a at */
			 "pop %0\n\t"				/* eflags'i stacktan eax kaydedicisine at */
			 "mov %0,%1\n\t"			/* ecx'e de at, kontrol amacli kullanicaz */
			 "xor $0x200000, %0\n\t"		/* xor islemine tabi tut */
			 "push %0\n\t"				/* eax'i stack'a at */
			 "popf\n\t"				/* eflags'a geri al */
			 "pushf\n\t"				/* eflags'i tekrar stack'a at */
			 "pop %0\n\t"				/* eax kaydedicisine atalim tekrar */
			 : "=a"(regs.eax), "=c"(regs.ecx)
			 :
			 );

	/*
	 * eflags'i xor islemine tabi tutarak editlemistik.
	 * eger yaptigimiz degisiklik sifirlanirsa. ve
	 * kontrol amacli olarak editlemeden sakladigimiz ecx ile
	 * eax ayni olursa islemcinin cpuid'yi desteklemedigini
	 * anliyoruz.
	 */ 
	if(regs.eax == regs.ecx)
		return false;

	return true;

}

/* 
 * do_intel_cpuinfo,eger cpuid_info_t yapisina islemci
 * bilgilerini doldurur.(islemci intel ise)
 *
 * @param cpuid_info : cpuid_info_t yapisi
 */
static void do_intel_cpuinfo(cpuid_info_t *cpuid_info){

	strcpy(cpuid_info->vendor_id,INTEL_VENDOR_NAME);

}


/* 
 * do_amd_cpuinfo,eger cpuid_info_t yapisina islemci
 * bilgilerini doldurur.(islemci intel ise)
 *
 * @param cpuid_info : cpuid_info_t yapisi
 */
static void do_amd_cpuinfo(cpuid_info_t *cpuid_info){

	strcpy(cpuid_info->vendor_id,AMD_VENDOR_NAME);

}

/*
 * get_cpuid_info, islemci bilgilerini cpuid_info_t yapisina
 * doldurur.
 *
 * @param cpuid_info : cpuid_info_t yapisi
 */
bool get_cpuid_info(cpuid_info_t *cpuid_info){

	/* islemci cpuid'yi desteklemiyor :S */
	if(!have_cpuid())
		return false;

	cpuid_regs_t cpuid_regs;
	cpuid(CPUID_VENDOR_ID,&cpuid_regs.eax,&cpuid_regs.ebx,&cpuid_regs.ecx,&cpuid_regs.edx);

	if(verify_intel_cpu(cpuid_regs))		/* intel */
		do_intel_cpuinfo(cpuid_info);
	else if(verify_amd_cpu(cpuid_regs))		/* amd */
		do_amd_cpuinfo(cpuid_info);
	else{						/* bilinmeyen */
		strcpy(cpuid_info->vendor_id,"Unknown cpu vendor!");

		return false;
	}
			
	return true;
	
}

/*
 * dump_cpuid_info, verilen cpuid_info_t yapisini ekrana 
 * yazdiririr.
 *
 * @param cpuid_info : cpuid_info_t yapisi
 */
void dump_cpuid_info(cpuid_info_t *cpuid_info){

	debug_print(KERN_INFO,"Dumping the cpu info");
	debug_print(KERN_DUMP,"vendor_id : %s",cpuid_info->vendor_id);

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
