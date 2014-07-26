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

#include <uniq/types.h>
#include <uniq/module.h>

/*
 * -Genel bilgiler-
 * --------------------------------------------------------------------------------------------
 *
 * İşlemci ilk başlarken gerçek modda başlar. Gerçek modda 1 MiB'lik bellek
 * adresleyebilirken, korumali modda 32 bitlik yani 4 GiB'lik bellegi adresler.
 * Korumali modda bellek erisimi GDT,LDT tablolariyla gerceklesir.
 *
 * GDTR : Global Descriptor Table Register
 * -----  (GDT)-Global tanimlayici tablosuna isaret eder.
 *
 * LDTR : Local Descriptor Table Register
 * ----- LDTR, GDTR gibi direk tabloyu isaret etmez. GDT tablosundaki bir tanimlayiciya 
 *	 isaret eder. (LDT)-Yerel tanimlayici tablosunun adresi bu tanimlayici icindedir.
 *
 * IDTR : Interrupt Descriptor Table Register
 * -----  IDT tablosuna isaret eder ve kesmeleri yonetecek fonksiyonlarin adreslerini tutar.
 *
 * TR : Task Register
 * ---	TSS yapisina isaret eder. TSS yapisi surec ile ilgili tum bilgileri icerir.
 *
 * !Not : GDT ve LDT tablolari en fazla 8192 tanimlayici icerebilir.
 * -----
 *
 * CR0 : Control Register 0
 * ----	Korumalı mod ve sayfalama mekanizmasi icin
 *	CR0'in PG biti(31.bit) = 1 yapılırsa sayfalama aktif olur.
 *	CR0'in PE biti(1.bit)  = 1 yapılırsa gercek moddan,korumali mod'a gecilir
 *
 * CR1 : Control Register 1
 * ----	intel tarafindan rezerve edilmistir.
 *
 * CR2 : Control Register 2
 * ----	sayfalama hatasi olustugunda olusan sayfa adresini tutar.
 * ----	!dikkat : sayfalama aktifse.
 *
 * CR3 : Control Register 3
 *	sayfa dizin tablosunun fiziksel adresini tutar.
 *
 * CR4 : Control Register 4
 * ----	intel icin.
 *
 * --------------------------------------------------------------------------------------------
 * -Korumali Modda Bellek Yonetimi-
 *
 * Korumali modda bellek yonetimi 2 turlu gerceklesir. Sayfalama pasifse,sadece segmentasyon ile
 * sayfalama aktifse once segmentasyon sonra sayfalama asamalariyla gerceklesir
 * 
 * --------------------------------------------------------------------------------------------
 * -Segmentasyon nedir?-
 * Segmentasyon,sureclerin kod,veri ve yigin gibi bolgelerinin birbirinden ayrilmasidir
 * diyebiliriz. Bir surec baska bir surecin segmentine mudahele eder yani kendi segmentinin
 * disina cikarsa genel koruma hatasi(general protection fault) olusur ve surec sonlandirilir.
 *
 * --------------------------------------------------------------------------------------------
 * -Sayfalama nedir?-
 * Sayfalamayala birlikte sureclere sanal bellek servisi sunulur ve o surece ait sayfa dizini
 * ve sayfa tablolari yardimiyla,lineer bellek adresleri fiziksel adreslere donusturulur.
 *
 * !Not: Segmentasyon her zaman aktiftir,sayfalamayi ise aktif hale getirmek programcinin
 * ----  gorevidir.
 *
 * --------------------------------------------------------------------------------------------
 * -Gdt Nedir?-
 * GDT(Global Descriptor Table) bir tanimlayici tablosudur. En fazla 8192 tanimlayici icerir.
 * Bu tanimliyicilar her biri bellekteki bir segmentin ozelliklerini barindirmaktadir.
 * GDTR yazmaci bu tablonun bellek adresini tutar.
 *
 * --------------------------------------------------------------------------------------------
 * -Segmentasyon icin daha ayrintili bilgi...-
 *
 * Korumali modda,segment yazmaclari selektor denilen deger tutarlar. Bunlar GDT'den tanimlayici
 * secmeye yarar. Korumali modda bellek adresleri 2 bolumden olusur.16 bitlik bir selektor ve 
 * 32 bitlik bir offset degeridir.
 *
 * Oncelikle selektorle GDT'ten bir tanimlayici secilir. Tanimlayicidan alinan taban adresine
 * offset degeri eklenerek gercek adres elde edilir. Eger sayfalama aktif degilse bu adres
 * fizikseldir, aktif ise bu lineer adrestir ve fiziksel hafizadaki karsiligini bulmak icin
 * sayfa dizini,sayfa tablosu kullanilir.
 *
 * --------------------------------------------------------------------------------------------
 * -Sayfalama icin daha ayrintili bilgi...-
 *
 * Sayfalama surecler sanal bellek servisi sunarken bunun yaninda bellegin iyi bir sekilde de
 * kullabilme avantajini saglar. Ornegin bir surec calistiralim ve bellekten 38 KiB bellek 
 * kullansin. Daha sonra yeni bir surec daha calistiralim ve onceki sureci kapatalim bellekte
 * 38 KiB'lik yer bosaldi,fakat oraya 39 KiB'lik bir surec yerlestirmeye kalsak yerlestiremeyiz,
 * boyle boyle bellekte bosluk artar ve bellek kullanilamaz hal alir.
 *
 * Sayfalamada bu olayi onler,bellek 4 KiB'lik sayfalara ayrilmistir ve surec bellege yuklenmeden
 * once sayfalara ayrilir. Daha sonra bu 4 KiB'lik sayfalara yuklenir. Surec sonlandirildiginda
 * arkasinda 4 KiB'lik bosluklar birakir. Ve yeni surecte bellege yuklenmeden sayfalara ayrilacagi
 * icin bu bosluklara rahatlikla yerlestirilir.Ve boylelikle bellek en iyi sekilde kullanilabilir
 * Gercekten guzel dusunulmus ;)
 *
 * Sayfalama CR0 yazmacinin 31.biti (PG) "1" yapilarak aktif hale getirilir. Sayfalama aktifken adres
 * donusumunun nasil oldugu hakkinda bilgi edinelim ;).
 *
 * Korumali modda 16 bitlik selektor ve 32 bitlik offset degerinden olusan adres segmentasyonla 32 bitlik
 * bir adres olusuyordu. Sayfalama aktif degilse fizikseldi,aktif ise lineer bir adresti. Sayfalama aktif 
 * ise bu lineer adresin fiziksel adresi donusmesi icin sayfa dizini ve sayfa tablosu kullanilir.
 *
 * Sayfa tablosu : Sayfalarin( 4 KiB) bellekteki fiziksel adres karsiliklarini tutar. Kısaca bir adres
 * -------------- tablosudur.
 *
 * Sayfa dizini : Sayfa tablolarinin bellekteki fiziksel adreslerini tutar.
 * -------------
 * -----------------------------------------------------------------------------------------------------------
 *
 * !Not : Her surecin bir adet sayfa dizini vardir ancak birden fazla sayfa tablosu olabilir. Her
 * ---- sayfa tablosu en fazla 1024 girdiye sahip olabilir. Sayfa tablosu boylelikle 4 MiB'lik bir bellek
 *      adresleyebilir(1024 * 1024 * 4).Eger surec 7 MiB bellek kullaniyorsa 1 adet sayfa dizini 2 adet sayfa 
 *      tablosu tahsis edilir. Sayfa dizini de sayfa tablolari gibi 1024 girdiye sahip olabilir. O zaman 
 *      1024 * 1024 * 1024 * 4 = 4 GiB'lik bir adres alani ayrilabilir.Iste boylelikle 32 bitlik islemcinin
 *      adresleyebilicegi tum bellek kullanilabilmektedir.
 *
 * -----------------------------------------------------------------------------------------------------------
 *
 * Her surecin dedigimiz gibi bir sayfa dizini vardi ve birden sayfa tablosu olabilir, sayfa tablolarina
 * sayfa dizini ile erisilebilir ancak sayfa dizinine erismek lazim.Peki bu nasil olacak en ust bilgilerde
 * belirtildigi gibi "CR3" sayfa dizin tablosunum adresini tutar. Korumali modda sayfalama acikken bellek
 * erisimini soyle ozetleyebiliriz.
 *
 * - 16 bitlik selektor ve 32 bitlik bir offset ile bellege erisilmeye calisilir.
 * - GDTR'den GDT'nin adresi alinir.
 * - Segment kaydedicisindeki index numarasina gore GDT'den bir tanimlayici secilir.
 * - Secilen tanimlayicidaki taban adresi alinir ve offset degeri eklenir,lineer adres adres elde edilir.
 * - CR3 kaydedicisinden sayfa dizininin adresi alinir.
 * - 32 bitlik olusan lineer adresin ilk 10 bitindeki deger gore sayfa dizininden index secilir. (2^10 = 1024
 * ve sayfa dizininde 1024 girdi olabildigini hatirlayin ;).)
 * - Secilen girdide gosterilen fiziksel ile sayfa tablosuna ulasilir.Sayfa tablosundan da bir girdi secmek icin
 * lineer adresin ikinci 10 bitindeki degere bakilir.
 * - Sayfa tablosundaki girdiden sayfanin fiziksel adresi alinmistir ancak sayfa 4 KiB boyutundadir. Daha sonra
 * lineer adresin kalan 12 biti offset olarak kullanilir ve bu sayfa adresine eklenir. Elimizdeki sayfa 4 KiB
 * boyunta olup kalan 12 biti offset olarak kullanicaktik 2^12 = 4096 ile sayfanin her bitine erisilebilir.
 * - Tum bu islemlerle fiziksel adrese erisilir,fakat bu asamalar gerceklesirken bircok koruma testi de yapilir.
 *
 * ------------------------------------------------------------------------------------------------------------
 *
 * -Tanimlayicilar-
 * GDT ve LDT tablolarinin her bir elemaninin bir segment tanimlayicisiydi. Segment tanimlayicisi segment icin
 * gerekli tum bilgileri barindirir. 64 bit boyutunda olup en fazla 8192 tanimlayici icerebilir.
 *
 * ==============================================================================================
 * =  base_high  =  granularity  =   access =   base_middle  = 	  base_low     =   limit_low    =
 * =   24-31     =	         =	    =     16-23      =      0-15       =       0-15     =
 * ==============================================================================================
 *     8 bit          8 bit          8 bit          8 bit           16 bit             16 bit
 *
 * limit_low : segmentin taban adresinden itibaren boyutunu tutat 20 bitlik bolumunun 0-15 arasindaki
 * ----------- 16 biti
 * 
 * base_low :  segmentin taban adresinin ilk 16 biti
 * -----------
 *
 * base_middle: segmentin taban adresinin 16-23 arasindaki 8 biti
 * ------------
 *
 * access :  access bayt formati,erisim ile ilgili bilgileri icerir.
 * --------  
 *          7     6      5 4      3    0
 *	    ============================
 *	    =  P =  DPL  =  DT  = TYPE =
 *	    ============================
 *
 *	    -> Type = segment, kod segmenti mi? veri segmenti mi?
 *	    ---------
 *	    -> DT   = eger segment normal bir hafiza segmentiyse(kod,veri,yigin) 1 degerini alir.
 *	    ---------
 *	    -> DPL  = segment ayricalik seviyesini belirtir.
 *	    ---------
 *	    -> P    = segmentin o an bellekte olup olmadigi belirtir. eger segment bellekte ise bu
 *	    --------- bit 1 olur. eger 0 iken segmente erisilmeye calisilirsa "segment not present"
 *		      hatasi olusur.
 *
 * granularity: segment ile ilgili diger bilgileri icerir.
 * ------------
 *
 *          7     6      5    4      3            0
 *	    ========================================
 *	    =  G  =  D  =  O  =  A  =    segment   =
 *	    =     =     =     =     =      19:16   =
 *	    ========================================
 *	    
 *	    -> segment = segment limitinin son 4 biti toplamda 20 bit oldugunu belirtmistik.
 *	    ------------
 *	    -> A       = isletim sistemi icin kullanilabilir/kullanilmayabilirde.
 *          ------------
 *	    -> O       = rezerve edilmistir. Daima sifirdir. A biti ile birlestirilip kullanilabilir.
 *	    ------------
 *	    -> D       = segment 16 bitlik mi ? 32 bitlik mi ? (0 = 16bit, 1 = 32bit)
 *	    ------------
 *	    -> G       = bu bit limit ile gosterilen sayinin bayt mi yoksa sayfa mi oldugunu belirtir.
 *	    ------------ limit sahasi toplam 20 bittir ve eger bayt olarak kullanilirsa en fazla 1 MiB (2^20)
 *		         olabilir. Sayfa olursa 2^20*4096 = 4 GiB gibi bir limit tanimlanabilir.
 *
 * base_high: segment taban adresinin son 8 biti. toplamda 32 bit.
 * ----------
 *
 * !Not: detayli inceleme icin "gdt_entry" yapisini inceleyebilirsiniz.
 * -----
 *
 * ----------------------------------------------------------------------------------------------------------
 *  -"TYPE" 4 bitin detayli aciklamasi-
 * 
 * * Eger tanimlayici bir hafiza segmenti tanimliyorsa(kod,veri,yigin) *
 * 
 * 1.bit : 0 ise segment veri yada yigindir. 1 ise kod segmentidir.
 * -------
 * 2.bit : eger segment kod segment degilse. veri mi? yigin mi? bu bit
 * ------ bit sayesinde anlasilir.1 ise yigin 0 ise veri segmentidir.
 *
 *	  eger segment kod segmenti ise bu bit baska anlam tasir.
 *        bu bit 0 ise ayricalik duzeyi(DPL) kontrol edilmez. eger 1 ise
 *	  ayricalik duzeyi(DPL) kontrolu yapilir.
 *  3.bit : bu bit segmentin kod segmenti olup olmamasina bagli olarak farkli
 * ------- anlam kazanir.
 *	  
 *	  eger segment veri yada yigin segmenti ise bu bit yazma kontrol bitidir.
 *	  eger bu bit 1 ise o segment hem yazilir hem okunur,0 ise sadece okunur.
 *
 *	  eger segment kod segmenti ise bu durumda okuma kontrol biti olur. eger
 *        bu bit 0 ise o kod segment calistirilabilir,ancak 1 ise veri segmenti 
 *	  gibi okunabilir de.
 * 4.bit : bu bit ortak ve islemci o segmente her eristiginde 1 yapilir. istatistik
 * ------ amaciyla kullanilir.
 *
 * * Eger tanimlayici bir sistem segmenti tanimliyorsa *
 *
 *  bunlar siradan bellek segmentleri degil ozel amaclarla dusunulmus yapilardir."TYPE"
 *  4 bitlerine gore farkli yapilar olabilirler.
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
 *  !Not: LDTR'n LDT'yi dogrudan gostermedigini biliyoruz, onun GDT tablosunda
 *  ----- bir index'i gosterdigini soyledik. Eger tanimlayicinin "DT" biti 0 ise
 *        ve "TYPE" biti LDT ise o tanimlayici LDT'yi gosterir.
 *
 * -------------------------------------------------------------------------------------------------------------
 *
 *  - segment yazmaclari -
 *
 *  segment yazmaclari 16 bittir.bu 16 bitin ilk iki bitip RPL degerini tasir.
 *  3.bit ise o selektorun neyi gosterdigini belirler.bit 0 ise index GDT tablosunun
 *  1 ise LDT tablosunundur. geriye 13 bit kaldi, (2^13 = 8192) ile yazilabilecek
 *  en buyuk sayi budur. bu yuzden GDT ve LDT tablolari en fazla 8192 eleman 
 *  alabilir.
 *
 *  15                                 3   2     0
 *  ==============================================
 *  =               index             = TI = RPL =
 *  ==============================================
 *  (TI = tanimlayici tablosu)
 *
 * --------------------------------------------------------------------------------------------------------------
 *
 * - sayfa dizini ve sayfa tablosu girdileri-
 *
 * sayfa tablosu ve sayfa dizini girdileri 32 bittir. sayfa tablosu ve sayfa dizinin 1024 girdi
 * alabildiginden bahsetmistik. bu girdiler 32 bit yani 4 bayttir. sayfa tablosu ve sayfa dizinlerinin
 * toplamda 4 KiB boyutundadir. Sayfa tablolarinda her girdinin 4 KiB boyutundaki sayfalari gosterdiginden
 * bahsettik ve bu tablolar 1024 girdiye sahip olduguna gore 4*1024*1024 = 4 MiB yani bir sayfa tablosuyla
 * 4 MiB bellek adreslenebilir. Sayfa dizinleride 1024 girdiye sahipti ve her girdiside bir sayfa tablosuna
 * isaret ediyordu.1024*4 MiB = 4 GiB toplamda bellegin adreslenebileceginide boylece tekrar aciklamis
 * olduk.
 *
 *                                 sayfa dizin girdisi - 32bit
 *  =========================================================================================
 *  =     sayfa tablosu adresi     =   sistem  = G = PS = 0 = A = PCD = PWT = U/S = R/W = P =
 *  ========================================================================================= 
 *              20 bit                  3bit     1   1    1   1    1     1     1     1    1
 *
 *           -> P biti  = eger bit 1 ise sayfa tablosu bellektedir,0 ise degildir.	
 *	    ------------
 *	    -> R/W     = sayfa tablosunun okunup yazilabilirligi belirtir.
 *          ------------
 *	    -> U/S     = sayfa tablosunun uygulama programcilari icin erisimini belirler.
 *	    ------------ asagidaki tablodan izinleri kontrol edebilirsiniz.
 *		
 *			=======================================================================
 *		        =   U/S   =   R/W   =  uygulama programcisi  =  sistem programcisi    =
 *			=======================================================================
 *			=    0    =    0    =        erisemez        =  okuyabilir/yazabilir  =
 *			=    0    =    1    =        erisemez        =  okuyabilir/yazabilir  =
 *			=    1    =    0    =   yalniz okuyabilir    =  okuyabilir/yazabilir  =
 *			=    1    =    1    =  okuyabilir/yazabilir  =  okuyabilir/yazabilir  =
 *			=======================================================================
 *	    
 *	    -> PCD     = islemcinin L1,L2 cahcelerinin sayfa tablolari icin kullanip kullanilmayacagini
 *	    ------------ belirtir. PCD 0 ise islemci cacheleri kullanilir.
 *	    -> PWT     = cacha mekanizmasi ile ilgilidir. PWT 0 isee icsel tampodan okuma yazma yapilabilir.
 *	    ------------
 *	    -> A       = her erisimde set edilir. istatistik icin kullanililabilir.
 *           ------------
 *	    -> D       = her yazmada set edilir. istatistik icin kullanilabilir.
 *	    ------------
 *	    -> PS      = sayfa boyutunu belirler. eger bu bit 0 ise 4 KiB,1 ise 4 MiB sayfa
 *           ------------ boyutu kullanir.
 *	    -> G biti  = sayfanin global olmasini saglar. sayfa dizin tablosu icin anlamsizdir.
 *          ------------
 *          -> sistem  = isletim sistemi icin ayrilmistir.
 *          ------------
 *
 *	    			   sayfa tablosu girdisi - 32bit
 *  =========================================================================================
 *  =     sayfa   adresi           =   sistem  = G = 0  = D = A = PCD = PWT = U/S = R/W = P =
 *  ========================================================================================= 
 *              20 bit                  3bit     1   1    1   1    1     1     1     1    1
 *
 *           -> P biti  = eger bit 1 ise sayfa bellektedir,0 ise degildir.	
 *	    ------------
 *	    -> R/W     = sayfanin okunup yazilabilirligi belirtir.
 *           ------------
 *	    -> U/S     = sayfaya uygulama programcilari icin erisimini belirler.
 *	    ------------ asagidaki tablodan izinleri kontrol edebilirsiniz.
 *		
 *			=======================================================================
 *		        =   U/S   =   R/W   =  uygulama programcisi  =  sistem programcisi    =
 *			=======================================================================
 *			=    0    =    0    =        erisemez        =  okuyabilir/yazabilir  =
 *			=    0    =    1    =        erisemez        =  okuyabilir/yazabilir  =
 *			=    1    =    0    =   yalniz okuyabilir    =  okuyabilir/yazabilir  =
 *			=    1    =    1    =  okuyabilir/yazabilir  =  okuyabilir/yazabilir  =
 *			=======================================================================
 *	    
 *	    -> PCD     = islemcinin L1,L2 cahcelerinin sayfa tablolari icin kullanip kullanilmayacagini
 *	    ------------ belirtir. PCD 0 ise islemci cacheleri kullanilir.
 *	    -> PWT     = cacha mekanizmasi ile ilgilidir. PWT 0 isee icsel tampondan okuma yazma yapilabilir.
 *	    ------------
 *	    -> A       = her erisimde set edilir. istatistik icin kullanililabilir.
 *           ------------
 *	    -> D       = her yazmada set edilir. istatistik icin kullanilabilir.
 *	    ------------
 *	    -> PS      = sayfa boyutunu belirler. eger bu bit 0 ise 4 KiB,1 ise 4 MiB sayfa
 *          ------------ boyutu kullanir.
 *	    -> G biti  = sayfanin global olmasini saglar. sayfa dizin tablosu icin anlamsizdir.
 *	    ------------
 *          -> sistem  = isletim sistemi icin ayrilmistir.
 *          ------------
 *
 *   !Not : sayfa tablosu ve sayfa adresinin 20 bit olmasinin sebebi 4 KiB katlari seklinde olmasidir.
 *   ------ kacinci 4 KiB'likta oldugunu gosterir.
 *
 * ----------------------------------------------------------------------------------------------------------------------
 *
 */
 

