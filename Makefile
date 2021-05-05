CC = clang++

CFLAGS = -Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow
CFLAGS += -Wunreachable-code -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual
CFLAGS += -std=c++17 -O3

all: archronis

byte_str.o: lib/byte_str.cpp lib/byte_str.h
	$(CC) $(CFLAGS) -c lib/byte_str.cpp

lzw.o: lzw.cpp lzw.hpp
	$(CC) $(CFLAGS) -c lzw.cpp

archronis: main.o lzw.o byte_str.o compressor.o
	$(CC) main.o lzw.o byte_str.o  compressor.o -o archronis

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

compressor.o: compressor.cpp compressor.hpp
	$(CC) $(CFLAGS) -c compressor.cpp
