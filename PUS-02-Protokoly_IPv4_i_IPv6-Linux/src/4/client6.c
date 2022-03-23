// Kompilacja: gcc client6.c -o client6
// Uruchomienie: ./client6 <adres IPv6 (IPv4-mapped)> <numer portu> <nazwa interfejsu>
// nazwa interfejsu: lo, adres interfejsu loopback: ::ffff:127.0.0.1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

int main(int argc, char **argv)
{   
    struct sockaddr_in6 remote_addr; // gniazdowa struktura adresowa dla IPv6
    socklen_t remote_addr_len; // rozmiar struktury adresowej w bajtach
    int sockfd; // deskryptor gniazda
    int retval; // wartosc zwracana przez funkcje
    int interface_id; // indeks interfejsu
    char buff[256]; // bufor wykorzystywany przez read()

    if (argc != 4)
    {
        fprintf(stderr, "Invocation: %s <IPv6_address)> <port_number> <interface_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(PF_INET6, SOCK_STREAM, 0); // utworzenie gniazda dla protokolu TCP
    if (sockfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&remote_addr, 0, sizeof(remote_addr)); // wyzerowanie struktury adresowej dla adresu zdalnego (serwera)
    remote_addr.sin6_family = AF_INET6; // domena komunikacyjna (rodzina protokolow)
    remote_addr.sin6_port = htons(atoi(argv[2])); // numer portu
    retval = inet_pton(AF_INET6, argv[1], &remote_addr.sin6_addr); // konwersja adresu IPv6 z postaci czytelnej dla czlowieka
    if (retval == 0)
    {
        perror("inet_pton(): invalid IPv6 address!");
        exit(EXIT_FAILURE);
    }
    else if (retval == -1)
    {
        perror("inet_pton() error: ");
        exit(EXIT_FAILURE);
    }
    
    interface_id = if_nametoindex(argv[3]); // zamiana nazwy interfejsu na indeks
    if (interface_id == 0)
    {
        perror("if_nametoindex() error: ");
        exit(EXIT_FAILURE);
    }
    remote_addr.sin6_scope_id = interface_id; // indeks interfejsu przez ktory ma nastąpić komunikacja

    remote_addr_len = sizeof(remote_addr);
    if (connect(sockfd, (const struct sockaddr*) &remote_addr, remote_addr_len) == -1) // polaczenie klienta z serwerem poprzez inicjacje trojfazowego polaczenia
    {
        perror("connect() error: ");
        exit(EXIT_FAILURE);
    }
    
    memset(buff, 0, strlen(buff));
    retval = read(sockfd, buff, sizeof(buff)); // klient odbiera wiadomość od serwera
    if (retval == -1)
    {
        perror("read() error: ");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Received server response: %s\n", buff); //wypisanie wiadomosci na stdout

    sleep(3);
    fprintf(stdout, "Closing socket (sending FIN to server)...\n"); 
    close(sockfd); // zamkniecie polaczenia TCP, klient konczy dzialanie
    sleep(3);

    return 0;
}