/*
 * gdt tablosu icin girdi yapisi yani diger adiyla soylersek
 * tanimlayici.
 */
struct gdt_entry_t{
	uint16_t limit_low;	/* limitin ilk 16 biti */
	uint16_t base_low;	/* taban adresin ilk 16 biti */
	uint8_t base_middle;	/* taban adresin diger 8 biti */
	uint8_t access;		/* erisim flaglari */
	uint8_t granularity;	/* diger flaglar */
	uint8_t base_high;	/* taban adresin son 8 biti */
} __attribute__((packed));


/*
 * gdt_ptr_t bahsettigimiz selektor ve adres yapisidir.
 */
struct gdt_ptr_t{
	uint16_t limit;		/* selektor limit */
	uint32_t base;		/* ilk gdt_entry_t adresi */
} __attribute__((packed));


/*
 * ------------------------------------------------------------------------------------------
 *
 *  __attribute__((packed)) nedir?
 * bunu ornekle aciklamak daha iyi olucak gibime geliyor. ornegin elimizde asagidaki
 * gibi struct var.
 *
 * struct deneme{
 *	int a;
 *	char b;
 *	char c;
 *  );
 *
 * struct boyutunu hesaplayalim simdi.
 * sizeof(a) + sizeof(b) + sizeof(c) =  4 + 1 + 1 = 6 bayt olmasi lazim. fakat
 * struct'in boyutunu hesapladigimizda 8 bayt cikacaktir. cunku 4 bayt bloklar
 * halinde sinirlandirilmaktadir. yani 4 bayt katlari seklinde olur. bos kalan
 * 2 baytida kendi tamamlayarak 4 baytin kati olmasi saglanir.
 *
 * struct deneme{
 *	int a;
 *	char b;
 *	char c;
 *  ) __attribute__((packed));
 *
 * bu struct'in ise boyutunu hesapladigimizda daha once boyle cikmasi lazimdi 
 * dedigimiz cikar. ve 4 baytla sinirlandirilmazlar. yani 4 baytin kati
 * olmaz. bi nevi bu direktifle struct'i oldugu gibi kabul et diyoruz.
 * anlatabildigim kadariyla ;).
 *
 * ------------------------------------------------------------------------------------------
 */


/*
 * ayricalik seviyeleri
 * ---------------------
 * DPL0 = kernel modu
 * DPL1,DPL2 genellikle kullanilmaz.
 * DPL3 = kullanici modu
 */
#define SEGMENT_DPL0		0x00
#define SEGMENT_DPL1		0x20
#define SEGMENT_DPL2		0x40
#define SEGMENT_DPL3		0x60

/*
 * diger
 */
#define SEGMENT_PRESENT 	0x80	/* segment bellekte mi ? */
#define SEGMENT_NORMAL		0x10	/* segment normal bir hafiza segmenti mi? */

/*
 * gdt_set_gate
 *
 * @param num :
 * @param base :
 * @param limit :
 * @param access :
 * @param gran :
 */
void gdt_set_gate(size_t num,uint32_t base,uint32_t limit,uint8_t access,uint8_t gran){
	
} 

/*
 * init_gdt
 */
void init_gdt(void){
	
	
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
