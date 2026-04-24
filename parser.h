/*
 * parser.h
 * Komut kayıt yapısı ve parser fonksiyonlarının başlık dosyası.
 */

#ifndef PARSER_H
#define PARSER_H

/* Her bir komutu ve sonucunu tutan kayıt yapısı */
typedef struct {
    char      command[20];   /* Komut adı: GCD, POW, PRIME vb. */
    long long args[4];       /* Komut parametreleri (max 4 adet) */
    int       arg_count;     /* Gerçek parametre sayısı          */
    char      result[100];   /* Hesaplanan sonuç metni           */
    int       is_error;      /* Hata varsa 1, yoksa 0            */
} CommandRecord;

/* Dinamik dizi yapısı: komutları esnek boyutlu bellekte tutar */
typedef struct {
    CommandRecord *records;  /* Dinamik dizi işaretçisi */
    int            size;     /* Şu anki eleman sayısı   */
    int            capacity; /* Ayrılmış kapasite       */
} CommandList;

/* Fonksiyon bildirimleri */
CommandList *create_list(void);
void         free_list(CommandList *list);
int          parse_file(const char *filename, CommandList *list);

#endif
