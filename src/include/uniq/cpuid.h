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

#ifndef __UNIQ_CPUID_H__
#define __UNIQ_CPUID_H__

#include <uniq/types.h>

typedef struct{
	uint32_t eax,ebx,ecx,edx;
}cpuid_regs_t;

typedef struct{
	char vendor_id[16];
	uint8_t cpu_family;
}cpuid_info_t;

#define CPUID_VENDOR_ID			0x00000000

#define AMD_VENDOR_NAME			"AuthenticAMD"
#define AMD_SIGNATURE_EBX		0x68747541
#define AMD_SIGNATURE_ECX		0x444d4163
#define AMD_SIGNATURE_EDX		0x69746e65

#define INTEL_VENDOR_NAME		"GenuineIntel"
#define INTEL_SIGNATURE_EBX		0x756e6547
#define INTEL_SIGNATURE_ECX		0x6c65746e
#define INTEL_SIGNATURE_EDX		0x49656e69

bool get_cpuid_info(cpuid_info_t *cpuid_info);
void dump_cpuid_info(cpuid_info_t *cpuid_info);

#endif /* __UNIQ_CPUID_H__ */
