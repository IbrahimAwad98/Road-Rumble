# Inställningar
SRCDIR = source
CC = gcc

# Justera till rätt plats om din SDL2 ligger någon annanstans
INCLUDEDIR = C:/msys64/mingw64/include/SDL2
LIBDIR = C:/msys64/mingw64/lib

# Flaggar
CFLAGS = -I$(INCLUDEDIR) -c
LDFLAGS = -L$(LIBDIR) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows

# Mål
TARGET = Game
OBJS = main.o

# Byggprogram
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET).exe $(LDFLAGS)

# Objektfil
main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

# Rensa
clean:
	del *.exe *.o
