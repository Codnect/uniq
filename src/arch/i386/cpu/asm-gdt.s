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

global gdt_load

gdt_load:
   mov eax, [esp+4]	; gonderilen parametreyi al
   lgdt [eax]		; gdt isaretcisi yukle

   ; korumali modda segment yazmaclarinin selektor degerlerini
   ; tuttugunu soylemistik, bunlari ayarlayalim.

   mov ax, 0x10		; 0x10 bizim veri segmentinin gdt'nin basindan itibaren
			; uzakligidir, yani 16 bayt(0x10).
   mov ds, ax
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax

   ; cs(kod segment) yazmacinin biraz daha farkli oluyor.
   jmp 0x08:.finish	; 0x8 gdt'nin basindan beri kod segment'in uzakligidir.

.finish:
   ret
