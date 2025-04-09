# Makefile.mac — för macOS med clang och SDL2 via Homebrew

SRCDIR = source
CC = clang

INCLUDEDIR = /opt/homebrew/include
LIBDIR = /opt/homebrew/lib

CFLAGS = -I$(INCLUDEDIR) -Iinclude -c -g
LDFLAGS = -L$(LIBDIR) -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lSDL2_mixer

TARGET = Game
OBJS = main.o game.o cleanup.o resources.o sdl_init.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

game.o: $(SRCDIR)/game.c
	$(CC) $(CFLAGS) $(SRCDIR)/game.c

cleanup.o: $(SRCDIR)/cleanup.c
	$(CC) $(CFLAGS) $(SRCDIR)/cleanup.c

resources.o: $(SRCDIR)/resources.c
	$(CC) $(CFLAGS) $(SRCDIR)/resources.c

sdl_init.o: $(SRCDIR)/sdl_init.c
	$(CC) $(CFLAGS) $(SRCDIR)/sdl_init.c

clean:
	rm -f *.o $(TARGET)
