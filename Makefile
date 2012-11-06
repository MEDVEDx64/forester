OBJS=gui.o input.o main.o tiles.o utils.o workspace.o
LDFLAGS=-pthread
CFLAGS=-s -O2
LIBS=-lSDL -lSDLmain -lSDL_image -lSDL_gfx -lSDL_ttf -ldl -lm

forester: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $(OBJS)
