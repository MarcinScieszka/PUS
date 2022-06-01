all: build_all

build_all:
	gcc -c libcommon.c -o libcommon.o
	gcc sftp.c -lssh2 libcommon.o -o sftp
