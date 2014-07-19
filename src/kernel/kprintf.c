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
 *  --------------------------------------------------------------------------
 *
 *  printf'in bu versiyonu 64-bit'i desteklememektedir :/.
 *
 */
 
#include <uniq/types.h>
#include <ctype.h>
#include <va_list.h>
#include <uniq/kernel.h>
#include <string.h>
#include <uniq/module.h>

#define MAX_HEXDIGIT	8	/* 'FFFFFFFF' */

/* flags */
#define SIGN		0x001	/* isaretli/isaretsiz tam sayi */
#define PLUS		0x002	/* saga dayali */
#define SMALL		0x004	/* '0xabdef' '0xABCDEF' */
#define LEFT		0x008	/* sola dayali */
#define CHAR		0x010	/* karakter */
#define STRING		0x020	/* karakter dizisi */
#define INTEGER		0x040	/* tam sayilar */
#define HEX		0x080	/* hexadecimal sayilar */
#define COLOR		0x100	/* renk */
#define SPECIAL		0x200	/* ozel karakter icin */
#define DOT		0x400	/* nokta */
#define POINTER		0x800	/* pointer */

/*
 * vga renklerinin ansi renklere cevrilmesi icin
 */
static const char vga_to_ansi[] = {
	0, 4, 2, 6, 1, 5, 3, 7,
	8,12,10,14, 9,13,11,15
};

/*
 * skip_atoi, kendisine verilen karater dizisinde digit
 * olanlari sayi haline getirir.
 * !dikkat : ulib icindeki atoi'den farkli olarak aldigi
 * karakter dizisinin adresini ilerletir.
 */
