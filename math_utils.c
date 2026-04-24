/*
 * math_utils.c
 * Tüm sayı teorisi algoritmalarının implementasyonu.
 * GCD, INV, POW, PRIME, PHI fonksiyonlarını içerir.
 */

#include "math_utils.h"

/* -------------------------------------------------------
 * GCD: Öklid Algoritması
 * O(log(min(a,b))) zaman karmaşıklığı
 * ------------------------------------------------------- */
long long gcd(long long a, long long b) {
    /* b sıfır olana kadar modül al, sonra a'yı döndür */
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* -------------------------------------------------------
 * Genişletilmiş Öklid Algoritması
 * a*x + b*y = gcd(a,b) denkleminin x ve y katsayılarını bulur.
 * Modüler ters hesaplamak için kullanılır.
 * ------------------------------------------------------- */
static long long extended_gcd(long long a, long long b,
                               long long *x, long long *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long x1, y1;
    long long g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

/* -------------------------------------------------------
 * mod_inverse: Modüler Ters
 * (a * sonuç) % m = 1 eşitliğini sağlayan sonucu bulur.
 * found: ters bulunamazsa 0, bulunursa 1 yapılır.
 * ------------------------------------------------------- */
long long mod_inverse(long long a, long long m, int *found) {
    long long x, y;
    long long g = extended_gcd(a, m, &x, &y);

    /* GCD 1 değilse modüler ters yoktur */
    if (g != 1) {
        *found = 0;
        return 0;
    }

    *found = 1;
    /* x negatif çıkabilir, pozitife çevir */
    return (x % m + m) % m;
}

/* -------------------------------------------------------
 * power_mod: İkili Üs Alma
 * (base^exp) % mod işlemi O(log exp) karmaşıklığında.
 * ------------------------------------------------------- */
long long power_mod(long long base, long long exp, long long mod) {
    long long result = 1;
    base = base % mod;

    while (exp > 0) {
        /* Üs tek ise sonuca tabanı çarp */
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        /* Tabanın karesini al, üssü ikiye böl */
        base = (base * base) % mod;
        exp = exp >> 1; /* exp / 2 ile aynı */
    }
    return result;
}

/* -------------------------------------------------------
 * is_prime: Asallık Testi
 * O(sqrt(N)) zaman karmaşıklığı.
 * 6k±1 optimizasyonu uygulanmıştır.
 * ------------------------------------------------------- */
int is_prime(long long n) {
    /* 1 ve altı asal değil */
    if (n <= 1) return 0;
    /* 2 ve 3 asaldır */
    if (n == 2 || n == 3) return 1;
    /* Çift sayılar ve 3'ün katları asal değil */
    if (n % 2 == 0 || n % 3 == 0) return 0;

    /* i = 5'ten başla, sadece 6k-1 ve 6k+1 formundaki
     * sayılarla böl, i*i <= n sınırına kadar devam et */
    long long i = 5;
    while (i * i <= n) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
        i += 6;
    }
    return 1;
}

/* -------------------------------------------------------
 * euler_phi: Euler Totient Fonksiyonu
 * PHI(N) = N * Π(1 - 1/p) (p: asal çarpanlar)
 * Hassasiyet için tam sayı aritmetiği kullanılır.
 * O(sqrt(N)) karmaşıklığında asal çarpanlara ayırma.
 * ------------------------------------------------------- */
long long euler_phi(long long n) {
    long long result = n;
    long long temp   = n;

    /* Asal çarpanlara ayır ve formülü uygula */
    for (long long p = 2; p * p <= temp; p++) {
        if (temp % p == 0) {
            /* p bir asal çarpan: result = result - result/p */
            result = result - (result / p);
            /* p'nin tüm katlarını böl */
            while (temp % p == 0) {
                temp /= p;
            }
        }
    }
    /* Kalan asal çarpan varsa (temp > 1) */
    if (temp > 1) {
        result = result - (result / temp);
    }
    return result;
}
