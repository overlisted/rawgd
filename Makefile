CC = gcc
CFLAGS = -I. -Irdui -O1 -g -Wall
LDFLAGS = -lX11 -lm -lpthread -lGL

rawgd: rawgd.c state.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	
run: rawgd
	./rawgd

install:
	mv rawgd $(PREFIX)/bin/rawgd

clean:
	rm rawgd
