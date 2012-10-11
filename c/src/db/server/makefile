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

INCLUDE = -I../ -I../../
COMPILE = $(CC) $(CFLAGS) $(INCLUDE) -c

default-target: all

all: node-server-udp

node-server-udp: command_mgr.o nodeFactory.o server-udp.o
	$(CC) $(CFLAGS) command_mgr.o nodeFactory.o server-udp.o -o node-server-udp.exe
	strip -s node-server-udp.exe -o node-server-udp-stripped.exe

nodeFactory.o: ../nodeFactory.cpp
	$(COMPILE) ../nodeFactory.cpp

command_mgr.o: ../command_mgr.cpp
	$(COMPILE) ../command_mgr.cpp
	
server-udp.o: server-udp.cpp
	$(COMPILE) server-udp.cpp

clean:
	rm -rf *.o node-server-udp.exe
