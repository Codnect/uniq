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
#include <uniq/types.h>
#include <uniq/kernel.h>
#include <string.h>

/*
 * -Genel bilgiler-
 * --------------------------------------------------------------------------------------------
 *
 * -IDT nedir?-
 * IDT kesme ve istisnalarin tutuldugu bir tablodur ve 256 adet girdi icerebilir fakat ilk elemeani gdt
 * ve ldt tablolari gibi null degildir. 256 adet girdi icermesinin sebebine gelicek olursak toplamda
 * 256 kesme bulumaktadir. bu 256 kesmeden ilk 32'si intel tarafindan istisnalar olarak belirlenmistir. 
 * geriye kalan kesmeler ise donanim ve yazilim kesmeler icin ayrilmistir.
 * 
 * bu ayrilan 32 istisnalar sunlardir;
 *
 * 1.Divide by 0 (Hata)
 * 2.Debug (Hata/Tuzak)
 * 3.NMI (Donanım)
 * 4.Breakpoint (Tuzak)
 * 5.Overflow (Tuzak)
 * 6.Bound Range Exceeded (Hata)
 * 7.Invalid Opcode (Hata)
 * 8.Device Not Available (Hata)
 * 9.Double fault (Bozulma)
 * 10.Coprocessor (Bozulma)
 * 11.Invalid TSS (Hata)
 * 12.Segment not present (Hata)
 * 13.Stack Segment Fault (Hata)
 * 14.General Protectionn (Hata)
 * 15.Page Fault(Hata)
 * 16.Rezerved
 * 17.Floating Point Fault (Hata)
 * 18–32.Rezerved
 *
 * Bozulma : ciddi istisna durumlaridir.
 * ---------
 *
 * IDTR : Interrupt Descriptor Table Register
 * -----  IDT tablosuna isaret eder ve IDT kesmeleri yonetecek fonksiyonlarin adreslerini tutar.
 *
 * IDTR yazmaci asagidaki gibidir.
 *
 * 47								16 15			     0	
 * ===========================================================================================
 * =                         taban adres 			 =	   limit	     =
 * ===========================================================================================
 *
 * -kapi olayi?-
 *
 * eger "gdt.c" dosyasinin incelemediyseniz incelemenizde fayda var. orda bu paragrafa rastlamaniz mumkun 
 * fakat burda da benzer bir aciklama yapacagimdan kopyalamayi uygun buldum ;).kapilar basit olarak tanimlamak 
 * istersek farkli ayricalik seviyesindeki segmentlerin birbirleriyle bazen iletisim kurmasi  gerekmektedir ve 
 * bunu saglamak amaciyla dusunulmus yapilardir diyebiliriz. ornegin kullanici  modundaki bir programin kernel
 * modda calisan servislere ihtiyac duyar ve bu servislere erisme  isini sistem cagrilariyla yapar. sistem 
 * cagrilarida kapilar yardimiyla olmaktadir. anlatabildigim kadariyla artik ;). simdi gelelim kapi cesitleri 
 * yukarida sistem segment turleri arasinda zaten gozukuyorlar ama aciklamakta yarar var. asagida kapi cesitleri 
 * ve aciklamalarini gorebilirsiniz.
 *
 * -> kesme kapilari(interrupt gates)
 * -> gorev kapilari(task gates)
 * -> cagirma kapilari(call gates)
 * -> yazilim kesme kapilari/tuzak kapilari(trap gates)
 *
 * --cagirma kapilari--
 * farkli ayricalik seviyesindeki programlarin birbirlerinin segmentlerine erismesini saglar. gdt ve ldt tablolarinda
 * bulunabilirler. cagirma kapilari "jmp" ve "call" komutlari ile gerceklesir. yapisi asagidaki gibidir.
 *
 *
 * ========================================================================================================
 * =       base_high       =     flags     =     zero    =       selector        =        base_low        =
 * =                       =               =	         =                       =                        =
 * ========================================================================================================
 *          16 bit              8 bit          8 bit               16 bit                 16 bit
 *
 * flags :
 * -------  7     6      5 4      3    0
 *	    ============================
 *	    =  P =  DPL  =  DT  = TYPE =
 *	    ============================
 *
 *	    -> Type = sistem segmenti olacagi icin asagidaki sistem segment turlerinden biri
 *	    --------- olacaktir.yani (1 1 0 0)
 *	    -> DT   = sistem segmenti oldugu icin 0 degerini alicak.
 *	    ---------
 *	    -> DPL  = segment ayricalik seviyesini belirtir.
 *	    ---------
 *	    -> P    = segmentin o an bellekte olup olmadigi belirtir. eger segment bellekte ise bu
 *	    --------- bit 1 olur. eger 0 iken segmente erisilmeye calisilirsa "segment not present"
 *		      hatasi olusur.
 *
 * zero:    
 * ----     7           5 4              0
 *	    ==============================
 *	    =  0 0 0     =      count    =
 *	    ==============================
 *          
 *          -> count = cagiran programin stack bolgesinden kac adet 32 bitlik veri kopyalayacagini belirtir.
 *          ----------
 *
 *
 *  =========================================================================
 *  ==	               Sistem segment tanimlayicisi turleri                ==
 *  =========================================================================
 *  = 0 = 0 = 0 = 0 = 		   rezerve edilmis                          =
 *  =========================================================================
 *  = 0 = 0 = 0 = 1 =              16 bit tss (hazir)                       =
 *  =========================================================================
 *  = 0 = 0 = 1 = 0 =              LDT tablosu                              =
 *  =========================================================================
 *  = 0 = 0 = 1 = 1 =              16 bit tss (mesgul)                      =
 *  =========================================================================
 *  = 0 = 1 = 0 = 0 =              16 bit cagri kapisi                      =
 *  =========================================================================
 *  = 0 = 1 = 0 = 1 =                gorev kapisi                           =
 *  =========================================================================
 *  = 0 = 1 = 1 = 0 =              16 bit kesme kapisi                      =
 *  =========================================================================
 *  = 0 = 1 = 1 = 1 =              16 bit tuzak kapisi                      =
 *  =========================================================================
 *  = 1 = 0 = 0 = 0 = 		   rezerve edilmis                          =
 *  =========================================================================
 *  = 1 = 0 = 0 = 1 =              32 bit tss (hazir)                       =
 *  =========================================================================
 *  = 1 = 0 = 1 = 0 = 		   rezerve edilmis                          =
 *  =========================================================================
 *  = 1 = 0 = 1 = 1 =             32 bit tss (mesgul)                       =
 *  =========================================================================
 *  = 1 = 1 = 0 = 0 =              32 bit cagri kapisi                      =
 *  =========================================================================
 *  = 1 = 1 = 0 = 1 = 		   rezerve edilmis                          =
 *  =========================================================================
 *  = 1 = 1 = 1 = 0 =             32 bit kesme kapisi                       =
 *  =========================================================================
 *  = 1 = 1 = 1 = 1 =              32 bit tuzak kapisi                      =
 *  =========================================================================
 *
 * --kesme kapilari--
 * yapi olarak cagirma kapilariyla cok benzerdir. yapisi asagidaki gibidir ve cagirma
 * kapilarinda farkindan bahsetmek gerekirse yukaridaki tablodan goruldugu gibi TYPE
 * kismi "1 1 1 0" olmali ve count kismi reserve edilmistir.ve son olarak kesme kapilari
 * "int" komutu yada donanim kesmeleriyle meydana gelebilir.
 *
 * ========================================================================================================
 * =       base_high       =     flags     =     zero    =       selector        =        base_low        =
 * =                       =               =	         =                       =                        =
 * ========================================================================================================
 *          16 bit              8 bit          8 bit               16 bit                 16 bit
 *
 * flags: flags yapisi cagirim kapilariyla ayni oldugu icin bahsetmeye gerek yok.
 * -----
 *
 * zero:    
 * ----     7           5 4              0
 *	    ==============================
 *	    =  0 0 0     =   reserved    =
 *	    ==============================
 *
 * --tuzak kapilari(yazilim kesmeleri)--
 * yapi olarak cagirma ve kesme kapilariyla cok benzerdir. yapisi asagidaki gibidir ve cagirma
 * kapilarinda farkindan bahsetmek gerekirse yukaridaki tablodan goruldugu gibi TYPE
 * kismi "1 1 1 1" olmali ve count kismi reserve edilmistir.ve kesme kapilarinda oldugu gibi 
 * "int" komutu  yada donanim kesmeleriyle meydana gelebilir.son olarak sundan da bahsetmek 
 * istiyorum. ikiside EFLAGS yazmacındaki IF flagini farkli sekilde degistirirler. kesme kapilari
 * EFLAGS yazmaci yigina atildiktan sonra IF(Interrupt Enabled) flagini sifirlar,boylece kesme
 * kapisindayken bir donanim kesmesinin meydana gelmesini engeller. tuzak kapilarinda ise sifirlama
 * olmadigi icin bir kesme olusabilir.
 *
 * ========================================================================================================
 * =       base_high       =     flags     =     zero    =       selector        =        base_low        =
 * =                       =               =	         =                       =                        =
 * ========================================================================================================
 *          16 bit              8 bit          8 bit               16 bit                 16 bit
 *
 * flags: flags yapisi cagirim kapilariyla ayni oldugu icin bahsetmeye gerek yok.
 * -----
 *
 * zero: zero yapisi kesme kapilariyla ayni oldugu icin bahsetmeye gerek yok.
 * -----
 *
 * --gorev kapilari--
 * bu kapilarin anlatilmasinin su anlik yeri degil gibi yeri ve zamani geldigi zaman aciklama
 * satirlari halinde gorebilirsiniz :). yukaridaki tablodan goruldugu gibi TYPE kismi "0 1 0 1"
 * olacagini soylesek simdilik yeterli gibi.
 *
 *
 * ---------------------------------------------------------------------------------------------------------------
 *
 * -kesme ve istisnalarin yonetimi-
 * herhangi bir kesme oldugunda kesme numarasi "struct idt_entry_t" boyutu yani 8 baytla carpilir
 * ve IDT'ten tanimliyiciya ulasilir. IDT girdilerinin (idt_entry_t) 8 bayt oldugunu unutmayin :).
 * sistemde bir kesme olustugunda EFLAGS,CS ve EIP yazmaclari yigitta saklanir bunun yaninda
 * bir hata kodu dondurulurse o da yigitta saklanir. eger bu kesme ayni ayricalik seviyesinde
 * olusursa kesme yoneticisinin yigitida asagidaki gibidir.
 *
 * -> ayni ayricalik duzeyi
 *  
 *  kesmeyle askiya alan fonksiyonun yigiti
 *  ============
 *  =          =  <-- kesme cagrimindan once "esp
 *  ============
 *  = EFLAGS   =
 *  ============
 *  =   CS     =
 *  ============
 *  =   EIP    =
 *  ============
 *  = ErrCode  =  <-- kesme yoneticisi sonlandirildiktan sonraki "esp"
 *  ============
 *  =          =
 *  ============
 *
 * -> farkli ayricalik duzeyi olursa
 * bu sefer olay biraz degisir ayni ayricalik duzeyinde olmayacagi icin bir yigit degisimi
 * olmalidir. islemci kesme oldugunda kesme yoneticisi cagirir ve yukarida bahsettigimiz
 * saklanan degerler kesme yoneticisinin yigitindadir. ve kesme yoneticisi islemi bittiginda
 * askiya alinan fonksiyonun yigitin SS,ESP,EFLAGS,CS,EIP ve hata kodu degerlerini kopyalar.
 * bu guvenlik acigi olusturabilir bu yuzden kesme yoneticisini sonlandirmak icin "int(interrupt
 * return)" kullanilir. "iret", ile CS, EIP, EFLAGS, SS, and ESP degerleri yuklenerek yigittan
 * alinir ve bu problemde boyle cozulmus olur. dikkat edilmesi gerek diger bir konuda yigit
 * degisiminin oldugu durumlarda "iret" kullanilmalidir.
 *
 *
 *  kesmeyle askiya alan fonksiyonun yigiti                     kesme yoneticisinin yigiti
 *  ============						============		
 *  =          =  <-- kesme cagrimindan once "esp"		=          =
 *  ============						============
 *  =   SS     = 						=    SS    =
 *  ============						============
 *  =   ESP    =						=   ESP    =
 *  ============						============
 *  = EFLAGS   =						=  EFLAGS  =
 *  ============						============
 *  =   CS     =						=   CS     =
 *  ============						============
 *  =   EIP    =						=   EIP    =
 *  ============						============
 *  = ErrCode  = 	Kesme yoneticisinin islemlerinden -->	= ErrCode  =
 *  ============			sonra "esp"		============
 *  =          =						=          =
 *  ============						============
 *
 * ---------------------------------------------------------------------------------------------------------------
 *
 */


