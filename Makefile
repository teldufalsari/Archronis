CC = clang++

CFLAGS = -Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow
CFLAGS += -Wunreachable-code -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual
CFLAGS += -std=c++17 -g

all: rat

lzw.o: lzw.cpp lzw.hpp
	$(CC) $(CFLAGS) -c lzw.cpp

rat: rat.o
	$(CC) rat.o lzw.o -o rat.exe

rat.o: rat.cpp
	$(CC) $(CFLAGS) -c rat.cpp

clean:
	rm -rf *.o *.exe
