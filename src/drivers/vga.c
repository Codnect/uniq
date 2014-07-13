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
 

#include <drivers/vga.h>
#include <uniq/port.h>
#include <uniq/types.h>
#include <uniq/module.h>
#include <string.h>

/*
 * grub onyukleyicisi kerneli 80x25 text modunda baslatir.
 * bu text modundaki karakter formati
 *
 * 15       12 11          8 7                       0
 * ====================================================
 * = Arkaplan =   Karakter  =      Karakter Kodu      =
 * =   Rengi  =     Rengi   =	      (ASCII)         =
 * ====================================================
 */



#define VGA_VRAM_START		0xB8000				/* vga video ram baslangici */
#define VGA_VRAM_END		0xC0000				/* vga video ram sonu */
#define VGA_CWIDTH		80				/* vga sutun sayisi */
#define VGA_CHEIGHT		25				/* vga satir sayisi */
#define VGA_CSIZE		(VGA_CWIDTH * VGA_CHEIGHT)	/* vga buyuklugu */
#define VGA_IPORT		0x3D4				/* vga crtc indis portu */
#define VGA_DPORT		0x3D5				/* vga crtc veri portu */
#define VGA_CLHIGH		14				/* vga imlec high */
#define VGA_CLLOW		15				/* vga imlec low */

#define TAB_SIZE		8				/* tab buyuklugu */
#define BLANK			0x20				/* bosluk karakteri */



static uint16_t * vga_vram = NULL;	/* vga video ram */
static uint8_t csr_x,csr_y;		/* imlec x ve y */
static uint16_t csr_loc;		/* imlec toplam konum (y*80 + x)*/
static uint8_t save_x,save_y;


/*
 * refresh_csr, imlec degerlerlerini yeniler, imleci
 * konumuna(csr_loc) gore csr_x ve csr_y degerlerini ayarlar.
 */
static void refresh_csr(void){
	
	csr_x = csr_loc % VGA_CWIDTH;
	csr_y = csr_loc / VGA_CWIDTH;
	
}

/*
 * restore_csr, imlecin kaydedilmis degerlerini (save_x ve save_y)
 * geri csr_x ve csr_y degerlerine atar ve eski konumuna imlecine
 * goturur.
 */
static void restore_csr(void){
	
	csr_x = save_x;
	csr_y = save_y;
	goto_xy(csr_x,csr_y);
	
}

/*
 * save_csr ,imlecin bulundugu son csr_x ve csr_y degerlerini  alir
 * ve kaydeder.
 */
static void save_csr(void){

	refresh_csr();
	save_x = csr_x;
	save_y = csr_y;

}

/*
 * scrollup, ekranin son satiri yazma islemi olursa tum satirlari bir ust
 * satira tasir.
 */
static void scrollup(void){

	if(csr_loc < VGA_CSIZE)
		return;
	
	/*
	 *for(int i=0;i<=VGA_CSIZE;i++)
	 *	vga_vram[i] = vga_vram[i+VGA_CWIDTH];
	 */
	 
	/*
	 * string.c'ye memcpy'yi ekledikten sonra onu kullanmak
	 * daha iyi gibi ;)
	 */
	for(int i=0;i<=VGA_CHEIGHT;i++)
		memcpy(vga_vram+i*VGA_CWIDTH,vga_vram+(i+1)*VGA_CWIDTH,(VGA_CWIDTH)*2);

	delete_line(VGA_CHEIGHT-1);
	csr_loc -= VGA_CWIDTH;
	
}

/*
 * delete_line, satir numarasi verilen satirdaki karakterleri temizler
 *
 * @param line_no : silinecek satir numarasi
 */
void delete_line(uint8_t line_no){
	
	if(!vga_vram)
		return;

	if(line_no > (VGA_CHEIGHT-1))
		return;

	uint8_t new_x = 0;
	uint8_t new_y = line_no;
	goto_xy(new_x,new_y);

	for(uint8_t i=new_x;i<VGA_CWIDTH;i++)
		vga_vram[csr_loc+i] = make_vga_entry(BLANK,DEFAULT_ATTR);

	restore_csr();
 	
}

/*
 * change_line,satirdaki karakter dizisini degistirir
 *
 * @param line_no : satir numarasi
 * @param s : karakter dizisi
 */
void change_line(uint8_t line_no,const char *s){
	
	if(!vga_vram || !s)
		return;

	if(line_no > (VGA_CHEIGHT-1))
		return;
		
	delete_line(line_no);	
	uint8_t new_x = 0;
	uint8_t new_y = line_no;
	goto_xy(new_x,new_y);
	putstr(s,DEFAULT_ATTR);
	restore_csr();
		
}
 
/*
 * move_csr, imleci degisen csr_loc'a gore ayarlar.kisacasi imlec
 * konumunu ayarlar.
 */
