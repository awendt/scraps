CC=gcc
CCMALLOC=gcc-4.0
AR = ar
ARFLAGS = ru
RANLIB = ranlib
#CFLAGS= -Wall -g
CFLAGS=
SRCS= blowfish.c helpers.c dh.c bob.c alice.c genprime.c

all:: bob alice

blowfish.o: blowfish.c blowfish.h blowinit.h
	$(CC) $(CFLAGS) -o blowfish.o -c blowfish.c

helpers.o: helpers.c helpers.h
	$(CC) $(CFLAGS) -o helpers.o -c helpers.c

dh.o: dh.c dh.h
	$(CC) $(CFLAGS) -o dh.o -c dh.c

bob: bob.c dh.o genprime.o helpers.o blowfish.o
	$(CC) $(CFLAGS) -o bob bob.c dh.o genprime.o helpers.o blowfish.o

alice: alice.c dh.o genprime.o helpers.o blowfish.o
	$(CC) $(CFLAGS) -o alice alice.c dh.o genprime.o helpers.o blowfish.o

genprime.o: genprime.c genprime.h
	$(CC) $(CFLAGS) -o genprime.o -c genprime.c

clean:
	/bin/rm -f blowfish dh bob alice *.o core *~ #*
