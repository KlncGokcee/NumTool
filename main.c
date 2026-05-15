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
static void sonucu_yazdir(CommandRecord *kayit);



/* --------------    Defines    ----------------------------------------------------- */

#define SATIR_BOYUTU 512   /* Maksimum girdi satırı uzunluğu */

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

/*
 **  ---------------------------------------------------------------------------
 **  Name : sonucu_yazdir (print_result)
 **
 ** \brief
 **          Tek bir komut kaydının sonucunu terminale (stdout) yazar.
 **          Format: KOMUT ARG1 ARG2 ... -> SONUÇ\n
 **          Component : Main \n
 **
 ** \param [in] CommandRecord *kayit - Yazdırılacak komut kaydı
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */
static void sonucu_yazdir(CommandRecord *kayit) {

    /* Komut adını yaz */
    printf("%s", kayit->command);

    /* Parametreleri yaz */
    for (int j = 0; j < kayit->arg_count; j++) {
        printf(" %lld", kayit->args[j]);
    }

    /* Sonucu yaz */
    printf(" -> %s\n", kayit->result);
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : main
 **
 ** \brief
 **          Program giriş noktası. Interaktif terminal döngüsünü başlatır.
 **          Kullanıcıdan satır satır komut alır, hesaplar ve sonucu
 **          terminale yazar. "exit" veya "quit" ile çıkılır.\n
 **          Component : Main \n
 **
 ** \param [in] int    argc     - Komut satırı argüman sayısı (kullanılmıyor)
 ** \param [in] char  *argv[]   - Komut satırı argümanları   (kullanılmıyor)
 **
 ** \returns int    0 : Başarıyla tamamlandı
 **                 1 : Hata oluştu
 **
 **  ---------------------------------------------------------------------------
 */
int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv;

    char satir[SATIR_BOYUTU];

    /* Karşılama mesajı */
    printf("============================================\n");
    printf("  NumTool - Sayi Teorisi Hesaplama Araci\n");
    printf("============================================\n");
    printf("Komutlar : GCD  POW  PRIME  INV  PHI  CHECK\n");
    printf("Cikis    : exit veya quit\n");
    printf("--------------------------------------------\n");

    /* Interaktif komut döngüsü */
    while (1) {
        /* Komut istemi yaz */
        printf("numtool> ");
        fflush(stdout);

        /* Kullanıcıdan satır oku; EOF gelirse (Ctrl+D) çık */
        if (fgets(satir, sizeof(satir), stdin) == NULL) {
            printf("\nCikiliyor...\n");
            break;
        }

        /* Boş satırı atla */
        if (satir[0] == '\n') continue;

        /* Yorum satırını atla */
        if (satir[0] == '#') continue;

        /* "exit" veya "quit" kontrolü */
        if (strncmp(satir, "exit", 4) == 0 ||
            strncmp(satir, "quit", 4) == 0) {
            printf("Cikiliyor...\n");
            break;
        }

        /* Geçici tek-kayıt listesi oluştur */
        CommandList *liste = create_list();
        if (!liste) {
            fprintf(stderr, "Hata: Bellek ayrilamadi.\n");
            return 1;
        }

        /* Satırı ayrıştır (noktalı virgülle birden fazla komut olabilir) */
        char *kayit_ptr = NULL;
        char *parca     = strtok_r(satir, ";", &kayit_ptr);

        while (parca != NULL) {
            /* Baş-sondaki boşlukları temizle */
            while (*parca == ' ' || *parca == '\t') parca++;
            char *son = parca + strlen(parca) - 1;
            while (son > parca && (*son == ' ' || *son == '\t' ||
                                   *son == '\n' || *son == '\r')) {
                *son-- = '\0';
            }

            if (parca[0] != '\0') {
                /* Geçici CommandRecord oluştur ve ayrıştır */
                CommandRecord kayit;
                memset(&kayit, 0, sizeof(CommandRecord));

                char komut_adi[20] = {0};
                int  ofset         = 0;

                if (sscanf(parca, "%19s%n", komut_adi, &ofset) >= 1) {
                    snprintf(kayit.command, sizeof(kayit.command),
                             "%s", komut_adi);

                    char     *kalan = parca + ofset;
                    long long deger;
                    int       n;
                    kayit.arg_count = 0;

                    while (sscanf(kalan, " %lld%n", &deger, &n) == 1
                           && kayit.arg_count < KOMUT_MAKS_PARAMETRE) {
                        kayit.args[kayit.arg_count++] = deger;
                        kalan += n;
                    }

                    kayit_ekle(liste, &kayit);
                }
            }
            parca = strtok_r(NULL, ";", &kayit_ptr);
        }

        /* Komutları çalıştır ve sonuçları terminale yaz */
        for (int i = 0; i < liste->size; i++) {
            komutu_calistir(&liste->records[i]);
            sonucu_yazdir(&liste->records[i]);
        }

        free_list(liste);
    }

    return 0;
}
