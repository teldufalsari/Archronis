CC = clang++

CFLAGS = -Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow
CFLAGS += -Wunreachable-code -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual
CFLAGS += -std=c++17 -g

all: rat

byte_str.o: lib/byte_str.cpp lib/byte_str.h
	$(CC) $(CFLAGS) -c lib/byte_str.cpp

lzw.o: lzw.cpp lzw.hpp
	$(CC) $(CFLAGS) -c lzw.cpp

rat: rat.o lzw.o byte_str.o
	$(CC) rat.o lzw.o byte_str.o -o rat.exe

rat.o: rat.cpp
	$(CC) $(CFLAGS) -c rat.cpp

clean:
	rm -rf *.o *.exe