static int skip_atoi(const char **s){
	int i = 0;
	
	while (isdigit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

/*
 * make_ansi_color, karakter dizisi seklinde verilmis karakter
 * formatini okur ve ansi renklere cevirir.
 *
 * @param s : karakter dizisi
 * !dikkat : yukaridaki skip_atoi gibi gonderilen karakter
 * dizisinin adresi ilerler.
 */
static uint8_t make_ansi_color(const char **s){

	/*
	* normalde terminal ansi color boyle degil fakat
	* kendimce basitlik acisinda bu kadari yeterli gibi.
	* strtok kullanilmamasinin nedeni fmt'nin bozulmasiyla
	* sonuclanacagindan ve az bucuk yine ayni kod satiri
	* sayisi kadar olacagi icin gerek duymadim.
	*
	*
 	* \033[aciklik;yazi_rengi;yazi_arkaplan_rengi ve sonuna
	* 'm' eki
	*
	* aciklik = 0-1
	* yazi rengi = 30-37
	* arkaplan rengi = 40-47 degerlerini alir
	*	
	* yazi rengi :
	* -------------
    	* 30 Black - 31 Red - 32 Green - 33 Yellow - 34 Blue
 	* 35 Magenta - 36 Cyan - 37 White
	* yazi arkaplan rengi:
	* --------------
	* 40 Black - 41 Red - 42 Green - 43 Yellow - 44 Blue
	* 45 Magenta - 46 Cyan - 47 White
	*/
	uint8_t attr;
	if(**s == '\033'){
			uint8_t fg,bg,t;
			fg = bg = t = 0;
			(*s)++;
			if(**s == '['){
				(*s)++;
				if(isdigit(**s))
					t = skip_atoi(s);
				if(**s == ';'){
					(*s)++;
					if(isdigit(**s))
						fg = skip_atoi(s);
				}

				if(**s == ';'){
					(*s)++;
					if(isdigit(**s))
						bg = skip_atoi(s);
				} 
				
				if(**s == 'm'){
					if(!t)						/* \033[0m */
						attr = make_vga_color(vga_to_ansi[t],vga_to_ansi[bg]);
					else if(t >= 0x1e && t<= 0x28)			/* \033[(30-37)m */
						attr = make_vga_color(vga_to_ansi[t % 0x1e],vga_to_ansi[bg]);
					else if(t >= 0x28 && t<= 0x37)			/* \033[(40-54)m */
						attr = make_vga_color(vga_to_ansi[fg],vga_to_ansi[t % 0x28]);
					else
						attr = make_vga_color(vga_to_ansi[(t * 0x8) + (fg % 0x1e)],vga_to_ansi[bg % 0x28]);
				}
				
			}

	}
	

	return attr;
}

/*
 * hex_number, decimal sayilari hexadecimal formatina cevirip karakter 
 * dizisi haline getirir.
 * 
 * pointer ve normal hexadecimal sayilari yazarken farklilik soyle,
 * ornegin sayi 32 decimal sayisi olsun
 * hexadecimalde : 20
 * pointer olarak ise : 0x00000020 olarak ekrana basilir
 *
 * @param buf : karakter dizisinin yazilacagi alan
 * @param flags : flaglar
 * @param nptr : sayinin adresi
 */
static int hex_number(char *buf,uint32_t flags,uint32_t* nptr){
	
	
	int hdigit_count = 0;
	char hex_digits[] = "0123456789ABCDEFabcdef";
	char *tmptr;
	char tmp_buf[8];
	char c;
	uint32_t number = *nptr;

	if(flags & POINTER){
		strcpy(buf,"0x");
		buf += 2;
	}
	
	tmptr = tmp_buf;
	
	if((flags & HEX) && (number == 0)){
		*buf++ = '0';
		*buf = '\0';
		return ++hdigit_count;
	}

	while(number != 0){
			uint8_t tmp = number % 16;
			number/=16;
			hdigit_count++;
			*tmptr++ = ((flags & SMALL) && !(tmp >= 0 && tmp <=9)) ? hex_digits[tmp + 6] : hex_digits[tmp];
	}
	
	
	*tmptr = '\0';
	int8_t x = MAX_HEXDIGIT - hdigit_count;
	if(flags & POINTER){
		while(x--)
			*buf++ = '0';
	}

	if(hdigit_count){
		tmptr--;
		while(hdigit_count--)
			*buf++ = *tmptr--; 

	}
	
	*buf = '\0';
	
	return (flags & HEX) ? hdigit_count : MAX_HEXDIGIT+2;
}

/*
 * dec_number, decimal sayilari karakter dizisi formatina gore
 * cevirir.
 * 
 * @param buf : karakter dizisinin yazilacagi alan
 * @param flags : flaglar
 * @param nptr : sayinin adresi
 */
static int dec_number(char *buf,uint32_t flags,uint32_t* nptr){

	uint32_t decimal_n,count = 0;
	bool neg = false;
	if(flags & SIGN){
		if((int32_t)*nptr < 0){
			decimal_n = -(*nptr);
			*buf = '-';
			neg = true;
			buf++;
		}
		else
			decimal_n = *nptr;

	}
	else
		decimal_n = *nptr;

	if(decimal_n == 0){
		*buf++ = '0';
		*buf ='\0';
		return ++count;
	}
	
	while(decimal_n > 0){
		uint32_t temp = decimal_n % 10;
		decimal_n /= 10;
		*(buf++) = temp + '0';
		count++;
			
	}
	*buf = '\0';

	if(count){
		char *start = (char*)buf-count;
		buf--;
		/* sayiyi ters cevir */
		for(int i=0;i<count/2;i++){
			char temp = *(start+i);
			*(start+i) = *(buf-i);
			*(buf-i) = temp;
		}
	}

	/* negatifse sayi +1 olarak '-' karakteri ekle */
	return (neg) ? count + 1 : count;
}

/*
 * vasprintf, kendisine verilen format ve arguman listesine
 * gore ekrana karakterleri basar.
 *
 * @param fmt : format
 * @param arg_list : arguman listesi
 */
static int vasprintf(const char *fmt, va_list arg_list){
	
	uint8_t attr = DEFAULT_ATTR;
	uint32_t flags,u,printed;
	uint32_t len = strlen(fmt),slen,buf_len;
	int32_t d;
	char *s;
	char buf[16];
	unsigned char c;
	bool cntrl = false;
	printed = 0;

	for( ;*fmt;++fmt){
		if(*fmt != '%' && *fmt != '\033'){
			putchar(*fmt,attr);
			printed++;
			continue;
		}

		if(*fmt == '\033'){
			attr = make_ansi_color(&fmt);
			continue;
				
		}
		fmt++;
			
		flags = 0;
		if(*fmt == '-')
			flags |= LEFT;
		else
			flags |= PLUS;

		if(flags & LEFT)
			fmt++;
		uint32_t arg_width,field_width;
		arg_width = field_width = 0;
		if(isdigit(*fmt))
			field_width = skip_atoi(&fmt);			

		if(*fmt == '.'){
			flags |= DOT;
			fmt++;
			arg_width = skip_atoi(&fmt);
		}

		switch(*fmt){
			case 'c':	/* karakter */
				c = (unsigned char)va_arg(arg_list,int);
				flags |= CHAR;
				break;
			case 's':	/* karakter dizisi */
				s = (char*)va_arg(arg_list,char*);
				if(!s)
					s = "(null)";
				flags |= STRING;
				slen = strlen(s);
				break;
			case 'd':	/* signed */
			case 'i':
				d = (int32_t)va_arg(arg_list,int);
				flags |= INTEGER;
				flags |= SIGN;													
				break;
			case 'u':	/* unsigned */
			case 'x':	/* hexadecimal */
			case 'X':
			case 'p':	/* pointer */
			case 'P':
				u = (uint32_t)va_arg(arg_list,unsigned int);
				if(*fmt == 'x'){
					flags |= HEX;
					flags |= SMALL;
				}
				else if(*fmt == 'X')
					flags |= HEX;
				else if(*fmt == 'u')
					flags |= INTEGER;
				else if(*fmt == 'p'){
					flags |= POINTER;
					flags |= SMALL;
				}
				else
					flags |= POINTER;
				break;
			case 'C':	/* renk */
				attr = (uint8_t)va_arg(arg_list,unsigned int);
				flags |= COLOR;
				break;
			case '%':
				putchar('%',attr);
				flags |= SPECIAL;
				break;
			default:
				putchar(*fmt,attr);
				break;

		}
		cntrl = false;

		/*
		 * char buf[32] = "\033[1;32mDeneme\033[0m";
		 * printf("%-10s",buf);
		 *
		 * yukaridaki ornekteki gibi renkli yazdirilirken hizalama
		 * konusunda bir bug olusuyordu. bunu onlemek icin gelen
		 * karakter dizileri icindeki renk kodlarinin karakter
		 * adedi cikartiliyor.
		 */
		if(flags & STRING){
			uint32_t length = 0;
			for(int i=0;i<slen;i++){
				if(s[i] == '\033'){
					length++;
					while(s[i] != 'm' && s[i] != '\0'){
						length++;
						i++;
					}
				}
			}
			slen -= length;
		}
				
		if((flags & COLOR) || (flags & SPECIAL))		
			continue;
		begin:

			if(flags & CHAR){
				if(!cntrl){
					if(field_width)
						field_width--;
				}
				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					putchar(c,attr);
					printed++;
				}
				if((flags & LEFT) && !cntrl)
					goto blank;
				continue;
			}
			else if(flags & STRING){
				if(!cntrl){
					/*
					 * "%-10.3s"
 					 * %(+/-)field_width.arg_width(char)
					 */
					if(!arg_width){
						if(field_width > slen)
							field_width -= slen;
						else
							field_width = 0;
					}
					else if(field_width <= arg_width)
						field_width = 0;
					else{
						if(arg_width && field_width)
							field_width -=arg_width;
					} 
				}

				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					if(!arg_width)
						while(*s){
							if(*s == '\033'){
								attr = make_ansi_color((const char**)(&s));
								s++;
								continue;
							}
							putchar(*s++,attr);
							printed++;
						}
					else{
						while(arg_width-- && *s){
							if(*s == '\033'){
								attr = make_ansi_color((const char**)(&s));
								s++;
								continue;
							}
							putchar(*s++,attr);
							printed++;
						}
					}
				}
				if((flags & LEFT) && !cntrl)
					goto blank;
				continue;

			}
			else{
				
				if((flags & INTEGER) && !cntrl){
					if(flags & SIGN)
						buf_len = dec_number(buf,flags,&d);
					else
						buf_len = dec_number(buf,flags,&u);
				}
				else if( ((flags & HEX) || (flags & POINTER)) && !cntrl)
					buf_len = hex_number(buf,flags,&u);

				s = buf;
				
				if(!cntrl){
					/*
					 * "%-10.3s"
 					 * %(+/-)field_width.arg_width(char)
					 */
					if(!arg_width){
						if(field_width > buf_len)
							field_width -= buf_len;
						else
							field_width = 0;
					}
					else if(field_width <= arg_width)
						field_width = 0;
					else{
						if(arg_width && field_width)
							field_width -=arg_width;
					} 
				}
				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					if(!arg_width)
						printed += putstr(buf,attr);
					else{
						while(arg_width-- && *s){
							putchar(*s++,attr);
							printed++;
						}
					}
				}
				if((flags& LEFT) && !cntrl)
					goto blank;
				
				continue;
			}

			blank:
				while(field_width && field_width >0){
					putchar(' ',attr);
					printed++;
					field_width--;
				}
				cntrl = true;
				goto begin;
	}

	return printed;
}

