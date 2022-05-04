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

    int                     listenfd;
    int                     bytes;
    struct sockaddr_in      servaddr;
    char                    buffer[BUFF_SIZE];
    int                     argopt;
    int                     res;

    struct sockaddr_in      clientaddr;

    struct sctp_sndrcvinfo sri;

    struct sctp_initmsg     initmsg;
    struct sctp_event_subscribe events;

    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <PORT NUMBER> %s <STREAM OPTION>\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    argopt = atoi(argv[2]);
    if (argopt == -1) {
        perror("atoi - wrong stream option");
        exit(EXIT_FAILURE);
    }

    listenfd = socket(PF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    if (listenfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(struct sctp_initmsg));   
    initmsg.sinit_num_ostreams = 10;
    initmsg.sinit_max_instreams = 10;
    initmsg.sinit_max_attempts = 5;
    initmsg.sinit_max_init_timeo = 60000;

    events.sctp_data_io_event = 1;

    if(setsockopt(listenfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg))==-1){
        perror("socket option");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(listenfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events))==-1){
        perror("socket option");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family             =       AF_INET;
    servaddr.sin_port               =       htons(atoi(argv[1]));
    servaddr.sin_addr.s_addr        =       htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 5) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }


    socklen_t fromlen=sizeof(clientaddr);
    int stream = 0;

    for (;;) {
        res = sctp_recvmsg(listenfd, (void*)buffer, BUFF_SIZE, (struct sockaddr *)&clientaddr, &fromlen, &sri, 0);
        if (res == -1) {
            perror("sctp_recvmsg()");
            exit(EXIT_FAILURE);
        }
        printf("received %s stream: %d\n", buffer, sri.sinfo_stream);
        
        if (argopt == 0) {
            stream = 0;
        }
        else {
            stream = sri.sinfo_stream + 1;
            if (stream >= 3) stream = 0;
        }
        bytes = sctp_sendmsg(listenfd, &buffer, sizeof(buffer), (struct sockaddr*)&clientaddr, sizeof(struct sockaddr_in), 0, 0, stream, 0, 0);

	    if (bytes == -1) {
            perror("sctp_sendmsg()");
            exit(EXIT_FAILURE);
        }
    }

}
