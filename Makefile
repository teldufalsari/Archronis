CXX = clang++

CFLAGS = -Wall -Wextra -Wnarrowing -Wfloat-equal -Wundef -Wshadow
CFLAGS += -Wunreachable-code -Wpointer-arith -Wcast-align
CFLAGS += -Wwrite-strings -Wswitch-default -Wswitch-enum -Winit-self -Wcast-qual
CFLAGS += -std=c++17 -O3

all: archronis

byte_str.o: lib/byte_str.cpp lib/byte_str.h
	$(CXX) $(CFLAGS) -c lib/byte_str.cpp

lzw.o: lzw.cpp lzw.hpp
	$(CXX) $(CFLAGS) -c lzw.cpp

archronis: main.o lzw.o byte_str.o compressor.o
	$(CXX) main.o lzw.o byte_str.o  compressor.o -o archronis

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

compressor.o: compressor.cpp compressor.hpp
	$(CXX) $(CFLAGS) -c compressor.cpp
