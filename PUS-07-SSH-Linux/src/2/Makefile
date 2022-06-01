all: build_all

build_all:
	gcc -c libcommon.c -o libcommon.o
	gcc exec.c -lssh2 libcommon.o -o exec
