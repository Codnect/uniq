;
;  Copyright(C) 2014 Codnect Team
;  Copyright(C) 2014 Burak Köken
;
;  This file is part of Nectlib.
;  
;  Nectlib is free software: you can redistribute it and/or modify it under the
;  terms of the GNU General Public License as published by the Free Software
;  Foundation, version 2 of the License.
;
;  Nectlib is distributed in the hope that it will be useful, but WITHOUT ANY
;  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
;  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
;  details.
;
;  You should have received a copy of the GNU General Public License along
;  with Uniq.  If not, see <http://www.gnu.org/licenses/>.
;


; flaglar
MBOOT_PAGE_ALIGN	equ	1<<0		; sayfa hizalamasi
MBOOT_MEM_INFO		equ	1<<1		; bellek bilgisi

MBOOT_HEADER_FLAGS	equ	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_HEADER_MAGIC	equ	0x1BADB002	; multiboot header magic number
MBOOT_CHECKSUM		equ	-(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


BITS 32						; tum talimatlar 32 bit olmalidir

global mboot_header
extern code
extern bss
extern end


mboot_header:
		dd	MBOOT_HEADER_MAGIC
		dd	MBOOT_HEADER_FLAGS
		dd	MBOOT_CHECKSUM
		dd	mboot_header		; multiboot header
		dd	code			; '.text' bolumu baslangici
		dd	bss			; '.data' bolumu sonu
		dd	end			; kernel sonu
		dd	kentry			; kernel giris noktasi (ilk EIP)


global kentry					; kernel giris noktasi
extern kmain

kentry:
		push eax			; multiboot onyuklecisi
						; magic number = 0x2BADB002

		; kerneli calistir
		cli				; kesmeleri kapat
		call kmain
		jmp $
