CC = gcc
CFLAGS = -I. -O1 -g -Wall
LDFLAGS = -lX11 -lm -lpthread -lGL

MINGW32?=i686-w64-mingw32-

rawgd: rawgd.c state.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	
rawgd.exe: rawgd.c state.c
	$(MINGW32)$(CC) $(CFLAGS) -o $@ $^ -lgdi32 -liphlpapi -lopengl32

run: rawgd
	./rawgd

install:
	mv rawgd $(PREFIX)/bin/rawgd

clean:
	rm rawgd rawgd.exe
