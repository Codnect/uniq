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


; isr ve irq makrolari
; tum isr ve irq'lara ayri ayri kod yazicagimiza isimizi
; makrolarlar halledelim ;)

; hata kodu icermeyen istisna kesmeleri icin macro
%macro ISR_NOERR 1			; 1 sayisi 1 adet parametre aldigini gosteriyor
	global _isr%1			; %1 , 1. parametre
	_isr%1:	
		cli			; kesmeleri devre disi birak
		push byte 0		; bos hata kodu stack'a atilir,registers_t yi 
					; alirken sikinti yasanmamasi icin
		push byte %1		; kesme numarasini sakla.
		jmp isr_common_entry	; ortak kesme servisi girisine zipla
%endmacro

; hata kodu iceren istisna kesmeleri icin macro
%macro ISR_ERR 1			; 1 sayisi 1 adet parametre aldigini gosteriyor
	global _isr%1			; %1 , 1. parametre
	_isr%1:
		cli			; kesmeleri devre disi birak
					; yukarida bos hata kodunu saklamistik,burda 
					; otomatik olarak hata kodu zaten islemci
					; tarafindan saklaniyor
		push byte %1		; kesme numarasini sakla
		jmp isr_common_stub	; ortak kesme servisi girisine zipla
%endmacro

; irq'lar icin macro
%macro IRQ_ENTRY 2			; 2 parametre aliyor
	global _irq%1			; %1,  1. parametre
	_irq%1:
		cli			; kesmeleri devre disi birak
		push byte 0		; bos hata kodu sakla
		push byte %2		; %2, 2.parametre. kesme numarasini sakla
		jmp irq_common_entry    ; irq'lar icin ortak girise zipla
%endmacro

extern fault_handler

; standart x86 kesme servisi rutinleri icin ortak giris
isr_common_entry:
  
  	; edi,esi,ebp,esp,ebx,edx,ecx,eax'leri sakla
	pusha
	
	; selektorleri sakla
	push ds
	push es
	push fs
	push gs
	
	; user modda cagrildiginda veri segmenti selektorleri 
	; farkli segmentleri gosterecekleri icin onlari yukarida
  	; stack'a attik. ve kernel mod veri segmentini tanimlayicisini
	; ayarliyoruz.
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; peki neden stack pointer'i sakladik diye dusunebilirsiniz.
	; fault_handler fonksiyonunuzu incelediginizde onun direk
	; registers_t yapisini almadigini goreceksiniz. bir pointer
	; aliyor yani adresini en son kaldigimiz yer bizim registers_t
	; yapisinin adresi olacaktir. ve adreside stack'a atiyoruz.
	; fonksiyon cagrildiginda parametre olarak bir isaretci lazimdi
	; ve bunu parametre olarak aliyor
	
	mov eax, esp          ; stack pointer'i eax kaydedicisine kopyala
	push eax              ; sakla
	call fault_handler    ; hata isleyicisini cagir
	
	; en son olarak stack'a attigimiz deger fault_handler fonksiyonunun
	; parametresi olan isaretcinin adresiydi yukarida bahsettigimiz gibi
	; fonksiyonun isi bittigine gore stack'tan alalim artik.
	pop eax
	
	; orjinal yada eski desek daha mi iyi olur bilmiyorum ama segment selektorleri
	; stack'tan alalim.
	pop gs
	pop fs
	pop es
	pop ds
	
	; edi,esi,ebp,esp,ebx,edx,ecx,eax'leri stack'tan al
	popa
	
	; kesme numarasi ve hata kodunu kaldir.("idt.c'yi inceleyin")
	add esp, 8
	
	; iret'ten bahsetmistik. bir kesme oldugunda calismakta olan fonksiyon
	; askiya alinir ve kesme yoneticisi cagrilir,islemci otomatik olarak kesme
	; yoneticisinin yigitina eip, cs, eflags, esp, ss kaydedicilerini atar. eger
	; askiya alinan fonksiyon ve kesme yoneticisi ayni ayricalik duzeyine sahip
	; olsa bir guvenlik sikintisi cikmaz fakat askiya alinan fonksiyonun user
	; modda olan bir fonksiyon olmasi guvenlik acigi meydana getirebilir. cunku
	; kesme yoneticisinin isi bittiginde otomatik olarak stack'a atilan bu
	; kaydediciler askiya alinan fonksiyonunun stack'inda da olur ve guvenlik
	; sikintisi cikar. iret fonksiyonu atilan bu kaydedicileri stack'tan geri
	; almamizi saglar.( iret = interrupt return)
	iret      ; eip, cs, eflags, esp ve ss'yi geri al


extern irq_handler

; irq'lar icin ortak giris
irq_common_entry:
  
  	; edi,esi,ebp,esp,ebx,edx,ecx,eax'leri sakla
	pusha
	
	; selektorleri sakla
	push ds
	push es
	push fs
	push gs
	
	; user modda cagrildiginda veri segmenti selektorleri 
	; farkli segmentleri gosterecekleri icin onlari yukarida
  	; stack'a attik. ve kernel mod veri segmentini tanimlayicisini
	; ayarliyoruz.
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; peki neden stack pointer'i sakladik diye dusunebilirsiniz.
	; fault_handler fonksiyonunuzu incelediginizde onun direk
	; registers_t yapisini almadigini goreceksiniz. bir pointer
	; aliyor yani adresini en son kaldigimiz yer bizim registers_t
	; yapisinin adresi olacaktir. ve adreside stack'a atiyoruz.
	; fonksiyon cagrildiginda parametre olarak bir isaretci lazimdi
	; ve bunu parametre olarak aliyor
	
	mov eax, esp          ; stack pointer'i eax kaydedicisine kopyala
	push eax              ; sakla
	call irq_handler      ; irq isleyicisini cagir
	
	; en son olarak stack'a attigimiz deger fault_handler fonksiyonunun
	; parametresi olan isaretcinin adresiydi yukarida bahsettigimiz gibi
	; fonksiyonun isi bittigine gore stack'tan alalim artik.
	pop eax
	
	; orjinal yada eski desek daha mi iyi olur bilmiyorum ama segment selektorleri
	; stack'tan alalim.
	pop gs
	pop fs
	pop es
	pop ds
	
	; edi,esi,ebp,esp,ebx,edx,ecx,eax'leri stack'tan al
	popa
	
	; kesme numarasi ve hata kodunu kaldir.("idt.c'yi inceleyin")
	add esp, 8
	
	; iret'ten bahsetmistik. bir kesme oldugunda calismakta olan fonksiyon
	; askiya alinir ve kesme yoneticisi cagrilir,islemci otomatik olarak kesme
	; yoneticisinin yigitina eip, cs, eflags, esp, ss kaydedicilerini atar. eger
	; askiya alinan fonksiyon ve kesme yoneticisi ayni ayricalik duzeyine sahip
	; olsa bir guvenlik sikintisi cikmaz fakat askiya alinan fonksiyonun user
	; modda olan bir fonksiyon olmasi guvenlik acigi meydana getirebilir. cunku
	; kesme yoneticisinin isi bittiginde otomatik olarak stack'a atilan bu
	; kaydediciler askiya alinan fonksiyonunun stack'inda da olur ve guvenlik
	; sikintisi cikar. iret fonksiyonu atilan bu kaydedicileri stack'tan geri
	; almamizi saglar.( iret = interrupt return)
	iret      ; eip, cs, eflags, esp ve ss'yi geri al
