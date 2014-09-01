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
#include <uniq/regs.h>
#include <uniq/kernel.h>

/*
 * teorik bilgiler
 * ---------------------------------------------------------------------------------------------------------
 *
 * 8259 PIC nedir?
 * 8259 PIC, IBM firmasi tarafindan uretilmis bir kesme denetleyicisidir. PIC( Programmable Interrupt 
 * Controller )'i turkce ifadesiyle programlanabilir kesme denetleyicisidir. IBM firmasi tarafindan ilk
 * uretildiginde 8 kesmeye kadar destekliyordu fakat donanim araclarin gelismesi ve artmasiyla  bi 8 tane 
 * kesme eklenerek bu model cikartilmistir. gunumuzde ise bu kadar kesmenin yetmeyecegi dusunulerek intel
 * tarafindan APIC yani gelismis kesme denetleyicisi cikarilmistir yanlis hatirlamiyorsam 240 adet kesmeyi
 * destekliyor. 8259 PIC'te master ve slave PIC olarak iki PIC bulunmakadir. bunlardan master'i birincil
 * PIC olarak slave PIC'den ise ikincil PIC olarak bahsetmek bahsetmek daha iyi gibi duruyor :). herneyse
 * her ikisi soylemide soylesek yanlis olmaz dimi.
 *
 * peki bu kesme listesine bi goze atalim.
 *
 *      Master PIC
 *     ===============================================================================
 *     = 8259 PIN = Kesme Numarasi = Aciklama					     = 
 *     ===============================================================================
 *     = IRQ0	  = 0x08	   = Zamanlayici				     =
 *     = IRQ1	  = 0x09	   = Klavye					     =
 *     = IRQ2	  = 0x0A           = Slave PIC baglantisi icin			     =
 *     = IRQ3     = 0x0B           = Serial port 2 (COM2)			     =
 *     = IRQ4	  = 0x0C	   = Serial port 1 (COM1)			     =
 *     = IRQ5	  = 0x0D	   = LPT(LinePrinTer)
 *     = IRQ6     = 0x0E	   = Disket surucusu				     =
 *     = IRQ7	  = 0x0F	   = Paralel Port 1				     =
 *     ===============================================================================
 *
 * *   Slave PIC
 *     ===============================================================================
 *     = 8259 PIN = Kesme Numarasi = Aciklama					     = 
 *     ===============================================================================
 *     = IRQ8/IRQ0  =	0x70	   = CMOS - Gercek zaman saati                       =
 *     = IRQ9/IRQ1  =	0x71	   = Reserve edilmis				     =
 *     = IRQ10/IRQ2 =	0x72	   = Reserve Edilmis				     =
 *     = IRQ11/IRQ3 =	0x73	   = Reserve Edilmis				     =
 *     = IRQ12/IRQ4 =	0x74	   = Mouse					     =
 *     = IRQ13/IRQ5 =	0x75	   = FPU					     =
 *     = IRQ14/IRQ6 =	0x76	   = Hard disk denetleyicisi 1			     =
 *     = IRQ15/IRQ7 =	0x77	   = Hard disk denetleyicisi 2			     =
 *     ===============================================================================
 *
 *  !Not : kesme numaralari varsayilandir, irq numaralarini duzenleyecegiz. Ve IRQ2
 *  ------ aslinda bir kesme degildir Slave PIC'in baglantisi yapmak icin ayrilmistir.
 *	   boylelikle slave PIC'i kullanabiliriz. Asagidaki sekildeki gibidir.
 *
 *									Slave PIC
 * 							=======================================
 *							= 8 = 9 = 10 = 11 = 12 = 13 = 14 = 15 =
 *							=======================================
 * 						       |
 *						       |
 *	Master PIC				       |
 *   =================================		       |
 *   = 0 = 1 = 2 = 3 = 4 = 5 = 6 = 7 =		       |
 *   =================================		       |
 *             |				       |
 *             |---------------------------------------|
 *
 *  ve son olarak kesme oncelikleri asagidaki gibidir.
 *  0 > 1 > 8 > 9 > 10 > 11 > 12 > 13 > 14 > 15 > 3 > 4 > 5 > 6 > 7 
 *
 * ---------------------------------------------------------------------------------------------------------
 */


/*
 * irq isleyicileri
 */
extern void _irq0(void);
extern void _irq1(void);
extern void _irq2(void);
extern void _irq3(void);
extern void _irq4(void);
extern void _irq5(void);
extern void _irq6(void);
extern void _irq7(void);
extern void _irq8(void);
extern void _irq9(void);
extern void _irq10(void);
extern void _irq11(void);
extern void _irq12(void);
extern void _irq13(void);
extern void _irq14(void);
extern void _irq15(void);

#define MAX_IRQ  			16
#define INT_GATE_TYPE			0xE
#define INT_PRESENT			0x80
#define INT_GATE			INT_PRESENT | INT_GATE_TYPE	/* kesme kapisi */
#define KERN_CODE_SEGMENT		0x8

/*
 * PIC tanimlamalari
 * PIC'leri ayarlamak icin toplamda 4 adet ICW'vardir.ICW
 * kesme kontrol olarak aklinizda tutabilirsiniz.
 *
 */
