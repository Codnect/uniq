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


char *intel_old_cpu_list[] = {
	/* 0 */
	"Brand String Not Supported.", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Intel(R) Pentium(R) III Xeon(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) III processor-M", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Xeon(R) Processor", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) 4 processor-M", 
	"Mobile Intel(R) Pentium(R) Celeron(R) processor", 
	"Reserved", 
	"Mobile Genuine Intel(R) processor", 
	"Intel(R) Celeron(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Celeron(R) processor", 
	"Mobile Geniune Intel(R) processor", 
	"Intel(R) Pentium(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor"
	/* 1 */
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Celeron(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved"
};


/*
 * get_brand_string,verilen cpuid kaydedici iceriklerinden
 * brand string'ini olusturur.
 * 
 * @param brand, brand_string
 * @param regs, cpuid regs
 */
void get_brand_string(char *brand,cpuid_regs_t *regs){

	char brand_string[17];
	brand_string[16] = '\0';
	for(uint32_t i = 0; i < 4; i++){
		
		brand_string[i] = regs->eax >> (8 * i);
		brand_string[i + 4] = regs->ebx >> (8 * i);
		brand_string[i + 8] = regs->ecx >> (8 * i);
		brand_string[i + 12] = regs->edx >> (8 * i);
	
	}

	strncat(brand,brand_string,strlen(brand_string));

}

/*
 * cpuid, kendisine verilen op_num'a gore eax,ebx,ecx ve edx'e parametrelerini doldurur.
 * ornegin op_num(operation number) 0 oldugunda eger islemci cpuid'yi destekliyorsa
 * vendor_id'yi elde ederiz. cpuid, i486 islemcilerle birlikte eklenmistir. intel cpuid
 * pdf'sinde okuduguma gore bazi i386 islemci tiplerinde de kullanilmis.
 *
 * @param eax,ebx,ecx,edx : cpuid regs
 */
