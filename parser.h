/* ----------------------------------------------------------------------------------- */

/*
 * \file        parser.h
 *
 * \brief       Giriş verilerini işleyen ve ayrıştıran modülün arayüzü. Komut kayıt yapısı.
 *
 * \developer   Gokce Kılınc , Mustafa Sarı
 */
/* ----------------------------------------------------------------------------------- */


#ifndef PARSER_H
#define PARSER_H

/* --------------    Include Files   --------------------------------------------- */

#include <stddef.h>   // size_t

/* --------------    Defines    -------------------------------------------------- */

#define CMD_NAME_LEN    20    /* Komut adı için maksimum karakter sayısı */
#define CMD_MAX_ARGS     4    /* Bir komutun alabileceği maksimum parametre sayısı */
#define CMD_RESULT_LEN 100    /* Sonuç metni için maksimum karakter sayısı */

/* --------------    Types    ---------------------------------------------------- */

// Her bir komutu ve sonucunu tutan kayıt yapısı

typedef struct {
    char      command[CMD_NAME_LEN];   /* Komut adı: GCD, POW, PRIME vb. */
    long long args[CMD_MAX_ARGS];      /* Komut parametreleri             */
    int       arg_count;               /* Gerçek parametre sayısı         */
    char      result[CMD_RESULT_LEN];  /* Hesaplanan sonuç metni          */
    int       is_error;                /* Hata varsa 1, yoksa 0           */
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
 * \param  list  Serbest bırakılacak liste işaretçisi.
 */
void free_list(CommandList *list);

/*
 * \brief  Belirtilen dosyayı satır satır okur, komutları ayrıştırarak listeye ekler.
 * \param  filename  Okunacak dosyanın yolu.
 * \param  list      Ayrıştırılan komutların ekleneceği liste.
 * \return Başarıda 0; hata durumunda negatif değer.
 */
int parse_file(const char *filename, CommandList *list);

#endif /* PARSER_H */