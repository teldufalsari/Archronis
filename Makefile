CC = clang++

CFLAGS = -Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow
CFLAGS += -Wunreachable-code -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual
CFLAGS += -std=c++17

all: compress decomp rat

compress: lzw.o compress.o
	$(CC) lzw.o compress.o -o compress.exe

decomp: lzw.o decomp.o
	$(CC) lzw.o decomp.o -o decomp.exe

lzw.o: lzw.cpp lzw.hpp
	$(CC) $(CFLAGS) -c lzw.cpp

compress.o: compress.cpp
	$(CC) $(CFLAGS) -c compress.cpp

decomp.o: decomp.cpp
	$(CC) $(CFLAGS) -c decomp.cpp

rat: rat.o
	$(CC) rat.o lzw.o -o rat.exe

rat.o: rat.cpp
	$(CC) $(CFLAGS) -c rat.cpp

clean:
	rm -rf *.o *.exe
