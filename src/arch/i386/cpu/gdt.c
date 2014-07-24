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
 * ----	intel tarafinda rezerve edilmistir.
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
 * -----------------------------------------------------------------------------------------------------------------
 *  -"TYPE" 4 bitin detayli aciklamasi-
 * 
 * * Eger tanimlayici bir hafiza segmenti tanimliyorsa(kod,veri,yigin) *
 * 
 * 1.bit : 0 ise segment veri yada yigindir. 1 ise kod segmentidir.
 * 2.bit : eger segment kod segment degilse. veri mi? yigin mi? bu bit
 *	  bit sayesinde anlasilir.1 ise yigin 0 ise veri segmentidir.
 *
 *	  eger segment kod segmenti ise bu bit baska anlam tasir.
 *        bu bit 0 ise ayricalik duzeyi(DPL) kontrol edilmez. eger 1 ise
 *	  ayricalik duzeyi(DPL) kontrolu yapilir.
 *  3.bit : bu bit segmentin kod segmenti olup olmamasina bagli olarak farkli
 *	  anlam kazanir.
 *	  
 *	  eger segment veri yada yigin segmenti ise bu bit yazma kontrol bitidir.
 *	  eger bu bit 1 ise o segment hem yazilir hem okunur,0 ise sadece okunur.
 *
 *	  eger segment kod segmenti ise bu durumda okuma kontrol biti olur. eger
 *        bu bit 0 ise o kod segment calistirilabilir,ancak 1 ise veri segmenti 
 *	  gibi okunabilir de.
 * 4.bit : bu bit ortak ve islemci o segmente her eristiginde 1 yapilir. istatistik
 *	  amaciyla kullanilir.
 *
 * * Eger tanimlayici bir sistem segmenti tanimliyorsa *
 *
 *  bunlar siradan bellek segmentleri degil ozel amaclarla dusunulmus yapilardir."TYPE"
 *  4 bitlerine gore farkli yapilar olabilirler.
 *
 *  =========================================================================
 *  ==	               Sistem segment tanimlayicisi turleri               ==
 *  =========================================================================
 *  = 0 = 0 = 0 = 0 = 		   rezerve edilmis                         =
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
 *  = 1 = 0 = 0 = 0 = 		   rezerve edilmis                         =
 *  =========================================================================
 *  = 1 = 0 = 0 = 1 =              32 bit tss (hazir)                       =
 *  =========================================================================
 *  = 1 = 0 = 1 = 0 = 		   rezerve edilmis                         =
 *  =========================================================================
 *  = 1 = 0 = 1 = 1 =             32 bit tss (mesgul)                       =
 *  =========================================================================
 *  = 1 = 1 = 0 = 0 =              32 bit cagri kapisi                      =
 *  =========================================================================
 *  = 1 = 1 = 0 = 1 = 		   rezerve edilmis                         =
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
 */
 
struct gdt_entry{
	u16 limit_low;
	u16 base_low;
	u8 base_middle;
	u8 access;
	u8 granularity;
	u8 base_high;
} __attribute__((packed));


struct gdt_ptr {
	u16 limit;
	u32 base;
} __attribute__((packed));
