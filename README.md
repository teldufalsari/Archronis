# Rat
Simple file archiver and compressor

This project consists of three independent programs for now

---

## compress
This is the analog of classic UNIX **compress**.
It also uses LZW algorithm for data compression, but its classic version with 12-bit code words published by Welch,
rather that UNIX compress version with variable length codes.
### usage
./compress file_name

*Compress* will create file named file_name.cmp. It won't remove the original file like **gzip** does.

---

## decompress
This is the analog of UNIX **uncompress**
It restores files compressed by *compress* using reverse LZW algorithm.
### usage:
./decomp file_name

*Decomp* will create file named file_name.dcp. It won't remove the origilal file like **gunzip** does

---

## rat
This is the analog of UNIX **tar**
It creates an archive from the specified list of files.
Unlike **tar**, it won't align data records inside the archive, creating as compact archives as possible.

As for now, file access mode and last modification time are preserved.

### usage
./rat pack file1 [file2] [file3] ... output_file

./rat unpack archive_file

---

**All programs support only regular files now.** No symlinks, fifos and other special files are supported.

