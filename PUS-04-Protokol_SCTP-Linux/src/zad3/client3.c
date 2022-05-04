#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/sctp.h>

#define BUFF_SIZE 256

int main(int argc, char** argv) {

    int                     sockfd;
    int                     retval, bytes;
    char                    *retptr;
    char                    buff[BUFF_SIZE];
    struct sockaddr_in      serveraddr;
    int                     res;

    struct sctp_initmsg     initmsg;
    struct sctp_sndrcvinfo sri;
    struct sctp_event_subscribe events;

    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IP ADDRESS> <PORT NUMBER>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&serveraddr,0,sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    retval = inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    //Set inimsg struct
    memset(&initmsg, 0, sizeof(struct sctp_initmsg));
    initmsg.sinit_num_ostreams = 3;
    initmsg.sinit_max_instreams = 3;
    initmsg.sinit_max_attempts = 5;
    initmsg.sinit_max_init_timeo=30000;

    events.sctp_data_io_event = 1;


    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    //Set options set in initmsg
    if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg))==-1){
	perror("socket option");
	exit(EXIT_FAILURE);
    }
    
    if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events))==-1){
        perror("socket option");
        exit(EXIT_FAILURE);
    }

    int stream = 0;
    for (;;) {

        memset(buff,0, 256);
        retptr = fgets(buff, BUFF_SIZE, stdin);
        if ((retptr == NULL) || (strcmp(buff, "\n") == 0)) {
            break;
        }

        printf("Stream number: %d\n", stream);

        bytes = sctp_sendmsg(sockfd, &buff, sizeof(buff), (struct sockaddr*)&serveraddr, sizeof(struct sockaddr_in), 0, 0, stream, 0, 0);
        if (bytes == -1) {
                perror("send()");
                exit(EXIT_FAILURE);
        }

        res = sctp_recvmsg(sockfd, (void*)buff, BUFF_SIZE, NULL, NULL, &sri, 0);
        if (res == -1) {
            perror("sctp_recvmsg()");
            exit(EXIT_FAILURE);
        }
        stream = sri.sinfo_stream;

        fprintf(stdout, "Echo: ");
        fflush(stdout);
        retval = write(STDOUT_FILENO, buff, bytes);

        printf("Stream number: %d\nSSN: %d\nID: %d\n", sri.sinfo_stream, sri.sinfo_ssn, sri.sinfo_assoc_id);

        
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}
