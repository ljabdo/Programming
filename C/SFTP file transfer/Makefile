# udpCode makefile
# written by Hugh Smith - Feb 2021

CC = gcc
CFLAGS = -g -Wall


SRC = networks.c  gethostbyname.c safeUtil.c pdu.c window.c pollLib.c srej.c
OBJS = networks.o gethostbyname.o safeUtil.o pdu.o window.o pollLib.o srej.o

#uncomment next two lines if your using sendtoErr() library
LIBS += libcpe464.2.21.a -lstdc++ -ldl
#CFLAGS += -D__LIBCPE464_

all:  rcopy server

rcopy: rcopy.c $(OBJS) 
	$(CC) $(CFLAGS) -o rcopy rcopy.c $(OBJS) $(LIBS)

server: server.c $(OBJS) 
	$(CC) $(CFLAGS) -o server server.c  $(OBJS) $(LIBS)

#test1: testWindow.c $(OBJS) 
#	$(CC) $(CFLAGS) -o test1 testWindow.c  $(OBJS) $(LIBS)

%.o: %.c *.h 
	gcc -c $(CFLAGS) $< -o $@ 

cleano:
	rm -f *.o

clean:
	rm -f server rcopy *.o

