/**
 * \file        parser.c
 *
 * \brief       Girdi dosyasını okur, satırları ayrıştırır ve
 *              CommandRecord listesini dinamik olarak doldurur.
 *
 * \developer   Mustafa Sarı, Gokce Kılınc
 *
 *
 */

/* --------------    Include Files    ----------------------------------------------- */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"


/* --------------    Static Variables    -------------------------------------------- */

#define INITIAL_CAPACITY 8    // Başlangıç kapasitesi
#define LINE_SIZE        512  // Maksimum satır uzunluğu

/* --------------    Static Function Prototypes    ----------------------------------- */

static char *trim(char *s);
static int   add_record(CommandList *list, CommandRecord *rec);
static void  parse_command(char *token, CommandRecord *rec);

/*
 **  ---------------------------------------------------------------------------
 **  Name : trim (kırpmak/temizlemek)
 **
 ** \brief
 **          Verilen karakter dizisinin baştaki ve sondaki boşluk
 **          karakterlerini temizler.\n
 **          Component : Parser \n
 **
 ** \param [in] char *s - Temizlenecek karakter dizisi
 **
 ** \returns char*   Temizlenmiş dizinin başlangıç adresi
 **
 **  ---------------------------------------------------------------------------
 */

/* --------------    Function Declarations    ---------------------------------------- */

static char *trim(char *s) {

    // Baştaki boşlukları atla
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;

    // Sondaki boşlukları sil
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return s;
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : create_list
 **
 ** \brief
 **          Dinamik komut listesi oluşturur ve başlangıç belleğini heap'te
 **          ayırır.\n
 **          Component : Parser \n
 **
 ** \returns CommandList*   Başarıda geçerli işaretçi; bellek hatasında NULL
 **
 **  ---------------------------------------------------------------------------
 */
CommandList *create_list(void) {
    CommandList *list = (CommandList *)malloc(sizeof(CommandList));
    if (!list) return NULL;

    list->records = (CommandRecord *)malloc(INITIAL_CAPACITY * sizeof(CommandRecord));
    if (!list->records) {
        free(list);
        return NULL;
    }
    list->size     = 0;
    list->capacity = INITIAL_CAPACITY;
    return list;
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : add_record
 **
 ** \brief
 **          Listeye yeni bir komut kaydı ekler. Kapasite dolarsa realloc
 **          ile iki katına büyütür.\n
 **          Component : Parser \n
 **
 ** \param [in] CommandList   *list - Kaydın ekleneceği liste
 ** \param [in] CommandRecord *rec  - Eklenecek komut kaydı
 **
 ** \returns int    1 : Başarıyla eklendi
 **                 0 : Bellek hatası
 **
 **  ---------------------------------------------------------------------------
 */
static int add_record(CommandList *list, CommandRecord *rec) {

    // Kapasite doldu mu?
    if (list->size >= list->capacity) {
        int new_cap = list->capacity * 2;
        CommandRecord *tmp = (CommandRecord *)realloc(
                list->records, new_cap * sizeof(CommandRecord));
        if (!tmp) return 0; // Bellek hatası
        list->records  = tmp;
        list->capacity = new_cap;
    }
    list->records[list->size++] = *rec;
    return 1;
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : parse_command
 **
 ** \brief
 **          Tek bir komut metnini (örn: "GCD 48 18") ayrıştırır ve
 **          CommandRecord yapısını komut adı ile parametrelerle doldurur.
 **          Bilgisayarın anlayacağı bir formata sokar.\n
 **
 **          Component : Parser \n
 **
 ** \param [in]  char          *token - Ayrıştırılacak komut metni
 ** \param [out] CommandRecord *rec   - Doldurulan komut kaydı
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */
static void parse_command(char *token, CommandRecord *rec) {
    memset(rec, 0, sizeof(CommandRecord));

    // Komut adını oku
    char cmd[20] = {0};
    int  offset  = 0;

    if (sscanf(token, "%19s%n", cmd, &offset) < 1) {
        snprintf(rec->result, sizeof(rec->result), "PARSE_ERROR");
        rec->is_error = 1;
        return;
    }
    snprintf(rec->command, sizeof(rec->command), "%s", cmd);

    // Parametreleri oku
    char     *rest = token + offset;
    long long  val;
    int        n;

    rec->arg_count = 0;
    while (sscanf(rest, " %lld%n", &val, &n) == 1
           && rec->arg_count < 4) {
        rec->args[rec->arg_count++] = val;
        rest += n;
    }
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : parse_file
 **
 ** \brief
 **          Girdi dosyasını satır satır okur. Yorum satırlarını (//) ve boş
 **          satırları atlar. Noktalı virgülle ayrılmış komutları bölerek
 **          listeye ekler.\n
 **          Component : Parser \n
 **
 ** \param [in]  const char  *filename - Okunacak dosyanın yolu
 ** \param [out] CommandList *list     - Ayrıştırılan komutların ekleneceği liste
 **
 ** \returns int    1 : Başarıyla tamamlandı
 **                 0 : Dosya açılamadı veya bellek hatası
 **
 **  ---------------------------------------------------------------------------
 */
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
            if (t[0] != '\0') {
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

/*
 **  ---------------------------------------------------------------------------
 **  Name : free_list
 **
 ** \brief
 **          Dinamik listeyi ve tüm ayrılmış belleği serbest bırakır.
 **          NULL işaretçi ile çağrılması güvenlidir.\n
 **          Component : Parser \n
 **
 ** \param [in] CommandList *list - Serbest bırakılacak liste işaretçisi
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */
void free_list(CommandList *list) {
    if (!list) return;
    free(list->records);
    free(list);
}