/*
 * idt_entry_t, idt tablosunun girdi yada diger bir tabirler
 * tanimlayici yapisidir.
 */
struct idt_entry_t{
	uint16_t base_low;	/* atlanicak fonksiyonunun taban adresinin ilk 16 biti */
	uint16_t sel;		/* kernel segment selektor */
	uint8_t zero;		/* genellikle sifir */
	uint8_t flags;		/* flaglar */
	uint16_t base_high;	/* atlanicak fonksiyonun taban adresini son 16 biti */
} __attribute__((packed));


/*
 * idt_ptr_t, idt tablosunun adresini ve limitini tutan ozel
 * bir yapidir. IDTR yazmacina dikkat edin :).
 */
struct idt_ptr_t{
	uint16_t limit;		/* idt'nin uzunlugu(bayt olarak) */
	uint32_t base;		/* idt'nin taban adresi */
} __attribute__((packed));

#define IDT_MAX_ENTRY	256
#define DPL3		0x60

struct idt_entry_t idt_entry[IDT_MAX_ENTRY];
struct idt_ptr_t idt_ptr;



extern void idt_load(uint32_t idt_ptr);

/*
 * idt_set_gate,idt tablosunun girdilerinin ayarlanmasini saglar.
 *
 * @param num : idt tablo kesme index'i
 * @param base : kesme icin ayarlanan fonksiyonun adresi 
 * @param sel : kod segmenti offset (0x08)
 * @param flags : flaglar
 */
void idt_set_gate(uint8_t num,void (*base)(void),uint16_t sel,uint8_t flags){
	
	idt_entry[num].base_low		= ((uint32_t)base & 0xFFFF);
	idt_entry[num].base_high 	= ((uint32_t)base >> 16) & 0xFFFF;
	idt_entry[num].sel 		= sel;
	idt_entry[num].zero 		= 0;
	idt_entry[num].flags 		= flags /* | DPL3 */;
	
}



/*
 * init_idt,idt tablosunun hazirlanmasi ve yuklenmesi islemleri 
 * gerceklestirir.
 */
void init_idt(void){
	
 	debug_print(KERN_INFO,"Initializing the IDT. IDT table address is \033[1;37m%P",(uint32_t)&idt_entry);

	idt_ptr.limit = (sizeof(struct idt_entry_t) * IDT_MAX_ENTRY) - 1;
	idt_ptr.base = (uint32_t)&idt_entry;
	memset(&idt_entry, 0, sizeof(struct idt_entry_t) * IDT_MAX_ENTRY);
	idt_load((uint32_t)&idt_ptr);
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
