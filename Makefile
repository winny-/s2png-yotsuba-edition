# Don't you dare edit this file. You can override variables at invocation:
# $ make CC=gcc CFLAGS=-02
CC = cc
CFLAGS = -g -Wall -Wextra -pedantic -std=c99
INCS = -I/usr/local/include -I/usr/include
LIBS = -lgd -L/usr/local/lib -L/usr/lib -L/lib
PREFIX = /usr/local/bin

s2png: s2png.c
	$(CC) $(CFLAGS) $(LDFLAGS) s2png.c -o s2png $(INCS) $(LIBS)

install: s2png
	mkdir -p $(PREFIX)/bin
	install -m 755 s2png $(PREFIX)/bin

clean:
	rm -f *.o s2png testfile* README.html
	rm -rf s2png.dSYM

test: s2png
	sh ./test.sh

.PHONY = clean install test
