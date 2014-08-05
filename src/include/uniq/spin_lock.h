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

#ifndef __UNIQ_SPIN_LOCK_H__
#ifndef __UNIQ_SPIN_LOCK_H__


/*
 * spin_lock, ayni anda birden fazla process calistirabilen isletim sistemleri
 * icin onemli. mesela 1. process'imiz disk'e veri yaziyor ve o anda baska bir
 * process daha diske veri yazmak yada ayni adrese mudahele etmek istediler
 * ilk once erisen process bir kilit(spin_lock) olusturur ve diger process ilk
 * erisen process'i beklemek zorundadir. ilk process isi bittiginde kilidi
 * (spin_unlock) kaldirir ve diger process isini yapmaya baslar. spin_lock durumda
 * cpu baya yorulur fakat kullanmak zorundayiz.
 *
 * spin_lock ile ilgili bir arastirma daha yaptim. kulladigimiz bu fonksiyonlar
 * (__sync_lock_test_and_set,__sync_lock_release) yada her ne dersek diyelim buyuk
 * ihtimal makrodur. bunlar gcc'nin bize sundugu yararlardan sadece biri.
 */
 
#ifndef spin_lock
#define spin_lock

  /*
   * spin_lock
   *
   * @param lock_addr :
   */
  static void spin_lock(uint32_t volatile *lock_addr){
  
  	while(__sync_lock_test_and_set(lock_addr, 0x01));
  	
  }
  
  /*
   * spin_unlock
   *
   * @param lock_addr :
   */
  static void spin_unlock(uint32_t volatile *lock_addr){
  
  	__sync_lock_release(lock_addr);
  	
  }

#endif

#endif /* __UNIQ_SPIN_LOCK_H__ */
