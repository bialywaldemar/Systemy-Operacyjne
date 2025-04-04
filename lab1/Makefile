CC = gcc
CFLAGS = -Wall -std=c17 -O2 -g
TARGET = countdown
SRC = countdown.c
OBJ = countdown.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean




