CC=g++
CFLAGS=-Wall -pedantic -O3

qseq2fq: qseq2fq.cpp
	$(CC) $(CFLAGS) -o qseq2fq qseq2fq.cpp
