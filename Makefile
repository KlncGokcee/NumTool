# Makefile - NumTool projesi derleme otomasyonu
# Kullanım: terminalde sadece "make" yaz, "make clean" ile temizle.

<<<<<<< HEAD
#MakeFile, temizlik işlerinde sorumludur. Makefile'dan silersen, bir daha programını derleyemezsin!


=======
>>>>>>> cd7e91aecdf2aed3ceeb22ef033022f8ad57696e
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2
TARGET  = numtool
SRCS    = main.c parser.c math_utils.c
OBJS    = $(SRCS:.c=.o)

# Varsayılan hedef: çalıştırılabilir dosyayı üret
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "Derleme tamamlandi: ./$(TARGET) hazir."

# Her .c dosyasından .o dosyası üret
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Bağımlılıklar (header değişince ilgili .c yeniden derlenir)
main.o:       main.c parser.h math_utils.h
parser.o:     parser.c parser.h
math_utils.o: math_utils.c math_utils.h

# Derleme dosyalarını ve çalıştırılabiliri sil
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Temizlendi."

.PHONY: all clean
