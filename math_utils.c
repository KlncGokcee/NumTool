/*
 * \file        math_utils.c

 * \brief       Sayı teorisi algoritmalarının implementasyonu.

 * \developer   Mustafa Sari  No : 425511 , Gokce Kılınc  No : 430634
 */

#include "math_utils.h"

/*
 ** ---------------------------------------------------------------------------
 ** Name : gcd
 **
 ** \brief
 ** İki sayının en büyük ortak bölenini (EBOB) Öklid algoritması
 ** ile hesaplar.
 **
 ** \param [in] long long a - Birinci tam sayı
 ** \param [in] long long b - İkinci tam sayı
 **
 ** \returns long long - EBOB değeri
 ** ---------------------------------------------------------------------------
 */
long long gcd(long long a, long long b) {
    long long temp; // Değişkenlerin değerini kaybetmemek için geçici bir kutu açıyoruz.

    /*
     * Öklid algoritmasının kuralı: GCD(a, b) = GCD(b, a % b)
     * Döngü b (kalan) 0 olana kadar devam eder.
     */
    while (b != 0) {
        temp = b;       // b'nin o anki değerini temp içine saklıyoruz.
        b = a % b;      // b'nin yeni değeri, a'nın b'ye bölümünden kalan (modül) oluyor.
        a = temp;       // a'nın yeni değeri ise sakladığımız eski b değeri oluyor.
    }

    /* Döngü bittiğinde (yani b sıfırlandığında) a'nın içinde EBOB değeri kalır. */
    return a;
}

/*
 ** ---------------------------------------------------------------------------
 ** Name : power_mod
 **
 ** \brief
 ** İkili üs alma yöntemiyle (base^exp) % mod hesaplar.
 ** O(log b) zaman karmaşıklığında çalışır.
 **
 ** \param [in] long long base - Taban değeri
 ** \param [in] long long exp  - Üs değeri
 ** \param [in] long long mod  - Modül değeri
 **
 ** \returns long long - Modüler üs sonucu
 ** ---------------------------------------------------------------------------
 */
long long power_mod(long long base, long long exp, long long mod) {
    long long result = 1; // Çarpmada etkisiz eleman 1'dir.

    /* Taban baştan moddan büyük olabilir, önden bir mod alıp rahatlatıyoruz */
    base = base % mod; 

    /* Taban 0 ise (ve mod > 0), sonuç hep 0'dır */
    if (base == 0) return 0;

    while (exp > 0) {
        /* Eğer üs tek sayı ise sonucu tabanla çarpıyoruz */
        if (exp % 2 == 1) { 
            result = (result * base) % mod; // Her çarpmadan sonra mod!
        }

        /* Üssü yarıya düşür (exp = exp / 2 veya exp >>= 1 aynı şeydir) */
        exp = exp / 2;

        /* Tabanın karesini alıyoruz ki sonraki adımda kullanabilelim */
        base = (base * base) % mod; // Yine her çarpmadan sonra mod!
    }

    return result;
}

/*
 ** ---------------------------------------------------------------------------
 ** Name : is_prime
 **
 ** \brief
 ** Verilen sayının asal olup olmadığını test eder. O(sqrt(N)) karmaşıklığında
 ** ve 6k +/- 1 optimizasyonu ile çalışır.
 **
 ** \param [in] long long n - Test edilecek sayı
 **
 ** \returns int - Asal ise 1 (YES), değilse 0 (NO)
 ** ---------------------------------------------------------------------------
 */
int is_prime(long long n) {
    /* KURAL 1: N <= 1 reddedilmeli */
    if (n <= 1) return 0;

    /* KURAL 2: N = 2 ve N = 3 kabul edilmeli */
    if (n == 2 || n == 3) return 1;

    /* KURAL 3: Çift sayılar ve 3'ün katları baştan elenmeli */
    if (n % 2 == 0 || n % 3 == 0) return 0;

    /* * KURAL 4: i = 5'ten başla, i += 6 adımla.
     * <math.h> yasak olduğu için i <= sqrt(n) yazamayız.
     * Bunun yerine matematiği ters çevirip i * i <= n yazıyoruz.
     */
    for (long long i = 5; i * i <= n; i += 6) {
        /* Sadece 6k - 1 (yani i) ve 6k + 1 (yani i + 2) sayılarına böl */
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0; // Kalansız bölündü, asal değil!
        }
    }

    return 1; // Hiçbir şeye bölünmedi, paşalar gibi asal.
}


