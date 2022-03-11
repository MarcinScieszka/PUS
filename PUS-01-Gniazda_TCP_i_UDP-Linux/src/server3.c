/*
 * Kompilacja:          $ gcc server3.c libpalindrome.c -o server3
 * Uruchamianie:        $ ./server3 <numer portu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "libpalindrome.h"

int main(int argc, char **argv)
{
    int sockfd; /* Deskryptor gniazda. */
    int retval; /* Wartosc zwracana przez funkcje. */

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t client_addr_len, server_addr_len;

    /* Bufor wykorzystywany przez recvfrom() i sendto(): */
    char buff[256];

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char addr_buff[256];

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu UDP: */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej serwera: */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    server_addr.sin_family = AF_INET;
    /* Adres nieokreslony (ang. wildcard address): */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Numer portu: */
    server_addr.sin_port = htons(atoi(argv[1]));
    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_addr_len = sizeof(server_addr);

    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(sockfd, (struct sockaddr *)&server_addr, server_addr_len) == -1)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    client_addr_len = sizeof(client_addr);

    while (1)
    {
        fprintf(stdout, "\nServer is listening for incoming connection...\n");

        /* Oczekiwanie na dane od klienta: */
        retval = recvfrom(
            sockfd,
            buff, sizeof(buff),
            0,
            (struct sockaddr *)&client_addr, &client_addr_len);

        if (retval == -1)
        {
            perror("recvfrom()");
            exit(EXIT_FAILURE);
        }

        // Diagram UDP bez danych - serwer kończy działanie
        if (buff[0] == '\n')
        {
            break;
        }

        // serwer wypisuje adres i numer portu klienta
        fprintf(stdout, "UDP datagram received. Client address: %s, port number: %d\n",
                inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
                ntohs(client_addr.sin_port));

        // serwer sprawdza, czy otrzymany ciąg znaków można traktować jako liczbę całkowitą
        int is_digit = 1;
        for (int i = 0; i < strlen(buff); i++)
        {
            if (isspace(buff[i]) == 0)
            {
                if (isdigit(buff[i]) == 0)
                {
                    is_digit = 0;
                    break;
                }
            }
        }

        // Jeżeli ciąg znaków można traktować jako liczbę całkowitą, to serwer sprawdza czy liczba jest palindromem
        if (is_digit)
        {
            buff[strlen(buff)] = '\0';

            if (is_palindrome(buff, strlen(buff)) == 1)
            {
                strcpy(buff, "Otrzymane dane są palindromem liczbowym.");
            }
            else
            {
                strcpy(buff, "Otrzymane dane nie są palindromem liczbowym.");
            }
        }
        else
        {
            strcpy(buff, "Otrzymany ciąg znaków nie można traktować jako liczbę całkowitą.");
        }

        //  serwer przesyła odpowiedź do klienta
        retval = sendto(
            sockfd,
            buff, retval,
            0,
            (struct sockaddr *)&client_addr, client_addr_len);
        if (retval == -1)
        {
            perror("sendto()");
            exit(EXIT_FAILURE);
        }

        buff[0] = '\0';
    }

    close(sockfd);

    exit(EXIT_SUCCESS);
}
