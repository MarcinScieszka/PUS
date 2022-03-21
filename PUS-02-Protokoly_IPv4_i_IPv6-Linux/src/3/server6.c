/*
 * Kompilacja:          $ gcc server6_1.c -o server6_1
 * Uruchamianie:        $ ./server6_1 <numer portu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#define MAX_CLINETS 10

int main(int argc, char **argv)
{
  int sockfd; /* Deskryptor gniazda. */
  int retval; /* Wartosc zwracana przez funkcje. */
  int new_sd;

  /* Gniazdowe struktury adresowe (dla klienta i serwera): */
  struct sockaddr_in6 client_addr, server_addr;

  struct sockaddr_in6 addr6; /* Struktura adresowa dla IPv6. */
  char buff6[INET6_ADDRSTRLEN];

  /* Rozmiar struktur w bajtach: */
  socklen_t client_addr_len, server_addr_len;

  if (argc != 2)
  {
    fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Utworzenie gniazda */
  sockfd = socket(AF_INET6, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    perror("socket() error");
    exit(EXIT_FAILURE);
  }

  /* Wyzerowanie struktury adresowej serwera: */
  memset(&server_addr, 0, sizeof(server_addr));
  /* Domena komunikacyjna (rodzina protokolow): */
  server_addr.sin6_family = AF_INET6;
  /* Adres nieokreslony (ang. wildcard address): */
  server_addr.sin6_addr = in6addr_any;
  /* Numer portu: */
  server_addr.sin6_port = htons(atoi(argv[1]));
  /* Rozmiar struktury adresowej serwera w bajtach: */
  server_addr_len = sizeof(server_addr);

  /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
  if (bind(sockfd, (struct sockaddr *)&server_addr, server_addr_len) < 0)
  {
    perror("bind() error");
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 10) < 0)
  {
    perror("listen() error");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    fprintf(stdout, "Server is listening for incoming connection...\n");
    client_addr_len = sizeof(client_addr);
    new_sd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (new_sd == -1)
    {
      perror("accept() error:");
      exit(EXIT_FAILURE);
    }

    fprintf(
        stdout, "inet_ntop(): %s\n",
        inet_ntop(AF_INET6, (struct sockaddr *)&(client_addr.sin6_addr), buff6, INET6_ADDRSTRLEN));

    if (IN6_IS_ADDR_V4MAPPED(&client_addr.sin6_addr))
      fprintf(stdout, "Client is ipv4\n");
    else
      fprintf(stdout, "Client is ipv6\n");

    retval = send(new_sd, "Laboratorium PUS", strlen("Laboratorium PUS"), 0);
    if (retval == -1)
    {
      perror("send()");
      exit(EXIT_FAILURE);
    }
  }
  close(sockfd);

  exit(EXIT_SUCCESS);
}