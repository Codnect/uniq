/*
 *  Copyright(C) 2014 Codnect Team
 *  Copyright(C) 2014 Burak Köken
 *
 *  This file is part of Nectlib.
 *  
 *  Nectlib is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, version 2 of the License.
 *
 *  Nectlib is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Uniq.  If not, see <http://www.gnu.org/licenses/>.
 *  ---------------------------------------------------------------------------
 *
 *  tam ansi terminal color gibi olmasa da kernel icin olusturulmus basit bir
 *  ansi color sistemi gibidir.
 *
 *  uniq icin ansi color sistemi asagidaki sekilde duzenlenmistir.
 *
 *  \033[yazi_rengi_acikligi;yazi_rengi;yazi_arkaplan_rengi ve sonuna
 * 'm' eki
 *
 *  yazi rengi acikligi : 0-1
 *  yazi rengi : 30-37
 *  yazi arkaplan rengi : 40-55 degerlerini alir
 */

#ifndef __UNIQ_ANSI_COLOR_H__
#define __UNIQ_ANSI_COLOR_H__

/* yazi renkleri */
#define ANSI_COLOR_RED		"\033[31m"
#define ANSI_COLOR_GREEN	"\033[32m"
#define ANSI_COLOR_YELLOW	"\033[33m"	/* daha cok kapali hali kahverengi'ye benziyor */
#define ANSI_COLOR_BROWN	"\033[33m"
#define ANSI_COLOR_BLUE		"\033[34m"
#define ANSI_COLOR_MAGENTA	"\033[35m"
#define ANSI_COLOR_CYAN		"\033[36m"
#define ANSI_COLOR_GRAY		"\033[37m"
#define ANSI_COLOR_DARKGREY	"\033[1;30m"
#define ANSI_COLOR_LRED		"\033[1;31m"
#define ANSI_COLOR_LGREEN	"\033[1;32m"
#define ANSI_COLOR_LYELLOW	"\033[1;33m"
#define ANSI_COLOR_LBLUE	"\033[1;34m"
#define ANSI_COLOR_LMAGENTA	"\033[1;35m"	/* pembe gibi */
#define ANSI_COLOR_LCYAN	"\033[1;36m"
#define ANSI_COLOR_WHITE	"\033[1;37m"

/* arkaplan renkleri */
#define ANSI_COLOR_BG_BLACK	"\033[40m"
#define ANSI_COLOR_BG_RED	"\033[41m"
#define ANSI_COLOR_BG_GREEN	"\033[42m"
#define ANSI_COLOR_BG_YELLOW	"\033[43m"
#define ANSI_COLOR_BG_BLUE	"\033[44m"
#define ANSI_COLOR_BG_MAGENTA	"\033[45m"
#define ANSI_COLOR_BG_CYAN	"\033[46m"
#define ANSI_COLOR_BG_GRAY	"\033[47m"
#define ANSI_COLOR_BG_DARKGREY	"\033[48m"
#define ANSI_COLOR_BG_LRED	"\033[49m"
#define ANSI_COLOR_BG_LGREEN	"\033[50m"
#define ANSI_COLOR_BG_LYELLOW	"\033[51m"
#define ANSI_COLOR_BG_LBLUE	"\033[52m"
#define ANSI_COLOR_BG_LMAGENTA	"\033[53m"	/* pembe gibi */
#define ANSI_COLOR_BG_LCYAN	"\033[54m"
#define ANSI_COLOR_BG_WHITE	"\033[55m"

#endif /* __UNIQ_ANSI_COLOR_H__ */
