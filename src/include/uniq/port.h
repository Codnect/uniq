
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
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

#ifndef __UNIQ_IOPORT_H__
#define __UNIQ_IOPORT_H__

typedef unsigned short		port_t;

/*
 * inline assembly
 *
 *  asm ( assembly kodu
 *	 : cikis listesi			 	(istege bagli)
 *	 : giris listesi			 	(istege bagli)
 *	 : geri donen kaydedicilerin listesi 	  	(istege bagli)
 *	 );
 *
 *  ------------
 *  | a | %eax |
 *  | b | %ebx |
 *  | c | %ecx |
 *  | d | %edx |
 *  | S | %esi |
 *  | D | %edi |
 *  ------------
 */


/*
 * port'a bir byte yaz
 *
 * @param port : veri yazilacak port
 * @param data : yazilacak veri
 */
static inline void outb(port_t port,uint8_t data){
	__asm__ volatile("outb %1,%0"
			 :				/* cikis yok */
			 : "dN"(port),"a"(data)		/* girisler  */
			 );
}

/*
 * porttan bir byte oku
 *
 * @param port : veri okunacak port
 */
static inline uint8_t inb(port_t port){
	uint8_t ret_val;
	__asm__ volatile("inb %1,%0"
			 : "=a"(ret_val)		/* cikislar */
			 : "dN"(port)			/* girisler */
			 ); 
	return ret_val;		 
}

/*
 * port'a bir word yaz
 *
 * @param port : veri yazilacak port
 * @param data : yazilacak veri
 */
static inline void outw(port_t port,uint16_t data){
	__asm__ volatile("outw %1,%0"
			 :				/* cikis yok */
			 : "dN"(port),"a"(data)		/* girisler  */
			 );

}

/*
 * porttan bir word oku
 *
 * @param port : veri okunacak port
 */
static inline uint16_t inw(port_t port){
	uint16_t ret_val;
	__asm__ volatile("inw %1,%0"
			 : "=a"(ret_val)		/* cikislar */
			 : "dN"(port)			/* girisler */
			 );	
	return ret_val;
}

#endif /* __UNIQ_IOPORT_H__ */
