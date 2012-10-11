#!/bin/make

CC = g++

#-march=pentium4 -mfpmath=sse -mmmx -msse -msse2 -msse3
#CFLAGS = -D__DEBUG__ -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-mem -fforce-addr -pipe
#CFLAGS = -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-mem -fforce-addr -fno-exceptions -pipe

CFLAGS = -Wall -g -O3 -march=pentium4 -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-addr -pipe


#-------- Profiling --------
#CFLAGS = -D_REENTRANT -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fprofile-generate -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-addr -fno-exceptions -pipe
#CFLAGS = -D_REENTRANT -DNO_RESULT_OUTPUT -Wall -O3 -march=pentium4 -fprofile-use -Wcoverage-mismatch -fomit-frame-pointer -foptimize-register-move -ffast-math -fforce-addr -fno-exceptions -pipe
#-------- --------- --------

#CFLAGS = -D_REENTRANT -g -Wall -march=pentium4
#-lpthread

INCLUDE = -I../../
COMPILE = $(CC) $(CFLAGS) $(INCLUDE) -c

default-target: all

all: node-client-udp

node-client-udp: node-client-udp.o
	$(CC) $(CFLAGS) node-client-udp.o -o node-client-udp.exe
	strip -s node-client-udp.exe -o node-client-udp-stripped.exe

client-udp.o: node-client-udp.cpp
	$(COMPILE) node-client-udp.cpp

clean:
	rm -rf *.o node-client-udp.exe