#define PIC_MASTER_COMMAND		0x20
#define PIC_MASTER_DATA			0x21
#define PIC_SLAVE_COMMAND		0xA0
#define PIC_SLAVE_DATA			0xA1
#define PIC_EOI				0x20

/*
 * ICW'ler
 */
#define ICW1_ICW4			0x01	/* ICW4'de var */
#define ICW1_SINGLE			0x02	/* tek PIC */
#define ICW1_INIT			0x10	/* baslatma icin gerekli */
#define ICW1				ICW1_ICW4 | ICW1_INIT
#define ICW2_MASTER_PIC_OFFSET		0x20	/* ilk donanim kesme numarasinin 0x20(32) oldugunu
						 * hatirlayin :)
						 */
#define ICW2_SLAVE_PIC_OFFSET		0x28	/* 8 kesme sonrasi slave PIC kesmelerinin baslama
						 * noktasi
						 */
#define ICW3_MASTER_PIC			0x04	/* IRQ2'nin kesme numarasini kadar bit sola kaydirilmis
						 * hali  "0 1 0 0". IRQ2 nin slave PIC ile master PIC
					 	 * baglanti noktasi oldugunu hatirlayin.
					 	*/
#define ICW3_SLAVE_PIC			0x02	/* IRQ2'nin kesme numarasinin binary formati "10" = "2" */
#define ICW4_8086_MODE			0x01	/* x86 modu */
#define ICW4				ICW4_8086_MODE
#define ICW_NULL			0x0

static int_handler_t irq_handlers[MAX_IRQ] = { NULL };

/*
 * irq_add_handler, irq isleyicisi ekler.
 *
 * @param irq_num : irq numarasi
 * @param handler : kesme isleyicisi
 */
void irq_add_handler(uint8_t irq_num, int_handler_t handler){
	
	if(irq_num >= MAX_IRQ)
		return;
		
	irq_handlers[irq_num] = handler;
	
}

/*
 * irq_remove_handler, irq isleyicisini kaldirir.
 *
 * @param irq_num : irq numarasi
 */
void irq_remove_handler(uint8_t irq_num){

	if(irq_num >= MAX_IRQ)
		return;
		
	irq_handlers[irq_num] = NULL;
	
}

/*
 * irq_set_gates, donanim kesmelerini idt tablosuna ekler.
 */
static void irq_set_gates(void){
	
	idt_set_gate(32, _irq0, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(33, _irq1, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(34, _irq2, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(35, _irq3, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(36, _irq4, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(37, _irq5, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(38, _irq6, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(39, _irq7, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(40, _irq8, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(41, _irq9, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(42, _irq10, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(43, _irq11, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(44, _irq12, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(45, _irq13, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(46, _irq14, KERN_CODE_SEGMENT, INT_GATE);
	idt_set_gate(47, _irq15, KERN_CODE_SEGMENT, INT_GATE); 
 
}

/*
 * irq_remap, irq kesmeleri icin PIC'yi ayarlar.
 */
static void irq_remap(void){
	
	debug_print(KERN_INFO,"Initializing the Master & Slave PIC.");
	/* PIC'leri baslat - ICW1 */
	outbyte(PIC_MASTER_COMMAND, ICW1);
	outbyte(PIC_SLAVE_COMMAND, ICW1);
	
	/* kesme offsetlerini ayarla - ICW2 */
	outbyte(PIC_MASTER_DATA, ICW2_MASTER_PIC_OFFSET);
	outbyte(PIC_SLAVE_DATA, ICW2_SLAVE_PIC_OFFSET);
	
	/* */
	outbyte(PIC_MASTER_DATA, ICW3_MASTER_PIC);
	outbyte(PIC_SLAVE_DATA, ICW3_SLAVE_PIC);
	
	/* x86 modu - ICW4 */
	outbyte(PIC_MASTER_DATA, ICW4);
	outbyte(PIC_SLAVE_DATA, ICW4);
	
	/* null - ICW_NULL */
	outbyte(PIC_MASTER_DATA, ICW_NULL);
	outbyte(PIC_SLAVE_DATA, ICW_NULL);
 
}

/*
 * irq_eoi, PIC'lere kesme sonu sinyali gonderir.
 * 
 * (END OF INTERRUPT) = EOI
 *
 * @param irq_num : irq numarasi
 */
void irq_eoi(uint8_t irq_num){

	if (irq_num >= 8) 
		outbyte(PIC_SLAVE_COMMAND, PIC_EOI);
		
	outbyte(PIC_MASTER_COMMAND, PIC_EOI);
	
}

/*
 * irq_handler, irq isleyicisidir.
 *
 * @param regs : kaydediciler. hangi kaydedicileri icerdigini ogrenmek
 *		 icin registers_t yapisini inceleyin.
 */
void irq_handler(registers_t *regs){

	int_handler_t handler = NULL;
	
	if(regs->int_num > 31  && regs->int_num < 48)
		handler = irq_handlers[regs->int_num - 32];
	
	if(!handler)
		irq_eoi(regs->int_num - 32);
	else
		handler(regs);
	
}

/*
 * irq_init, irq-donanim kesmelerini baslatir.
 */
void irq_init(void){
 
 	debug_print(KERN_INFO,"Initializing the IRQ.");
	irq_remap();
	irq_set_gates();
	enable_irq();

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
