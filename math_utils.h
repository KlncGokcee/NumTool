/*
 * math_utils.h
 * Sayı teorisi fonksiyonlarının başlık dosyası.
 */

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

/* EBOB - Öklid Algoritması */
long long gcd(long long a, long long b);

/* Modüler Ters - Genişletilmiş Öklid */
long long mod_inverse(long long a, long long m, int *found);

/* İkili Üs Alma - (base^exp) % mod */
long long power_mod(long long base, long long exp, long long mod);

/* Asallık Testi */
int is_prime(long long n);

/* Euler Totient Fonksiyonu */
long long euler_phi(long long n);

#endif
