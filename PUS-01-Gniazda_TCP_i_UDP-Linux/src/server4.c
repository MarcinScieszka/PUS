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
    int i, on = 1; 
    int desc_ready;
    int sockfd; /* Deskryptor gniazda. */
    int retval; /* Wartosc zwracana przez funkcje. */
    int new_sd;

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    // socklen_t client_addr_len, server_addr_len;
    int server_addr_len;

    /* Bufor wykorzystywany przez recv() i send(): */
    char buff[256];

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char addr_buff[256];

    if (argc != 2)
    {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }


    retval = ioctl(sockfd, FIONBIO, (char *)&on);

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
    if (bind(sockfd, (struct sockaddr *)&server_addr, server_addr_len) < 0)
    {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, MAX_CLINETS) < 0)
    {
        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(sockfd, &current_sockets);
    
    while (1)
    {
        fprintf(stdout, "Server is listening for incoming connection...\n");

        ready_sockets = current_sockets;
        
        retval = select(MAX_CLINETS + 1, &ready_sockets, NULL, NULL, NULL);
        if(retval < 0)
        {
            perror("select() error:");
            exit(EXIT_FAILURE);
        }

        desc_ready = retval;
        for (i = 0; i < MAX_CLINETS && desc_ready > 0; i++) 
        {
            // Istnieją deskryptory do odczytu

            if (FD_ISSET(i, &ready_sockets)) 
            {
                // Znaleziono deskryptor gotowy do odczytu
                desc_ready -= 1 ;

                if (i == sockfd) 
                {
                // aktywność związana z gniazdem serwera
                    do
                    {
                        // przyjęcie połączeń do czasu rzucenia EWOULDBLOCK 
                        int cl_addr_len;
                        cl_addr_len = sizeof(client_addr);
                        new_sd = accept(sockfd, (struct sockaddr *)&client_addr, &cl_addr_len);
                        if (new_sd < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  accept() failed\n");
                                exit(EXIT_FAILURE);   
                            }
                            break;
                            // perror("accept() error");
                            // exit(EXIT_FAILURE);
                        }
                        FD_SET(new_sd, &current_sockets);
                        printf("New connection\n");
                    }
                    while (new_sd != -1);
                }
                else 
                {
                    retval = ioctl(sockfd, FIONREAD, (char *)&on);
                    printf("Descriptor %d is readable\n", i);
                    // do
                    // {
                        // TODO: debug sending message to all clients
                        
                        //odebranie wiadomości od klientow
                        memset(&(buff[0]), 0, sizeof(buff)); // clear buffor
                        retval = recv(i, buff, sizeof(buff), 0); 
                        buff[strcspn(buff, "\n")] = '\0'; // remove endline from message
                        printf("received %s\n", buff);
                        if(retval < 0)
                        {
                            perror("recv() error:");
                            exit(EXIT_FAILURE);
                        }
                        //poloczenie zostalo zamkniete
                        if (retval == 0)
                        {
                            close(i);
                            FD_CLR(i, &current_sockets);
                            printf("Connection closed\n");
                            break;
                        }
                        int j;
                        for (j = 0; j < FD_ISSET(j, &current_sockets) && j != sockfd; j++) 
                        {
                            
                            //odeslanie wiadomosci do klienta
                            retval = send(i, buff, sizeof(buff), 0);
                            if (retval < 0)
                            {
                                close(i);
                                FD_CLR(i, &current_sockets);
                                perror("send() error");
                                break;
                            }
                        }
                    // } while (1);
                }
            }
        }
    }
    
    for (i = 0; i <= MAX_CLINETS; ++i)
    {
        if (FD_ISSET(i, &current_sockets))
            close(i);
    }

    exit(EXIT_SUCCESS);
}
