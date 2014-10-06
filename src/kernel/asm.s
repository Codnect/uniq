;
;  Copyright(C) 2014 Codnect Team
;  Copyright(C) 2014 Burak Köken
;
;  This file is part of Uniq.
;  
;  Uniq is free software: you can redistribute it and/or modify it under the
;  terms of the GNU General Public License as published by the Free Software
;  Foundation, version 2 of the License.
;
;  Uniq is distributed in the hope that it will be useful, but WITHOUT ANY
;  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
;  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
;  details.
;
;  You should have received a copy of the GNU General Public License along
;  with Uniq.  If not, see <http://www.gnu.org/licenses/>.
;

BITS 32

global copy_page_phys
copy_page_phys:
		push ebx
		pushf
		cli
		
		mov ebx, [esp + 12]
		mov ecx, [esp + 16]
		
		mov edx, cr0
		and edx, 0x7fffffff
		mov cr0, edx
		
		mov edx, 0x400

.page_loop:
		mov eax, [ebx]
		mov [ecx], eax
		
		add ebx, 0x4
		add ecx, 0x4
		dec edx
		
		jnz .page_loop
		
		mov edx, cr0
		or  edx, 0x80000000
		mov cr0, edx
		
		popf
		pop ebx
		ret
