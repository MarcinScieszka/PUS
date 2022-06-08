#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <time.h>
#include <errno.h>
#include <systemd/sd-daemon.h>

int main(int argc, char** argv) {

    /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
    int             listenfd, connfd;

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len;

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];

    /* Wiadomosc do wyslania: */
    char*           message = "Laboratorium PUS.";

    // Socket z SOCKET-ACTIVATABLE SERVICE zamiast tworzenia własnego
    if (sd_listen_fds(0) != 1) {
        fprintf(stderr, "No or too many file descriptors received.\n");
        exit(EXIT_FAILURE);
    }
    listenfd = SD_LISTEN_FDS_START + 0;

    fprintf(stdout, "Server is listening for incoming connection...\n");


    for (;;) {
        /* Funkcja pobiera polaczenie z kolejki polaczen oczekujacych na
         * zaakceptowanie i zwraca deskryptor dla gniazda polaczonego: */
        client_addr_len = sizeof(client_addr);
        connfd = accept(listenfd,
                        (struct sockaddr*)&client_addr, &client_addr_len);

        if (connfd == -1) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        fprintf(
            stdout, "TCP connection accepted from %s:%d\n",
            inet_ntop(AF_INET, &client_addr.sin_addr,
                      addr_buff, sizeof(addr_buff)),
            ntohs(client_addr.sin_port)
        );

        /* Wyslanie aktualnego czasu do klienta: */
        write(connfd, message, strlen(message));

        close(connfd);
    }

}
