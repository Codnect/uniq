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
#include <compiler.h>

char * __kern_name = "uniq";
char * __kern_version_format = "%d.%d.%d-%s";

int __kern_version_major = 0;
int __kern_version_minor = 0;
int __kern_version_lower = 1;

#define STR(x) #x
#define STRSTR(x) STR(x)

#ifdef KERN_SPEC_SUFFIX
	#define KERN_VERSION_SUFFIX STRSTR(KERN_SPEC_SUFFIX)	/* ozel ekler icin */
#else
	#define KERN_VERSION_SUFFIX "beta"			/* beta,final */
#endif
char * __kern_version_suffix   = KERN_VERSION_SUFFIX;
char * __kern_version_codename = "Dennis Ritchie";

char * __kern_arch = "i386";
char * __kern_build_date = __DATE__;
char * __kern_build_time = __TIME__;

char * __kern_compiler_version = COMPILER_VERSION;

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
