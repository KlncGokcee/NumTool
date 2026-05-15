/*
 * \file        parser.c
 *
 * \brief       Girdi dosyasını okur, satırları ayrıştırır ve
 *              CommandRecord listesini dinamik olarak doldurur.
 *
 * \developer   Gokce Kılınc  No : 430634 , Mustafa Sari  No : 425511
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

#define BASLANGIC_KAPASITESI 8    /* Başlangıç kapasitesi       */
#define SATIR_BOYUTU        512   /* Maksimum satır uzunluğu    */

/* --------------    Static Function Prototypes    ----------------------------------- */

static char *bosluklari_temizle(char *s);
static void  komutu_ayristir(char *parca, CommandRecord *kayit);


/*
 **  ---------------------------------------------------------------------------
 **  Name : bosluklari_temizle (trim)
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


static char *bosluklari_temizle(char *s) {
    // Baştaki boşlukları atla
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;

    // Sondaki boşlukları sil
    char *son = s + strlen(s) - 1;
    while (son > s && isspace((unsigned char)*son)) son--;
    *(son + 1) = '\0';
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
    CommandList *liste = (CommandList *)malloc(sizeof(CommandList));
    if (!liste) return NULL;

    liste->records = (CommandRecord *)malloc(BASLANGIC_KAPASITESI
                                             * sizeof(CommandRecord));
    if (!liste->records) {
        free(liste);
        return NULL;
    }
    liste->size     = 0;
    liste->capacity = BASLANGIC_KAPASITESI;
    return liste;
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : kayit_ekle (add_record)
 **
 ** \brief
 **          Listeye yeni bir komut kaydı ekler. Kapasite dolarsa realloc
 **          ile iki katına büyütür.\n
 **          Component : Parser \n
 **
 ** \param [in] CommandList   *liste - Kaydın ekleneceği liste
 ** \param [in] CommandRecord *kayit - Eklenecek komut kaydı
 **
 ** \returns int    1 : Başarıyla eklendi
 **                 0 : Bellek hatası
 **
 **  ---------------------------------------------------------------------------
 */
int kayit_ekle(CommandList *liste, CommandRecord *kayit) {
    // Kapasite doldu mu?
    if (liste->size >= liste->capacity) {
        int yeni_kapasite = liste->capacity * 2;
        CommandRecord *gecici = (CommandRecord *)realloc(
                liste->records, yeni_kapasite * sizeof(CommandRecord));
        if (!gecici) return 0; /* Bellek hatası */
        liste->records  = gecici;
        liste->capacity = yeni_kapasite;
    }
    liste->records[liste->size++] = *kayit;
    return 1;
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : komutu_ayristir (parse_command)
 **
 ** \brief
 **          Tek bir komut metnini (örn: "GCD 48 18") ayrıştırır ve
 **          CommandRecord yapısını komut adı ile parametrelerle doldurur.\n
 **          Component : Parser \n
 **
 ** \param [in]  char          *parca - Ayrıştırılacak komut metni
 ** \param [out] CommandRecord *kayit - Doldurulan komut kaydı
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */


static void komutu_ayristir(char *parca, CommandRecord *kayit) {
    memset(kayit, 0, sizeof(CommandRecord));

    // Komut adını oku
    char komut_adi[20] = {0};
    int  ofset         = 0;

    if (sscanf(parca, "%19s%n", komut_adi, &ofset) < 1) {
        snprintf(kayit->result, sizeof(kayit->result), "PARSE_ERROR");
        kayit->is_error = 1;
        return;
    }
    snprintf(kayit->command, sizeof(kayit->command), "%s", komut_adi);

    // Parametreleri oku
    char     *kalan = parca + ofset;
    long long deger;
    int       n;

    kayit->arg_count = 0;
    while (sscanf(kalan, " %lld%n", &deger, &n) == 1
           && kayit->arg_count < 4) {
        kayit->args[kayit->arg_count++] = deger;
        kalan += n;
    }
}

/*
 **  ---------------------------------------------------------------------------
 **  Name : parse_file
 **
 ** \brief
 **          Girdi dosyasını satır satır okur. Yorum satırlarını (#) ve boş
 **          satırları atlar. Noktalı virgülle ayrılmış komutları bölerek
 **          listeye ekler.\n
 **          Component : Parser \n
 **
 ** \param [in]  const char  *dosya_adi - Okunacak dosyanın yolu
 ** \param [out] CommandList *liste     - Ayrıştırılan komutların ekleneceği liste
 **
 ** \returns int    1 : Başarıyla tamamlandı
 **                 0 : Dosya açılamadı veya bellek hatası
 **
 **  ---------------------------------------------------------------------------
 */

int parse_file(const char *dosya_adi, CommandList *liste) {
    FILE *dosya = fopen(dosya_adi, "r");
    if (!dosya) {
        fprintf(stderr, "Hata: '%s' dosyasi acilamadi.\n", dosya_adi);
        return 0;
    }

    char satir[SATIR_BOYUTU];

    while (fgets(satir, sizeof(satir), dosya)) {
        char *temiz_satir = bosluklari_temizle(satir);

        // Boş satır veya yorum satırı ise atla
        if (temiz_satir[0] == '\0' || temiz_satir[0] == '#') continue;

        // Noktalı virgülle ayrılmış komutları böl
        char *kayit_ptr = NULL;
        char *parca     = strtok_r(temiz_satir, ";", &kayit_ptr);

        while (parca != NULL) {
            char *temiz_parca = bosluklari_temizle(parca);
            if (temiz_parca[0] != '\0') {
                CommandRecord kayit;
                komutu_ayristir(temiz_parca, &kayit);
                if (!kayit_ekle(liste, &kayit)) {
                    fprintf(stderr, "Hata: Komut listesi genisletilemedi.\n");
                    fclose(dosya);
                    return 0;
                }
            }
            parca = strtok_r(NULL, ";", &kayit_ptr);
        }
    }

    fclose(dosya);
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
 ** \param [in] CommandList *liste - Serbest bırakılacak liste işaretçisi
 **
 ** \returns void
 **
 **  ---------------------------------------------------------------------------
 */
void free_list(CommandList *liste) {
    if (!liste) return;
    free(liste->records);
    free(liste);
}