static void move_csr(void){

	outbyte(VGA_IPORT,VGA_CLHIGH);
	outbyte(VGA_DPORT,csr_loc >> 8);

	outbyte(VGA_IPORT,VGA_CLLOW);
	outbyte(VGA_DPORT,(csr_loc & 0xFF));
	
	refresh_csr();
	
}

/*
 * goto_xy ,verilen x ve y koordinatlarina gore imleci tasir.
 *
 * @param new_x : x koordinati
 * @param new_y : y koordinati
 */
void goto_xy(uint8_t new_x,uint8_t new_y){
	
	if(!vga_vram)
		return;

	if(new_x > (VGA_CWIDTH-1) || (VGA_CHEIGHT-1) < new_y)
		return;

	save_csr();
	csr_loc = new_y * VGA_CWIDTH + new_x;
	move_csr();

}

/*
 * goto_save_csr, kaydedilmis koordinatlara geri doner.
 * goto_xy fonksiyonuyla baska koordinatlara gidilmeden once
 * o anki koordinatlar kaydedilir ve yeni koordinatlar gidilir,
 * bir degisiklik yapilicaksa yapilir(karakter dizisi yazilmasi
 * gibi) ondan sonra eski koordinatlara donulmek istenirse bu
 * fonksiyon yardimiyla yapilir,restore_csr ile farkina gelecek
 * olursak restore_csr sadece bu kod sayfasi icinden erisilebilecek
 * duzeydedir diye bir aciklama yapmisim :) zaten goruldugu gibi 
 * restore_csr fonksiyonuna erisiliyor.
 */
void goto_save_csr(void){
	
	if(!vga_vram)
		return;
	
	if(save_x && save_y)
		restore_csr();
	
}

/*
 * putchar, verilen karakteri ekrana basar.
 *
 * @param c: karakter
 * @param attr: karakter ozelligi(rengi)
 */
void putchar(const char c, uint8_t attr){

	if(!vga_vram || !c)
		return;
		
	if(!attr)
		attr = DEFAULT_ATTR;

	switch(c){
		/* bir alt satir */
		case '\n':
			goto_xy(0,csr_y+1);
			break;
		/* imleci sola kaydir(backspace) */
		case '\b':
			vga_vram[csr_loc--] = make_vga_entry(BLANK,DEFAULT_ATTR);
			break;
		/* satir basi */
		case '\r':
			goto_xy(0,csr_y);
			break;
		/* yatay tab */
		case '\t':
			for(int i=0;i<TAB_SIZE;i++)
				vga_vram[csr_loc++] = make_vga_entry(BLANK,DEFAULT_ATTR);
			break;
		/* dikey tab */
		case '\v':
			putstr("\n\t",DEFAULT_ATTR);
			break;
		/* cift tirnak */
		case '\"':
			vga_vram[csr_loc++] = make_vga_entry(0x22,attr);
			break;
		/* tek tirnak */
		case '\'':
			vga_vram[csr_loc++] = make_vga_entry(0x27,attr);
			break;
		case '\\':
			vga_vram[csr_loc++] = make_vga_entry(0x5C,attr);
			break;
		default:
			vga_vram[csr_loc++] = make_vga_entry(c,attr);
			break;
			
	}

	scrollup();
	move_csr();
	
}

/*
 * putstr, verilen karakter dizisini ekrana basar.
 *
 * @param string : karakter dizisi
 * @param attr : karakter ozelligi
 */
void putstr(const char *string,uint8_t attr){
	
	if(!vga_vram || !string)
		return;
	
	if(!attr)
		attr = DEFAULT_ATTR;
		
	int i = 0;
	while(string[i])
		putchar(string[i++],attr);

}

/*
 * reset_console, ekrani temizler.
 */
void reset_console(void){
	
	if(!vga_vram)
		return;
	
	csr_x = csr_y = csr_loc = 0;

	for(uint8_t i=0;i<VGA_CHEIGHT;i++)
		delete_line(i);
	move_csr();
	
}

/*
 * exit_vga_console, vga konsolunu kapatir.kapattiktan 
 * sonra tum ekrani temizleyecek ve ekrana bundan sonra
 * karakter yada karakter dizisi yazdiramiyacaginizi
 * unutmayin! ekrana karakter yazdirabilmek icin 
 * vga_init_console fonksiyonuyla vga konsolunu tekrar 
 * baslatmaniz gereklidir.
 */
void exit_vga_console(void){
	
	if(!vga_vram)
		return;
		
	/*
	 * eger debug modu tanimliysa ekrani temizleme!
	 */
	#ifndef DEBUG_MODE
		reset_console();
	#endif
	
	vga_vram = NULL;
	
}


/*
 * init_vga_console, vga konsolunu baslatir.eger bu fonksiyon
 * vga konsolu baslatilmazsa ekrana karakter yada karakter 
 * dizisi yazdirmaniz mumkun olmaz.onun icin init_vga_console'u
 * baslatmayi unutmayin!
 */
void init_vga_console(void){

	if(vga_vram)
		return;

	vga_vram = (uint16_t*)(VGA_VRAM_START);
	reset_console();
	
}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
