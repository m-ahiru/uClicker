CC=gcc
CFLAGS=`pkg-config --cflags --libs gtk+-3.0` -Wall -Wextra

all: uClicker autoclicker_backend

uClicker: gui/main.c gui/ui.c gui/config.c
	$(CC) -o uClicker gui/main.c gui/ui.c gui/config.c $(CFLAGS)

autoclicker_backend: backend/autoclicker.c gui/config.c
	$(CC) -o autoclicker_backend backend/autoclicker.c gui/config.c -lpthread

clean:
	rm -f uClicker autoclicker_backend