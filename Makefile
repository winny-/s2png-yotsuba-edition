# Don't you dare edit this file. You can override variables at invocation:
# $ make CC=gcc CFLAGS=-02
CC = cc
CFLAGS = -g
INCS = -I/usr/local/include -I/usr/include
LIBS = -lgd -L/usr/local/lib -L/usr/lib -L/lib
PREFIX = /usr/local/bin

s2png: s2png.c
	$(CC) $(CFLAGS) $(LDFLAGS) s2png.c -o s2png $(INCS) $(LIBS)

install: s2png
	mkdir -p $(PREFIX)/bin
	install -m 755 s2png $(PREFIX)/bin

clean:
	rm -rf *.o s2png s2png.dSYM testfile* README.html

test:
	sh ./test.sh

.PHONY = clean install test
