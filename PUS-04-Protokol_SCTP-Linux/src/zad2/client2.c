// $ ./client2 <IP address> <port number>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFF_SIZE 256

int main(int argc, char **argv)
{
    int sockfd;
    int retval, bytes_received, len;
    char *retptr;
    struct addrinfo hints, *result;
    struct sctp_initmsg initmsg;
    struct sctp_sndrcvinfo sinfo;
    struct sctp_status status;
    char buff[BUFF_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "Invocation: %s <IP ADDRESS> <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    retval = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (retval != 0)
    {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(retval));
        exit(EXIT_FAILURE);
    }

    if (result == NULL)
    {
        fprintf(stderr, "Could not connect!\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(result->ai_family, result->ai_socktype, IPPROTO_SCTP);
    if (sockfd == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = 2;
    initmsg.sinit_max_instreams = 2;
    initmsg.sinit_max_attempts = 5;
    retval = setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
    if (retval < 0)
    {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, result->ai_addr, result->ai_addrlen) == -1)
    {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result); // frees the memory that was allocated for 'result'
    len = sizeof(status);
    retval = getsockopt(sockfd, IPPROTO_SCTP, SCTP_STATUS, &status, &len);
    if (retval < 0)
    {
        perror("getsockopt() error");
    }
    printf("assoc_id: %d\n", status.sstat_assoc_id);
    printf("state: %d\n", status.sstat_state);
    printf("instrms: %d\n", status.sstat_instrms);
    printf("outstrms: %d\n", status.sstat_outstrms);

    for (int i = 0; i < 2; i++)
    {
        memset(buff, 0, sizeof(buff));
        bzero(&sinfo, sizeof(sinfo));
        bytes_received = sctp_recvmsg(sockfd, buff, BUFF_SIZE, NULL, 0, &sinfo, 0);   
        if (bytes_received < 0)
        {
            perror("sctp_recvmsg() error");
            exit(EXIT_FAILURE);
        }

        printf("read %d bytes on channel %hd\n", bytes_received, sinfo.sinfo_stream);
        printf("message received: %s\n", buff);
        // printf("sinfo flags: %d\n", sinfo.sinfo_flags);
    }
    
    close(sockfd);
    exit(EXIT_SUCCESS);
}
