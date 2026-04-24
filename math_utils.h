/* ----------------------------------------------------------------------------------- */
/*
 * \file        math_utils.h
 *
 * \brief       Sayısal hesaplamalar için kullanılan matematik yardımcı fonksiyonları. Sayı teorisi fonksiyonlarının başlık dosyası
 *
 * \developer   Gokce Kılınc, Mustafa Sarı
 */
/* ----------------------------------------------------------------------------------- */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H


/* --------------    Public Functions   ------------------------------------------ */

/*
 * \brief  İki sayının en büyük ortak bölenini Öklid algoritması ile hesaplar.
 * \param  a  Birinci tam sayı.
 * \param  b  İkinci tam sayı.
 * \return EBOB değeri.
 */
long long gcd(long long a, long long b);

/*
 * \brief  Genişletilmiş Öklid algoritması ile modüler tersi bulur.
 * \param  a      Tersi aranacak sayı.
 * \param  m      Modül değeri.
 * \param  found  Tersinin bulunup bulunmadığını döndürür (1: bulundu, 0: yok).
 * \return Modüler ters değeri; bulunamazsa 0.
 */
long long mod_inverse(long long a, long long m, int *found);

/*
 * \brief  İkili üs alma yöntemiyle (base^exp) % mod hesaplar.
 * \param  base  Taban değeri.
 * \param  exp   Üs değeri.
 * \param  mod   Modül değeri.
 * \return Modüler üs sonucu.
 */
long long power_mod(long long base, long long exp, long long mod);

/*
 * \brief  Verilen sayının asal olup olmadığını test eder.
 * \param  n  Test edilecek sayı.
 * \return Asal ise 1, değilse 0.
 */

int is_prime(long long n);

/*
 * \brief  Euler totient fonksiyonunu (phi(n)) hesaplar.
 * \param  n  Girdi değeri.
 * \return phi(n) sonucu.
 */
long long euler_phi(long long n);

#endif /* MATH_UTILS_H */
