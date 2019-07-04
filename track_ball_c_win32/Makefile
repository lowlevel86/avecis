CC = i686-w64-mingw32-gcc
OBJ = main.o avecis.o vector_ascii.o ucrot.o shapes.o controls.o
LIBS = -mwindows -lws2_32
BIN = prog.exe
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
