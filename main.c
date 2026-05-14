/*
 * \file        main.c
 *
 * \brief       NumTool - Sayı Teorisi Hesaplama Aracı.
 *              Ana kontrol akışı: argüman kontrolü, dosya işlemleri,
 *              komutların çalıştırılması ve sonuçların yazılması.
 *
 * \developer   Gokce Kılınc  No : 430634 , Mustafa Sari  No : 425511
 *
 *
 */

/* --------------    Include Files    ----------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "math_utils.h"


/* --------------    Static Function Prototypes    ----------------------------------- */


static void komutu_calistir(CommandRecord *kayit);
static void sonuclari_yazdir(const char *dosya_adi, CommandList *liste);



/*
 **  ---------------------------------------------------------------------------

 **  Name : komutu_calistir (execute_record)
 **
 ** \brief
 **          Her CommandRecord'u işler: uygun matematik fonksiyonunu çağırır,
 **          sonucu result alanına yazar, hata varsa is_error = 1 yapar.\n
 **          Component : Main \n
 **
 ** \param [in] CommandRecord *kayit - İşlenecek komut kaydı
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */

/* --------------    Function Declarations    ---------------------------------------- */


static void komutu_calistir(CommandRecord *kayit) {
    char *komut = kayit->command;

    /* ---- GCD komutu (A ve B'nin EBOB'unu hesaplar) ---- */

    if (strcmp(komut, "GCD") == 0) {
        if (kayit->arg_count < 2
            || kayit->args[0] <= 0 || kayit->args[1] <= 0) {
            snprintf(kayit->result, sizeof(kayit->result),
                     "ERROR_INVALID_INPUT");
            kayit->is_error = 1;
        } else {
            long long sonuc = gcd(kayit->args[0], kayit->args[1]);
            snprintf(kayit->result, sizeof(kayit->result), "%lld", sonuc);
        }


        /* ---- POW komutu (Taban üs) ---- */

    } else if (strcmp(komut, "POW") == 0) {
        if (kayit->arg_count < 3
            || kayit->args[2] <= 0 || kayit->args[1] < 0) {
            snprintf(kayit->result, sizeof(kayit->result),
                     "ERROR_INVALID_INPUT");
            kayit->is_error = 1;
        } else {
            long long sonuc = power_mod(kayit->args[0],
                                        kayit->args[1],
                                        kayit->args[2]);
            snprintf(kayit->result, sizeof(kayit->result), "%lld", sonuc);
        }

        /* ---- PRIME komutu ---- */
    } else if (strcmp(komut, "PRIME") == 0) {
        if (kayit->arg_count < 1 || kayit->args[0] < 2) {
            snprintf(kayit->result, sizeof(kayit->result),
                     "ERROR_INVALID_INPUT");
            kayit->is_error = 1;
        } else {
            snprintf(kayit->result, sizeof(kayit->result),
                     is_prime(kayit->args[0]) ? "YES" : "NO");
        }


        /* ---- INV komutu (A'nın M modundaki tersini hesaplar) ---- */

    } else if (strcmp(komut, "INV") == 0) {
        if (kayit->arg_count < 2 || kayit->args[1] <= 1) {
            snprintf(kayit->result, sizeof(kayit->result),
                     "ERROR_INVALID_INPUT");
            kayit->is_error = 1;
        } else {
            int bulundu = 0;
            long long sonuc = mod_inverse(kayit->args[0],
                                          kayit->args[1], &bulundu);
            if (!bulundu) {
                snprintf(kayit->result, sizeof(kayit->result),
                         "ERROR_NO_INVERSE");
                kayit->is_error = 1;
            } else {
                snprintf(kayit->result, sizeof(kayit->result),
                         "%lld", sonuc);
            }
        }


        /* ---- PHI komutu (Euler Totient fonksiyonunu hesaplar) ---- */

    } else if (strcmp(komut, "PHI") == 0) {
        if (kayit->arg_count < 1 || kayit->args[0] < 1) {
            snprintf(kayit->result, sizeof(kayit->result),
                     "ERROR_INVALID_INPUT");
            kayit->is_error = 1;
        } else {
            long long sonuc = euler_phi(kayit->args[0]);
            snprintf(kayit->result, sizeof(kayit->result), "%lld", sonuc);
        }


        /* ---- CHECK komutu (Modüler tersin doğruluğunu test eder) ---- */

    } else if (strcmp(komut, "CHECK") == 0) {
        if (kayit->arg_count < 2 || kayit->args[1] <= 1) {
            snprintf(kayit->result, sizeof(kayit->result),
                     "ERROR_INVALID_INPUT");
            kayit->is_error = 1;
        } else {
            long long taban    = kayit->args[0];
            long long mod      = kayit->args[1];
            int       bulundu  = 0;
            long long ters     = mod_inverse(taban, mod, &bulundu);

            if (!bulundu) {
                snprintf(kayit->result, sizeof(kayit->result),
                         "ERROR_NO_INVERSE");
                kayit->is_error = 1;
            } else {
                /* (taban * ters) % mod == 1 ise CORRECT */
                long long dogrulama = (taban % mod * ters % mod) % mod;
                snprintf(kayit->result, sizeof(kayit->result),
                         dogrulama == 1 ? "CORRECT" : "FAILED");
            }
        }

        /* ---- Bilinmeyen komut ---- */
    } else {
        snprintf(kayit->result, sizeof(kayit->result),
                 "ERROR_UNKNOWN_COMMAND");
        kayit->is_error = 1;
    }
}

