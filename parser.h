/* ----------------------------------------------------------------------------------- */

/*
 * \file        parser.h
 *
 * \brief       Giriş verilerini işleyen ve ayrıştıran modülün arayüzü. Komut kayıt yapısı.
 *
 * \developer   Gokce Kılınc  No : 430634 , Mustafa Sari  No : 425511
 */
/* ----------------------------------------------------------------------------------- */


#ifndef PARSER_H
#define PARSER_H

/* --------------    Include Files   --------------------------------------------- */

#include <stddef.h>   // size_t

/* --------------    Defines    -------------------------------------------------- */

#define KOMUT_ADI_UZUNLUGU    20    /* Komut adı için maksimum karakter sayısı */
#define KOMUT_MAKS_PARAMETRE   4    /* Bir komutun alabileceği maksimum parametre sayısı */
#define KOMUT_SONUC_UZUNLUGU 100    /* Sonuç metni için maksimum karakter sayısı */

/* Eski isimler — diğer dosyalar hâlâ bunları kullanıyorsa derleme kırılmasın diye takma ad */
#define CMD_NAME_LEN    KOMUT_ADI_UZUNLUGU
#define CMD_MAX_ARGS    KOMUT_MAKS_PARAMETRE
#define CMD_RESULT_LEN  KOMUT_SONUC_UZUNLUGU

/* --------------    Types    ---------------------------------------------------- */

// Her bir komutu ve sonucunu tutan kayıt yapısı

typedef struct {
    char      command[KOMUT_ADI_UZUNLUGU];    /* Komut adı: GCD, POW, PRIME vb. */
    long long args[KOMUT_MAKS_PARAMETRE];     /* Komut parametreleri             */
    int       arg_count;                      /* Gerçek parametre sayısı         */
    char      result[KOMUT_SONUC_UZUNLUGU];   /* Hesaplanan sonuç metni          */
    int       is_error;                       /* Hata varsa 1, yoksa 0           */
} CommandRecord;

// Dinamik dizi yapısı: komutları esnek boyutlu bellekte tutar

typedef struct {
    CommandRecord *records;   /* Dinamik dizi işaretçisi */
    int            size;      /* Şu anki eleman sayısı   */
    int            capacity;  /* Ayrılmış kapasite       */
} CommandList;



/* --------------    Public Functions   ------------------------------------------ */

/*
 * \brief  Yeni ve boş bir CommandList oluşturur, heap'te tahsis eder.
 * \return Başarıda geçerli işaretçi; bellek hatasında NULL.
 */
CommandList *create_list(void);

/*
 * \brief  CommandList ve içindeki kayıtlar için ayrılmış belleği serbest bırakır.
 * \param  liste  Serbest bırakılacak liste işaretçisi.
 */
void free_list(CommandList *liste);

/*
 * \brief  Belirtilen dosyayı satır satır okur, komutları ayrıştırarak listeye ekler.
 * \param  dosya_adi  Okunacak dosyanın yolu.
 * \param  liste      Ayrıştırılan komutların ekleneceği liste.
 * \return Başarıda 1; hata durumunda 0.
 */
int parse_file(const char *dosya_adi, CommandList *liste);

#endif /* PARSER_H */
