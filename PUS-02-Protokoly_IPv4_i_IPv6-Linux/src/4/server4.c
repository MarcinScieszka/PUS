// Kompilacja: gcc server4.c -o server4
// Uruchomienie: ./server4 <numer portu>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char **argv) 
{
    struct sockaddr_in server_addr; // gniazdowa struktura adresowa serwera
    struct sockaddr_in client_addr; // gniazdowa struktura adresowa klienta
    socklen_t server_addr_len; // rozmiar struktury adresowej serwera w bajtach
    socklen_t client_addr_len; // rozmiar struktury adresowej klienta
    int listenfd; // deskryptor gniazda nasluchujacego
    int connfd; // deskryptor gniazda polaczonego 
    int retval; // wartosc zwracana przez funkcje
    char addr_buff[256]; // bufor dla adresu IP klienta w postaci kropkowo-dziesietnej
    char buff[256]; // bufor wykorzystywany przez write()

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <port_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    listenfd = socket(PF_INET, SOCK_STREAM, 0); // utworzenie gniazda dla protokolu TCP
    if (listenfd == -1) 
    {
        perror("socket() error: ");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr)); // wyzerowanie struktury adresowej serwera
    server_addr.sin_family = AF_INET; // domena komunikacyjna (rodzina protokolow)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // obieranie danych z dowolnego interfejsu sieciowego
    server_addr.sin_port = htons(atoi(argv[1])); // numer portu
    server_addr_len = sizeof(server_addr); // rozmiar struktury adresowej serwera w bajtach

    if (bind(listenfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) // przypisanie gniazdu "nazwy" (adresu IP i numeru portu)
    {
        perror("bind() error: ");
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 2) == -1) // przeksztalcenie gniazda w gniazdo nasluchujace
    {
        perror("listen() error: ");
        exit(EXIT_FAILURE);
    }

    sprintf(buff, "Laboratorium PUS\n");

    while (1)
    {
        fprintf(stdout, "Server is listening for incoming connection...\n");

        client_addr_len = sizeof(client_addr);
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len); // pobranie polaczenia z kolejki polaczen oczekujacych na zaakceptowanie
        if (connfd == -1) 
        {
            perror("accept() error: ");
            exit(EXIT_FAILURE);
        }

        // serwer wypisuje adres i numer portu klienta
        fprintf(
            stdout, "TCP connection accepted from %s:%d\n",
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
            ntohs(client_addr.sin_port)
        );

        fprintf(stdout, "Sending message to client...\n");
        retval = write(connfd, buff, strlen(buff)); // przesłanie do klienta wiadomości

        sleep(5);

        fprintf(stdout, "Closing connected socket (sending FIN to client)...\n");
        close(connfd); // serwer kończy obsługę bieżącego połączenia TCP
    }

    fprintf(stdout, "Closing listening socket and terminating server...\n");
    close(listenfd);

    return 0;
}
