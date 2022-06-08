PROJECT = 10
SYSTEM = linux
SRCDIR = src
BINDIR = bin
CC = gcc
CFLAGS = -O2 -pedantic -Wall

all: tcp_client tcp_server tcp_server_sock

files:
	@cp $(SRCDIR)/files/* $(BINDIR)/

clean:
	@rm -rf $(BINDIR)/*

distrib: clean
	tar -cvf $(SYSTEM)-$(PROJECT).tar $(SRCDIR)/* Makefile

tcp_client: tcp_client.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

tcp_client.o: $(SRCDIR)/tcp_client.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

tcp_server: tcp_server.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@

tcp_server.o: $(SRCDIR)/tcp_server.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o

tcp_server_sock: tcp_server_sock.o
	$(CC) $(CFLAGS) $(BINDIR)/$@.o -o $(BINDIR)/$@ -lsystemd

tcp_server_sock.o: $(SRCDIR)/tcp_server_sock.c
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(basename $@).c -o $(BINDIR)/$(basename $@).o
