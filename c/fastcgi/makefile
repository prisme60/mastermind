#!/bin/make

CC = g++

#-march=pentium4 -mfpmath=sse -mmmx -msse -msse2 -msse3
#CFLAGS = -D__DEBUG__ -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-mem -fforce-addr -pipe
#CFLAGS = -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-mem -fforce-addr -fno-exceptions -pipe
CFLAGS = -g -DFASTCGI_MODE -D_REENTRANT -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-addr -fno-exceptions -pipe

#-fprofile-generate
#-fprofile-use -Wcoverage-mismatch


#CFLAGS = -D_REENTRANT -g -Wall -march=pentium4
#-lpthread

COMPILE = $(CC) $(CFLAGS) -c

default-target: all

all: mastermind

mastermind: combinaison.o mastergame.o mastermind.o mastersolver.o nodecombinaison.o
	$(CC) combinaison.o mastergame.o mastermind.o mastersolver.o nodecombinaison.o ../fcgi-2.4.0/libfcgi/.libs/libfcgi.a ../fcgi-2.4.0/libfcgi/.libs/libfcgi++.a -o mastermind.exe
	strip -s mastermind.exe -o mastermind-stripped.exe

nodecombinaison.o: src/nodecombinaison.cpp
	$(COMPILE) src/nodecombinaison.cpp

combinaison.o: src/combinaison.cpp
	$(COMPILE) src/combinaison.cpp

mastergame.o: src/mastergame.cpp
	$(COMPILE) -I../fcgi-2.4.0/include src/mastergame.cpp

mastersolver.o: src/mastersolver.cpp
	$(COMPILE) src/mastersolver.cpp

mastermind.o: src/mastermind.cpp
	$(COMPILE) src/mastermind.cpp

clean:
	rm -rf *.o mastermind.exe
