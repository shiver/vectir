APPNAME = vectir
CC = gcc
COMPILERFLAGS = -Wall -g
CFLAGS = $(COMPILERFLAGS) #$(INCLUDE)
SRCDIR = src/
OBJDIR = obj/
LIBS = -lSDLmain -lSDL -lGL -lSDL_mixer -lSDL_ttf -lSDL_image

$(APPNAME): common.o score.o config.o ui.o font.o particle.o graphics.o tetromino.o grid.o input.o game.o main.o
	cd $(OBJDIR); $(CC) $(CFLAGS) -o $@ $? $(LIBS)

score.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)util/score.c
  
config.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)util/config.c
  
ui.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)ui/ui.c

particle.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)graphics/particle.c

font.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)graphics/font.c

input.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)input/input.c

tetromino.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)game/tetromino.c

grid.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)game/grid.c

game.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)game/game.c

graphics.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)graphics/graphics.c

common.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)util/common.c

main.o:
	$(CC) $(CFLAGS) -c -o $(OBJDIR)$@ $(SRCDIR)main.c

clean:
	rm -f obj/$(APPNAME).*
	rm -f obj/*.o
	rm -f obj/stdout.txt
	rm -f obj/stderr.txt

