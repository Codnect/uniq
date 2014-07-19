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

#include <uniq/version.h>

char * __kern_name = "uniq";
char * __kern_version_format = "%d.%d.%d-%s";

uint8_t __kern_version_major = 0;
uint8_t __kern_version_minor = 0;
uint8_t __kern_version_lower = 1;

#ifdef KERN_SPEC_SUFFIX
	#define STR(x) #x
	#define STRSTR(x) STR(x)
	#define KERN_VERSION_SUFFIX STRSTR(KERN_SPEC_SUFFIX)	/* ozel ekler icin */
#else
	#define KERN_VERSION_SUFFIX "beta"			/* beta,final */
#endif
char * __kern_version_suffix   = KERN_VERSION_SUFFIX;
char * __kern_version_codename = "nova";

char * __kern_arch = "i386";
char * __kern_build_date = __DATE__;
char * __kern_build_time = __TIME__;

#if (defined(__GNUC__) || defined(__GNUG__)) && !(defined(__clang__) || defined(__INTEL_COMPILER))
# define COMPILER_VERSION "gcc " __VERSION__
#else
# define COMPILER_VERSION "unknown compiler"
#endif

char * __kern_compiler_version = COMPILER_VERSION;
