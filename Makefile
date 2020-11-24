CC = gcc
CFLAGS = -I. -O1 -g -Wall
LDFLAGS = -lX11 -lm -lpthread -lGL

rawgd: rawgd.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	
run: rawgd
	./rawgd

install:
	mv rawgd $(PREFIX)/bin/rawgd

clean:
	rm rawgd
