/*
 * main.c
 * NumTool - Sayı Teorisi Hesaplama Aracı
 * Ana kontrol akışı: argüman kontrolü, dosya işlemleri,
 * komutların çalıştırılması ve sonuçların yazılması.
 *
 * Kullanım: ./numtool <giris_dosyasi> <cikis_dosyasi>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "math_utils.h"

/* -------------------------------------------------------
 * Her CommandRecord'u işler: uygun fonksiyonu çağırır,
 * sonucu result alanına yazar, hata varsa is_error = 1 yapar.
 * ------------------------------------------------------- */
static void execute_record(CommandRecord *rec) {
    char *cmd = rec->command;

    /* ---- GCD komutu ---- */
    if (strcmp(cmd, "GCD") == 0) {
        if (rec->arg_count < 2
            || rec->args[0] <= 0 || rec->args[1] <= 0) {
            snprintf(rec->result, sizeof(rec->result),
                     "ERROR_INVALID_INPUT");
            rec->is_error = 1;
        } else {
            long long r = gcd(rec->args[0], rec->args[1]);
            snprintf(rec->result, sizeof(rec->result), "%lld", r);
        }

    /* ---- POW komutu ---- */
    } else if (strcmp(cmd, "POW") == 0) {
        if (rec->arg_count < 3
            || rec->args[2] <= 0 || rec->args[1] < 0) {
            snprintf(rec->result, sizeof(rec->result),
                     "ERROR_INVALID_INPUT");
            rec->is_error = 1;
        } else {
            long long r = power_mod(rec->args[0],
                                    rec->args[1],
                                    rec->args[2]);
            snprintf(rec->result, sizeof(rec->result), "%lld", r);
        }

    /* ---- PRIME komutu ---- */
    } else if (strcmp(cmd, "PRIME") == 0) {
        if (rec->arg_count < 1 || rec->args[0] < 2) {
            snprintf(rec->result, sizeof(rec->result),
                     "ERROR_INVALID_INPUT");
            rec->is_error = 1;
        } else {
            snprintf(rec->result, sizeof(rec->result),
                     is_prime(rec->args[0]) ? "YES" : "NO");
        }

    /* ---- INV komutu ---- */
    } else if (strcmp(cmd, "INV") == 0) {
        if (rec->arg_count < 2 || rec->args[1] <= 1) {
            snprintf(rec->result, sizeof(rec->result),
                     "ERROR_INVALID_INPUT");
            rec->is_error = 1;
        } else {
            int found = 0;
            long long r = mod_inverse(rec->args[0],
                                      rec->args[1], &found);
            if (!found) {
                snprintf(rec->result, sizeof(rec->result),
                         "ERROR_NO_INVERSE");
                rec->is_error = 1;
            } else {
                snprintf(rec->result, sizeof(rec->result),
                         "%lld", r);
            }
        }

    /* ---- PHI komutu ---- */
    } else if (strcmp(cmd, "PHI") == 0) {
        if (rec->arg_count < 1 || rec->args[0] < 1) {
            snprintf(rec->result, sizeof(rec->result),
                     "ERROR_INVALID_INPUT");
            rec->is_error = 1;
        } else {
            long long r = euler_phi(rec->args[0]);
            snprintf(rec->result, sizeof(rec->result), "%lld", r);
        }

    /* ---- CHECK komutu ---- */
    } else if (strcmp(cmd, "CHECK") == 0) {
        if (rec->arg_count < 2 || rec->args[1] <= 1) {
            snprintf(rec->result, sizeof(rec->result),
                     "ERROR_INVALID_INPUT");
            rec->is_error = 1;
        } else {
            long long a = rec->args[0];
            long long m = rec->args[1];
            int found   = 0;
            long long inv = mod_inverse(a, m, &found);

            if (!found) {
                snprintf(rec->result, sizeof(rec->result),
                         "ERROR_NO_INVERSE");
                rec->is_error = 1;
            } else {
                /* (a * inv) % m == 1 ise CORRECT */
                long long check = (a % m * inv % m) % m;
                snprintf(rec->result, sizeof(rec->result),
                         check == 1 ? "CORRECT" : "FAILED");
            }
        }

    /* ---- Bilinmeyen komut ---- */
    } else {
        snprintf(rec->result, sizeof(rec->result),
                 "ERROR_UNKNOWN_COMMAND");
        rec->is_error = 1;
    }
}

/* -------------------------------------------------------
 * Tüm komut kayıtlarını çıktı dosyasına yazar.
 * Format: KOMUT ARG1 ARG2 ... -> SONUÇ
 * ------------------------------------------------------- */
static void write_results(const char *filename,
                           CommandList *list) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Hata: '%s' cikis dosyasi acilamadi.\n",
                filename);
        return;
    }

    for (int i = 0; i < list->size; i++) {
        CommandRecord *rec = &list->records[i];

        /* Komut adını yaz */
        fprintf(fp, "%s", rec->command);

        /* Parametreleri yaz */
        for (int j = 0; j < rec->arg_count; j++) {
            fprintf(fp, " %lld", rec->args[j]);
        }

        /* Sonucu yaz */
        fprintf(fp, " -> %s\n", rec->result);
    }

    fclose(fp);
}

/* -------------------------------------------------------
 * main: Argüman kontrolü, liste oluşturma, işleme, yazma.
 * ------------------------------------------------------- */
int main(int argc, char *argv[]) {
    /* Argüman sayısı kontrolü */
    if (argc != 3) {
        fprintf(stderr,
            "Kullanim: %s <giris_dosyasi> <cikis_dosyasi>\n",
            argv[0]);
        return 1;
    }

    const char *input_file  = argv[1];
    const char *output_file = argv[2];

    /* Dinamik komut listesini oluştur */
    CommandList *list = create_list();
    if (!list) {
        fprintf(stderr, "Hata: Bellek ayrilamadi.\n");
        return 1;
    }

    /* Girdi dosyasını ayrıştır */
    if (!parse_file(input_file, list)) {
        free_list(list);
        return 1;
    }

    /* Her komutu çalıştır */
    for (int i = 0; i < list->size; i++) {
        execute_record(&list->records[i]);
    }

    /* Sonuçları çıktı dosyasına yaz */
    write_results(output_file, list);

    printf("Tamamlandi: %d komut islendi. Sonuclar '%s' dosyasina yazildi.\n",
           list->size, output_file);

    /* Tüm dinamik belleği serbest bırak (bellek sızıntısı yok) */
    free_list(list);
    return 0;
}
