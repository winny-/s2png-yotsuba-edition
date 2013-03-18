# Don't you dare edit this file. You can override variables at invocation:
# $ make CC=gcc CFLAGS=-02
CC = cc
CFLAGS = -g
INCS = `libpng-config --cflags` -I/usr/local/include -I/usr/include
LIBS = `libpng-config --libs` -lgd -lm -L/usr/local/lib -L/usr/lib -L/lib
PREFIX = /usr/local/bin

s2png: s2png.c
	$(CC) $(CFLAGS) s2png.c -o s2png $(INCS) $(LIBS)

install: s2png
	mkdir -p $(PREFIX)/bin
	install -m 755 s2png $(PREFIX)/bin

clean:
	rm -rf *.o s2png s2png.dSYM testfile* README.html

test:
	sh ./test.sh

.PHONY = clean install test
