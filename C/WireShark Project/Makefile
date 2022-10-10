# Example makefile for CPE464 program 1
#
# 

CC = gcc
CFLAGS = -g -Wall -pedantic
#CFLAGS = -g

all:  trace

trace: trace.c
	$(CC) $(CFLAGS) -o trace trace.c print.c checksum.c -lpcap 

clean:
	rm -f trace