/*
 **  ---------------------------------------------------------------------------

 **  Name : sonuclari_yazdir (write_results)
 **
 ** \brief
 **          Tüm komut kayıtlarını çıktı dosyasına yazar.
 **          Format: KOMUT ARG1 ARG2 ... -> SONUÇ\n
 **          Component : Main \n
 **
 ** \param [in] const char    *dosya_adi - Yazılacak çıktı dosyasının yolu
 ** \param [in] CommandList   *liste     - Yazılacak komut listesi
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */

static void sonuclari_yazdir(const char *dosya_adi, CommandList *liste) {

    FILE *dosya = fopen(dosya_adi, "w");
    if (!dosya) {
        fprintf(stderr, "Hata: '%s' cikis dosyasi acilamadi.\n",
                dosya_adi);
        return;
    }

    for (int i = 0; i < liste->size; i++) {
        CommandRecord *kayit = &liste->records[i];

        /* Komut adını yaz */
        fprintf(dosya, "%s", kayit->command);

        /* Parametreleri yaz */
        for (int j = 0; j < kayit->arg_count; j++) {
            fprintf(dosya, " %lld", kayit->args[j]);
        }

        /* Sonucu yaz */
        fprintf(dosya, " -> %s\n", kayit->result);
    }

    fclose(dosya);
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : main
 **
 ** \brief
 **          Program giriş noktası. Argüman kontrolü, liste oluşturma,
 **          komut işleme ve sonuç yazma adımlarını sırasıyla yürütür.\n
 **          Component : Main \n
 **
 ** \param [in] int    argc     - Komut satırı argüman sayısı
 ** \param [in] char  *argv[]   - Komut satırı argümanları
 **                               argv[1]: Girdi dosyası yolu
 **                               argv[2]: Çıktı dosyası yolu
 **
 ** \returns int    0 : Başarıyla tamamlandı
 **                 1 : Hata oluştu
 **
 **  ---------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {

    /* Argüman sayısı kontrolü */

    if (argc != 3) {
        fprintf(stderr,
                "Kullanim: %s <giris_dosyasi> <cikis_dosyasi>\n",
                argv[0]);
        return 1;
    }

    const char *giris_dosyasi  = argv[1];
    const char *cikis_dosyasi  = argv[2];

    /* Dinamik komut listesini oluştur */
    CommandList *liste = create_list();
    if (!liste) {
        fprintf(stderr, "Hata: Bellek ayrilamadi.\n");
        return 1;
    }

    /* Girdi dosyasını ayrıştır */
    if (!parse_file(giris_dosyasi, liste)) {
        free_list(liste);
        return 1;
    }

    /* Komutları sırayla çalıştır */
    for (int i = 0; i < liste->size; i++) {
        komutu_calistir(&liste->records[i]);
    }

    /* Sonuçları dosyaya yazdır */
    sonuclari_yazdir(cikis_dosyasi, liste);

    printf("Tamamlandi: %d komut islendi. Sonuclar '%s' dosyasina yazildi.\n", liste->size, cikis_dosyasi);

    /* Belleği temizle ve dosyaları kapat */
    free_list(liste);
    return 0;
}