/* 
 * vsnprintf,verilen karakter dizisine format'a gore argumanlari
 * belirtilen boyuta kadar yerlestirir.
 *
 * @param strbuf : karakter dizisi adresi
 * @param n : boyut
 * @param fmt : format
 * @param ... : argumanlar
 */
int vsnprintf(char *strbuf,size_t n,const char *fmt, va_list arg_list){
	
	uint32_t flags,u,printed;
	uint32_t len = strlen(fmt),slen,buf_len;
	int32_t d;
	char *s;
	char buf[16];
	unsigned char c;
	bool cntrl = false;
	uint32_t i = 0;

	for( ;n && *fmt;++fmt){
		if(*fmt != '%'){
			*strbuf++ = *fmt; 
			i++;
			n--;
			continue;
		}
		fmt++;
			
		flags = 0;
		if(*fmt == '-')
			flags |= LEFT;
		else
			flags |= PLUS;

		if(flags & LEFT)
			fmt++;
		uint32_t arg_width,field_width;
		arg_width = field_width = 0;
		if(isdigit(*fmt))
			field_width = skip_atoi(&fmt);			

		if(*fmt == '.'){
			flags |= DOT;
			fmt++;
			arg_width = skip_atoi(&fmt);
		}

		switch(*fmt){
			case 'c':	/* karakter */
				c = (unsigned char)va_arg(arg_list,int);
				flags |= CHAR;
				break;
			case 's':	/* karakter dizisi */
				s = (char*)va_arg(arg_list,char*);
				if(!s)
					s = "(null)";
				flags |= STRING;
				slen = strlen(s);
				break;
			case 'd':	/* signed */
			case 'i':
				d = (int32_t)va_arg(arg_list,int);
				flags |= INTEGER;
				flags |= SIGN;													
				break;
			case 'u':	/* unsigned */
			case 'x':	/* hexadecimal */
			case 'X':
			case 'p':	/* pointer */
			case 'P':
				u = (uint32_t)va_arg(arg_list,unsigned int);
				if(*fmt == 'x'){
					flags |= HEX;
					flags |= SMALL;
				}
				else if(*fmt == 'X')
					flags |= HEX;
				else if(*fmt == 'u')
					flags |= INTEGER;
				else if(*fmt == 'p'){
					flags |= POINTER;
					flags |= SMALL;
				}
				else
					flags |= POINTER;
				break;
			case '%':
				if(n)
					*strbuf++ = '%';
				i++;
				n--;
				flags |= SPECIAL;
				break;
			default:
				break;

		}
		cntrl = false;
		
		/* renk bug'i icin */
		if(flags & STRING){
			uint32_t length = 0;
			for(int i=0;i<slen;i++){
				if(s[i] == '\033'){
					length++;
					while(s[i] != 'm' && s[i] != '\0'){
						length++;
						i++;
					}
				}
			}
			slen -= length;
		}

		if(flags & SPECIAL)		
			continue;
		begin:

			if(flags & CHAR){
				if(!cntrl){
					if(field_width)
						field_width--;
				}
				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl &&  n) ){
					*strbuf++ = c;
					i++;
					n--;
				}
				if((flags & LEFT) && !cntrl)
					goto blank;
				continue;
			}
			else if(flags & STRING){
				if(!cntrl){
					/*
					 * "%-10.3s"
 					 * %(+/-)field_width.arg_width(char)
					 */
					if(!arg_width){
						if(field_width > slen)
							field_width -= slen;
						else
							field_width = 0;
					}
					else if(field_width <= arg_width)
						field_width = 0;
					else{
						if(arg_width && field_width)
							field_width -=arg_width;
					} 
				}

				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					if(!arg_width)
						while(*s && n){
							*strbuf++ = *s++;
							i++;
							n--;
						}
					else{
						while(arg_width-- && *s && n){
							*strbuf++ = *s++;
							i++;
							n--;
						}
					}
				}
				if((flags & LEFT) && !cntrl)
					goto blank;
				continue;

			}
			else{
				
				if((flags & INTEGER) && !cntrl){
					if(flags & SIGN)
						buf_len = dec_number(buf,flags,&d);
					else
						buf_len = dec_number(buf,flags,&u);
				}
				else if( ((flags & HEX) || (flags & POINTER)) && !cntrl)
					buf_len = hex_number(buf,flags,&u);

				s = buf;
				
				if(!cntrl){
					/*
					 * "%-10.3s"
 					 * %(+/-)field_width.arg_width(char)
					 */
					if(!arg_width){
						if(field_width > buf_len)
							field_width -= buf_len;
						else
							field_width = 0;
					}
					else if(field_width <= arg_width)
						field_width = 0;
					else{
						if(arg_width && field_width)
							field_width -=arg_width;
					} 
				}
				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					if(!arg_width)
						while(*s && n){
							*strbuf++ = *s++;
							i++;
							n--;
						}
					else{
						while(arg_width-- && *s && n){
							*strbuf++ = *s++;
							i++;
							n--;
						}
					}
				}
				if((flags& LEFT) && !cntrl)
					goto blank;
				
				continue;
			}

			blank:
				while(field_width && field_width >0 && n){
					*strbuf++ = ' ';
					i++;
					n--;
					field_width--;
				}
				cntrl = true;
				goto begin;
	}
	*strbuf = '\0';
	return i;
}