/*
 ** ---------------------------------------------------------------------------
 ** Name : mod_inverse
 **
 ** \brief
 ** Genişletilmiş Öklid algoritması ile modüler tersi bulur.
 ** O(log(min(a,m))) zaman karmaşıklığında çalışır.
 **
 ** \param [in]  long long a     - Tersi aranacak sayı
 ** \param [in]  long long m     - Modül değeri
 ** \param [out] int *found      - Ters bulundu mu? (1: evet, 0: hayır)
 **
 ** \returns long long - Modüler ters değeri (bulunamazsa 0)
 ** ---------------------------------------------------------------------------
 */
long long mod_inverse(long long a, long long m, int *found) {
    long long m0 = m;      // Orijinal modül değerini saklıyoruz (en son formülde lazım olacak)
    long long y = 0, x = 1; // x, bizim aradığımız asıl ters değeridir.

    /* Geçici değişkenler (Öklid döngüsünde kullanacağız) */
    long long temp_a = a;
    long long temp_m = m;

    /* Mod 1 ise ters yoktur */
    if (m == 1) {
        *found = 0;
        return 0;
    }

    /* Standart EBOB döngüsünün ters bulacak şekilde genişletilmiş hali */
    while (temp_a > 1) {
        // temp_m sıfır olduysa EBOB 1'den büyüktür, aralarında asal değildir.
        if (temp_m == 0) break;

        // q: bölüm (quotient)
        long long q = temp_a / temp_m;
        
        // Klasik Öklid işlemi (bölümünden kalanı alma)
        long long t = temp_m;
        temp_m = temp_a % temp_m;
        temp_a = t;

        // X ve Y katsayılarını güncelleme (İşte bu kısım "Genişletilmiş" yapıyor)
        t = y;
        y = x - q * y;
        x = t;
    }

    /* Döngü bittiğinde temp_a içinde bu sayıların EBOB'u kalır. 
       Eğer EBOB 1 değilse tersi yoktur kuralını uyguluyoruz. */
    if (temp_a != 1) {
        *found = 0; // Pointer ile main'e haber veriyoruz (tersi yok)
        return 0;
    }

    *found = 1; // Tersini bulduk!

    /* Hoca kuralı: Negatif çıkan sonucu, pozitif eşdeğerine çevir */
    if (x < 0) {
        x = (x % m0 + m0) % m0;
    }

    return x;
}



/*
 ** ---------------------------------------------------------------------------
 ** Name : euler_phi
 **
 ** \brief
 ** Euler totient fonksiyonunu (phi(n)) hesaplar.
 ** O(sqrt(N)) zaman karmaşıklığında çalışır. Float kullanılmaz.
 **
 ** \param [in] long long n - Girdi değeri
 **
 ** \returns long long - phi(n) sonucu
 ** ---------------------------------------------------------------------------
 */
long long euler_phi(long long n) {
    long long result = n; // Başlangıçta sonucu sayının kendisine eşitliyoruz

    /* 2'den başlayarak kök N'e kadar asal çarpanları arıyoruz */
    for (long long p = 2; p * p <= n; p++) {
        
        /* Eğer p, n'i tam bölüyorsa p bir asal çarpandır */
        if (n % p == 0) {
            
            /* Hocanın istediği float kullanmayan tam sayı formülü */
            result = result - (result / p);
            
            /* Sayının içindeki p çarpanlarının hepsini temizliyoruz */
            while (n % p == 0) {
                n = n / p;
            }
        }
    }

    /* Döngü bittiğinde n hala 1'den büyükse, kendisi asal sayıdır 
       ve son bir asal çarpan olarak kalmıştır. */
    if (n > 1) {
        result = result - (result / n);
    }

    return result;
}