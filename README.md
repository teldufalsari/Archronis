# Rat
Simple file archiver and compressor.

## Usage
./rat pack file1 [file2] [file3] ... output_file

./rat unpack archive_file

## Description
Rat is an archiving program that stores multiple files compressed in a single compressed file.
It uses classic LZW algoritm published by Terry Welch in 1984, which encodes sequences of 8-bit data as fixed-length 12-bit codes

## Features
* only regular files are supported: no symlinks, fifos and other special files
* file access mode and last modification time are preserved
* CRC-32 checksum of each compressed block is calculated
* text files, source code and binaries can be compressed with excellent ratio
* alredy compressed data is usually compressed with poor ratio or even inflates
