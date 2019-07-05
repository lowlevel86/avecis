CC = gcc
OBJ = main.o drawGraphics.o line.o statusBar.o windowIcon.o keyCodes.o playSnd.o avecisHandler.o sendReceive.o server.o cmdLineArgs.o
LIBS = -lX11 -lm -lasound -pthread
BIN = avecis
CFLAGS = -Wall
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)
