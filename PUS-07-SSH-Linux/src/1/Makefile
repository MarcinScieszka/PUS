all: build_all

build_all:
	gcc -c libcommon.c -o libcommon.o
	gcc authlist.c -lssh2 libcommon.o -o authlist
