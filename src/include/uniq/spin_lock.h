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
#define __UNIQ_SPIN_LOCK_H__


/*
 * spin_lock, ayni anda birden fazla process calistirabilen isletim sistemleri
 * icin onemli. mesela 1. process'imiz disk'e veri yaziyor ve o anda baska bir
 * process daha diske veri yazmak yada ayni adrese mudahele etmek istediler
 * ilk once erisen process bir kilit(spin_lock) olusturur ve diger process ilk
 * erisen process'i beklemek zorundadir. ilk process isi bittiginde kilidi
 * (spin_unlock) kaldirir ve diger process isini yapmaya baslar. spin_lock durumda
 * cpu baya yorulur fakat kullanmak zorundayiz.
 *
 * kulladigimiz bu fonksiyonlar(__sync_lock_test_and_set,__sync_lock_release) 
 * yada her ne dersek diyelim buyuk ihtimal makrodur. bunlar gcc'nin bize sundugu 
 * yararlardan sadece biri ;).
 *
 * ornek vermek gerekirse iki process'de ayni fonksiyona cagri yapsin 
 *
 * ------------------------------------------------------------------------------------
 * uint32_t lock = 0; // kilit olacak degisken 
 *
 * // degiskenin adresini veriyoruz,ve spin_lock fonksiyonunu icini inceledigimizde
 * // lock degerini inceliyor eger 0 ise kilit yok , degerini 1 yaparak kilit
 * // olusturuluyor.
 * // diger process'de bu fonksiyonu cagriacak ve spin_lock ile lock'un adresini
 * // gonderiyor ve kontrol ediyor eger adresteki deger 0 degilse yani kilit varsa
 * // 0 olana (kilit kalkana) kadar 2. process beklemek zorunda,ilk process lock degerini 
 * // 0 yaptigi an(kilidi kaldirdiginda) diger process girdigi kontrol dongusunden cikar.
 * // arastirmalarim sonucu boyle bir aciklama ortaya cikardim ;). yararli bir kaynak
 * // olmasi dilegimle.
 *
 * spin_lock(&lock); 
 * // ...
 * spin_unlock(&lock); // deger 0 yapilarak kilit kalkiyor.
 *
 * ------------------------------------------------------------------------------------
 *
 */
 

/*
 * spin_lock,kilit olustur
 *
 * @param lock_addr : kilit olusturulacak adres
 */
static void spin_lock(uint32_t volatile *lock_addr){
  
  	while(__sync_lock_test_and_set(lock_addr, 0x1));
  	
}
  
/*
 * spin_unlock,kilidi kaldir
 *
 * @param lock_addr : kilidi kaldirilacak adres
 */
static void spin_unlock(uint32_t volatile *lock_addr){
  
  	__sync_lock_release(lock_addr);
  	
}



#endif /* __UNIQ_SPIN_LOCK_H__ */