static void cpuid(uint32_t op_num,uint32_t *eax,uint32_t *ebx,uint32_t *ecx,uint32_t *edx){

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
static bool verify_intel_cpu(cpuid_regs_t *regs){

	if(regs->ebx != INTEL_SIGNATURE_EBX ||
	   regs->ecx != INTEL_SIGNATURE_ECX ||
	   regs->edx != INTEL_SIGNATURE_EDX)
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
static bool verify_amd_cpu(cpuid_regs_t *regs){

	if(regs->ebx != AMD_SIGNATURE_EBX ||
	   regs->ecx != AMD_SIGNATURE_ECX ||
	   regs->edx != AMD_SIGNATURE_EDX)
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
 * do_intel_cpuinfo, cpuid_info_t yapisina islemci
 * bilgilerini doldurur.(islemci intel ise)
 *
 * @param cpuid_info : cpuid_info_t yapisi
 */
static void do_intel_cpuinfo(cpuid_info_t *cpuid_info){

	cpuid_regs_t cpuid_regs;
	uint32_t max_eax,unused;
	/* vendor */
	strcpy(cpuid_info->vendor_id,INTEL_VENDOR_NAME);


	/* 
	 * http://www.microbe.cz/docs/CPUID.pdf detayli bir sekilde inceleyebilirsiniz.
	 *
	 * islemci detay 
	 * 
	 * EAX:
	 *
 	 * 31    28 27          20  19       16 15 14 13  12 11       8  7	    4 3         0
	 * ======================================================================================
	 * =  R   =  ext_family	 =  ext_model  =  R = type = family = model_no = stepping	=
         * ======================================================================================
 	 *
	 * EBX :
	 *
	 * 31                  23                     15                   7                    0
	 * ======================================================================================
	 * =      APIC ID       =       Count          =       Chunks      =       Brand ID     =
         * ======================================================================================
	 *
	 * ECX,EDX = feature flag
	 */
	
	/*
	 * islemci detay ve flaglar 
	 */
	cpuid(CPUID_PROCESSOR_DETAIL,&cpuid_regs.eax,&cpuid_regs.ebx,&cpuid_regs.ecx,&cpuid_regs.edx);
	cpuid_info->stepping = cpuid_regs.eax & 0xf;
	cpuid_info->model = (cpuid_regs.eax >> 4) & 0xf;
	cpuid_info->cpu_family = (cpuid_regs.eax >> 8) & 0xf;
	cpuid_info->extended_model = (cpuid_regs.eax >> 16) && 0xf;
	cpuid_info->extended_family = (cpuid_regs.eax >> 20) && 0xff;
	cpuid_info->flags_ecx = cpuid_regs.ecx;
	cpuid_info->flags_edx = cpuid_regs.edx;
	cpuid_info->processor = 0;			/* daha sonra halledicem ;) */

	uint32_t brand = cpuid_regs.ebx & 0xff;
	uint32_t signature = cpuid_regs.eax;

	/* 
	 * genisletilmis mi ? 
	 */
	cpuid(CPUID_EXTENDED,&max_eax,&unused,&unused,&unused);
	cpuid_info->brand_string[0] = '\0';

	if(max_eax >= 0x80000004) {
			for(uint32_t i = 0x80000002; i <= 0x80000004;i++){

				cpuid(i,&cpuid_regs.eax,&cpuid_regs.ebx,&cpuid_regs.ecx,&cpuid_regs.edx);
				get_brand_string(cpuid_info->brand_string,&cpuid_regs);

			}
	}
	else if(brand > 0){

		if(brand < 0x18){
			if(signature == 0x000006B1 || signature == 0x00000F13)
				strcpy(cpuid_info->brand_string,intel_old_cpu_list[brand + 24]);
			else
				strcpy(cpuid_info->brand_string,intel_old_cpu_list[brand]);
		}
		else
			strcpy(cpuid_info->brand_string,"reserved");

	}
	

}


/* 
 * do_amd_cpuinfo, cpuid_info_t yapisina islemci
 * bilgilerini doldurur.(islemci amd ise)
 *
 * @param cpuid_info : cpuid_info_t yapisi
 */
static void do_amd_cpuinfo(cpuid_info_t *cpuid_info){

	cpuid_regs_t cpuid_regs;
	uint32_t extended,unused;
	strcpy(cpuid_info->vendor_id,AMD_VENDOR_NAME);
	
	/*
	 * islemci detay ve flaglar 
	 */
	cpuid(CPUID_PROCESSOR_DETAIL,&cpuid_regs.eax,&cpuid_regs.ebx,&cpuid_regs.ecx,&cpuid_regs.edx);
	cpuid_info->stepping = cpuid_regs.eax & 0xf;
	cpuid_info->model = (cpuid_regs.eax >> 4) & 0xf;
	cpuid_info->cpu_family = (cpuid_regs.eax >> 8) & 0xf;
	cpuid_info->extended_model = (cpuid_regs.eax >> 16) && 0xf;
	cpuid_info->extended_family = (cpuid_regs.eax >> 20) && 0xff;
	cpuid_info->flags_ecx = cpuid_regs.ecx;
	cpuid_info->flags_edx = cpuid_regs.edx;
	cpuid_info->processor = 0;			/* daha sonra halledicem ;) */


	cpuid(CPUID_EXTENDED,&extended,&unused,&unused,&unused);
	cpuid_info->brand_string[0] = '\0';

	/* 
	 * genisletilmemis ise 
	 */
	if(!extended)
		return;

	if(extended >= 0x80000002) {
			for(uint32_t i = 0x80000002;i <= 0x80000004;i++){

				cpuid(i,&cpuid_regs.eax,&cpuid_regs.ebx,&cpuid_regs.ecx,&cpuid_regs.edx);
				get_brand_string(cpuid_info->brand_string,&cpuid_regs);

			}
	}

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

	if(verify_intel_cpu(&cpuid_regs))		/* intel islemci */
		do_intel_cpuinfo(cpuid_info);
	else if(verify_amd_cpu(&cpuid_regs))		/* amd islemci */
		do_amd_cpuinfo(cpuid_info);
	else{						/* bilinmeyen */

		strcpy(cpuid_info->vendor_id,"Unknown cpu vendor!");
		strcpy(cpuid_info->brand_string,"Not found brand string!");

		cpuid_info->stepping = 0;
		cpuid_info->model = 0;
		cpuid_info->cpu_family = 0;
		cpuid_info->extended_model = 0;
		cpuid_info->extended_family = 0;
		cpuid_info->flags_ecx = 0;
		cpuid_info->flags_edx = 0;
		cpuid_info->processor = 0;

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
	debug_print(KERN_DUMP,"brand_string : %s",cpuid_info->brand_string);
	debug_print(KERN_DUMP,"cpu_family : %u",cpuid_info->cpu_family);
	debug_print(KERN_DUMP,"extended_family : %u",cpuid_info->extended_family);
	debug_print(KERN_DUMP,"model : %u",cpuid_info->model);
	debug_print(KERN_DUMP,"extended_model : %u",cpuid_info->extended_model);
	debug_print(KERN_DUMP,"stepping : %u",cpuid_info->stepping);

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
