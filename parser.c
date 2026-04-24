/*
 * parser.c
 * Girdi dosyasını okur, satırları ayrıştırır ve
 * CommandRecord listesini dinamik olarak doldurur.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

#define INITIAL_CAPACITY 8   /* Başlangıç kapasitesi */
#define LINE_SIZE        512  /* Maksimum satır uzunluğu */

/* -------------------------------------------------------
 * Baştaki ve sondaki boşlukları temizler.
 * ------------------------------------------------------- */
static char *trim(char *s) {
    /* Baştaki boşlukları atla */
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;

    /* Sondaki boşlukları sil */
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return s;
}

/* -------------------------------------------------------
 * Dinamik liste oluşturur ve başlangıç belleğini ayırır.
 * ------------------------------------------------------- */
CommandList *create_list(void) {
    CommandList *list = (CommandList *)malloc(sizeof(CommandList));
    if (!list) return NULL;

    list->records  = (CommandRecord *)malloc(INITIAL_CAPACITY
                                             * sizeof(CommandRecord));
    if (!list->records) {
        free(list);
        return NULL;
    }
    list->size     = 0;
    list->capacity = INITIAL_CAPACITY;
    return list;
}

/* -------------------------------------------------------
 * Listeye yeni bir kayıt ekler.
 * Kapasite dolarsa realloc ile iki katına büyütür.
 * ------------------------------------------------------- */
static int add_record(CommandList *list, CommandRecord *rec) {
    /* Kapasite doldu mu? */
    if (list->size >= list->capacity) {
        int new_cap = list->capacity * 2;
        CommandRecord *tmp = (CommandRecord *)realloc(
            list->records, new_cap * sizeof(CommandRecord));
        if (!tmp) return 0; /* Bellek hatası */
        list->records  = tmp;
        list->capacity = new_cap;
    }
    list->records[list->size++] = *rec;
    return 1;
}

/* -------------------------------------------------------
 * Tek bir komut metnini (örn: "GCD 48 18") ayrıştırır
 * ve CommandRecord yapısını doldurur.
 * ------------------------------------------------------- */
static void parse_command(char *token, CommandRecord *rec) {
    memset(rec, 0, sizeof(CommandRecord));

    /* Komut adını oku */
    char cmd[20] = {0};
    int  offset  = 0;

    /* sscanf ile komut adını al, kaç karakter okunduğunu bul */
    if (sscanf(token, "%19s%n", cmd, &offset) < 1) {
        snprintf(rec->result, sizeof(rec->result), "PARSE_ERROR");
        rec->is_error = 1;
        return;
    }
    snprintf(rec->command, sizeof(rec->command), "%s", cmd);

    /* Parametreleri oku */
    char *rest = token + offset;
    rec->arg_count = 0;
    long long val;
    int  n;

    while (sscanf(rest, " %lld%n", &val, &n) == 1
           && rec->arg_count < 4) {
        rec->args[rec->arg_count++] = val;
        rest += n;
    }
}

/* -------------------------------------------------------
 * Girdi dosyasını satır satır okur.
 * Yorum satırlarını (#) ve boş satırları atlar.
 * Noktalı virgülle ayrılmış komutları böler.
 * ------------------------------------------------------- */
int parse_file(const char *filename, CommandList *list) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Hata: '%s' dosyasi acilamadi.\n", filename);
        return 0;
    }

    char line[LINE_SIZE];

    while (fgets(line, sizeof(line), fp)) {
        char *trimmed = trim(line);

        /* Boş satır veya yorum satırı ise atla */
        if (trimmed[0] == '\0' || trimmed[0] == '#') continue;

        /* Noktalı virgülle ayrılmış komutları böl */
        char *saveptr = NULL;
        char *token   = strtok_r(trimmed, ";", &saveptr);

        while (token != NULL) {
            char *t = trim(token);
            if (t[0] != '\0') { /* Boş token değilse ekle */
                CommandRecord rec;
                parse_command(t, &rec);
                if (!add_record(list, &rec)) {
                    fprintf(stderr, "Hata: Komut listesi genisletilemedi.\n");
                    fclose(fp);
                    return 0;
                }
            }
            token = strtok_r(NULL, ";", &saveptr);
        }
    }

    fclose(fp);
    return 1;
}

/* -------------------------------------------------------
 * Dinamik listeyi ve tüm belleği serbest bırakır.
 * ------------------------------------------------------- */
void free_list(CommandList *list) {
    if (!list) return;
    free(list->records);
    free(list);
}
