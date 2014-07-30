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

#ifndef __UNIQ_KERN_LVLS_H__
#define __UNIQ_KERN_LVLS_H__

typedef enum{
	KERN_INFO = 0,		/* bilgilendirme */
	KERN_NOTICE,		/* normal fakat onemli durum */
	KERN_WARNING,		/* uyari durumlari */
	KERN_ERROR,		/* hata durumlari */
	KERN_CRITIC,		/* kritik durumlar */
	KERN_ALERT,		/* derhal onlem alinmali */
	KERN_EMERG,		/* sistem kullanilamaz halde */
	KERN_NULL		/* devamli ciktilarda kullanmak icin */
}kern_levels_t;

#endif /* __UNIQ_KERN_LVLS_H__ */