/* 
 * snprintf,verilen karakter dizisine format'a gore argumanlari
 * belirtilen boyuta kadar yerlestirecek olan vsnprintf fonksiyonu 
 * cagirir.
 *
 * @param buf : karakter dizisi adresi
 * @param n : boyut
 * @param fmt : format
 * @param ... : argumanlar
 */
int snprintf(char *buf,size_t n,const char *fmt, ...){
	
	va_list arg_list;
	int i;

	va_start(arg_list,fmt);
	i = vsnprintf(buf,n,fmt,arg_list);
	va_end(arg_list);
	return i;
}

/* 
 * vsprintf,verilen karakter dizisine format'a gore argumanlari
 * yerlestirir.
 *
 * @param strbuf : karakter dizisi adresi
 * @param fmt : format
 * @param ... : argumanlar
 */
int vsprintf(char *strbuf,const char *fmt, va_list arg_list){
	
	uint32_t flags,u,printed;
	uint32_t len = strlen(fmt),slen,buf_len;
	int32_t d;
	char *s;
	char buf[16];
	unsigned char c;
	bool cntrl = false;
	uint32_t i = 0;

	for( ;*fmt;++fmt){
		if(*fmt != '%'){
			*strbuf++ = *fmt; 
			i++;
			continue;
		}
		fmt++;
			
		flags = 0;
		if(*fmt == '-')
			flags |= LEFT;
		else
			flags |= PLUS;

		if(flags & LEFT)
			fmt++;
		uint32_t arg_width,field_width;
		arg_width = field_width = 0;
		if(isdigit(*fmt))
			field_width = skip_atoi(&fmt);			

		if(*fmt == '.'){
			flags |= DOT;
			fmt++;
			arg_width = skip_atoi(&fmt);
		}

		switch(*fmt){
			case 'c':	/* karakter */
				c = (unsigned char)va_arg(arg_list,int);
				flags |= CHAR;
				break;
			case 's':	/* karakter dizisi */
				s = (char*)va_arg(arg_list,char*);
				if(!s)
					s = "(null)";
				flags |= STRING;
				slen = strlen(s);
				break;
			case 'd':	/* signed */
			case 'i':
				d = (int32_t)va_arg(arg_list,int);
				flags |= INTEGER;
				flags |= SIGN;													
				break;
			case 'u':	/* unsigned */
			case 'x':	/* hexadecimal */
			case 'X':
			case 'p':	/* pointer */
			case 'P':
				u = (uint32_t)va_arg(arg_list,unsigned int);
				if(*fmt == 'x'){
					flags |= HEX;
					flags |= SMALL;
				}
				else if(*fmt == 'X')
					flags |= HEX;
				else if(*fmt == 'u')
					flags |= INTEGER;
				else if(*fmt == 'p'){
					flags |= POINTER;
					flags |= SMALL;
				}
				else
					flags |= POINTER;
				break;
			case '%':
				*strbuf++ = '%';
				i++;
				flags |= SPECIAL;
				break;
			default:
				break;

		}
		cntrl = false;
		
		/* renk bug'i icin */
		if(flags & STRING){
			uint32_t length = 0;
			for(int i=0;i<slen;i++){
				if(s[i] == '\033'){
					length++;
					while(s[i] != 'm' && s[i] != '\0'){
						length++;
						i++;
					}
				}
			}
			slen -= length;
		}

		if(flags & SPECIAL)		
			continue;
		begin:

			if(flags & CHAR){
				if(!cntrl){
					if(field_width)
						field_width--;
				}
				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					*strbuf++ = c;
					i++;
				}
				if((flags & LEFT) && !cntrl)
					goto blank;
				continue;
			}
			else if(flags & STRING){
				if(!cntrl){
					/*
					 * "%-10.3s"
 					 * %(+/-)field_width.arg_width(char)
					 */
					if(!arg_width){
						if(field_width > slen)
							field_width -= slen;
						else
							field_width = 0;
					}
					else if(field_width <= arg_width)
						field_width = 0;
					else{
						if(arg_width && field_width)
							field_width -=arg_width;
					} 
				}

				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					if(!arg_width)
						while(*s){
							*strbuf++ = *s++;
							i++;
						}
					else{
						while(arg_width-- && *s){
							*strbuf++ = *s++;
							i++;
						}
					}
				}
				if((flags & LEFT) && !cntrl)
					goto blank;
				continue;

			}
			else{
				
				if((flags & INTEGER) && !cntrl){
					if(flags & SIGN)
						buf_len = dec_number(buf,flags,&d);
					else
						buf_len = dec_number(buf,flags,&u);
				}
				else if( ((flags & HEX) || (flags & POINTER)) && !cntrl)
					buf_len = hex_number(buf,flags,&u);

				s = buf;
				
				if(!cntrl){
					/*
					 * "%-10.3s"
 					 * %(+/-)field_width.arg_width(char)
					 */
					if(!arg_width){
						if(field_width > buf_len)
							field_width -= buf_len;
						else
							field_width = 0;
					}
					else if(field_width <= arg_width)
						field_width = 0;
					else{
						if(arg_width && field_width)
							field_width -=arg_width;
					} 
				}
				if((flags & PLUS) && !cntrl)
					goto blank;
				if( ((flags & PLUS) && cntrl) || ((flags & LEFT) && !cntrl) ){
					if(!arg_width)
						while(*s){
							*strbuf++ = *s++;
							i++;
						}
					else{
						while(arg_width-- && *s){
							*strbuf++ = *s++;
							i++;
						}
					}
				}
				if((flags& LEFT) && !cntrl)
					goto blank;
				
				continue;
			}

			blank:
				while(field_width && field_width >0){
					*strbuf++ = ' ';
					i++;
					field_width--;
				}
				cntrl = true;
				goto begin;
	}
	*strbuf = '\0';
	return i;
}

/* 
 * sprintf,verilen karakter dizisine format'a gore argumanlari
 * yerlestirecek olan vsprintf fonksiyonu cagirir.
 *
 * @param buf : karakter dizisi adresi
 * @param fmt : format
 * @param ... : argumanlar
 */
int sprintf(char *buf, const char *fmt, ...){
	
	va_list arg_list;
	int i;
	
	/*
	 * format uzunlugu, buf uzunlugundan buyuk olmamali
	 * en azindan esit olmalidir. bug duzeltidi gibi :/
	 * gibi oldu diyelim. simdilik bu fonksiyonda bug
	 * bulunuyor. dikkat!
	 */
	
	va_start(arg_list,fmt);
	i = vsprintf(buf,fmt,arg_list);
	va_end(arg_list);
	return i;
}


/*
 * printf,formatli olarak ekrana cikti verir.
 *
 * @param fmt : format
 * @param ... : argumanlar
 */
int printf(const char *fmt, ...){

	va_list arg_list;
	va_start(arg_list, fmt);
	int printed = vasprintf(fmt,arg_list);
	va_end(arg_list);
	return printed;

}

MODULE_AUTHOR("Burak Köken");
MODULE_LICENSE("GNU GPL v2");
