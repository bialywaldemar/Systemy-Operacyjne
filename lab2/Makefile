CC = gcc
CFLAGS = -Wall -std=c11
OBJ = collatz.o

all: libcollatz.a libcollatz.so main

libcollatz.a: $(OBJ)
	ar rcs libcollatz.a $(OBJ)

libcollatz.so: $(OBJ)
	$(CC) -shared -o libcollatz.so $(OBJ)

collatz.o: collatz.c collatz.h
	$(CC) $(CFLAGS) -fPIC -c collatz.c

main: main.c collatz.h libcollatz.a
	$(CC) $(CFLAGS) main.c -L. -lcollatz -Wl,-rpath,. -o main

clean:
	rm -f *.o *.a *.so main

.PHONY: all clean
