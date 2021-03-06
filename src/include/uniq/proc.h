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

#ifndef __UNIQ_PROC_H__
#define __UNIQ_PROC_H__

#include <uniq/types.h>
#include <mm/mem.h>

#define PROCESS_STARTED			0x1
#define PROCESS_RUNNING			0x2
#define PROCESS_FINISHED		0x4
#define PROCESS_PREUMASK		022

typedef struct{
	uint32_t ebp;			/* base pointer */
	uint32_t esp;			/* stack pointer */
	uint32_t eip;			/* instruction pointer */
	page_dir_t *page_dir;		/* sayfa dizini */
}thread_t;

typedef struct{
  	pid_t	id;			/* surec id */
	char *name;			/* surec ismi */
	char *description;		/* surec aciklamasi */
	char **cmdline;
	
	thread_t thread;		/* thread */
	uint32_t flags;			/* flaglar */
	uint32_t umask;	
}process_t;


#endif /* __UNIQ_PROC_H__ */
