#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h> /* inet_pton() */
#include <net/if_arp.h>
#include <netinet/in.h> /* struct sockaddr_in */
#include <sys/ioctl.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int retval;
    struct arpreq request;
    struct ifreq st, st1;

    if (argc != 4)
    {
        fprintf(stderr, "Invocation: %s <INTERFACE NAME> <MAC ADDRESS> <MTU ADDRESS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&st, 0, sizeof(struct ifreq));
    memset(&st1, 0, sizeof(struct ifreq));

    int fd = socket(PF_INET, SOCK_DGRAM, 0);

    // get and print current MAC addres
    strcpy(st.ifr_name, argv[1]);
    if (0 == ioctl(fd, SIOCGIFHWADDR, &st))
    {
        int i;
        puts("Current MAC addres :");
        for (i = 0; i < 6; ++i)
            printf(" %02x", (unsigned char)st.ifr_addr.sa_data[i]);
    }

    printf("\n");

    strcpy(st.ifr_name, argv[1]);

    // get and print current mtu
    if (ioctl(fd, SIOCGIFMTU, &st) == -1)
    {
    }
    printf("Current MTU: %d\n", st.ifr_mtu);

    memset(&request, 0, sizeof(struct arpreq));

    strcpy(st1.ifr_ifrn.ifrn_name, argv[1]);

    // Interface
    st1.ifr_mtu = atoi(argv[3]);
    // MTU
    retval = ioctl(fd, SIOCSIFMTU, &st1);
    if (retval == -1)
    {
        perror("2 ioctl()");
        return 1;
    }

    st1.ifr_flags &= ~IFF_UP;

    retval = ioctl(fd, SIOCSIFFLAGS, &st1);
    if (retval == -1)
    {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }

    /* Adres MAC: */
    st1.ifr_ifru.ifru_hwaddr.sa_family = ARPHRD_ETHER;

    retval = sscanf(
        argv[2], "%2x:%2x:%2x:%2x:%2x:%2x",
        (unsigned int *)&st1.ifr_hwaddr.sa_data[0],
        (unsigned int *)&st1.ifr_hwaddr.sa_data[1],
        (unsigned int *)&st1.ifr_hwaddr.sa_data[2],
        (unsigned int *)&st1.ifr_hwaddr.sa_data[3],
        (unsigned int *)&st1.ifr_hwaddr.sa_data[4],
        (unsigned int *)&st1.ifr_hwaddr.sa_data[5]);

    if (retval != 6)
    {
        fprintf(stderr, "Invalid address format!\n");
        exit(EXIT_FAILURE);
    }

    retval = ioctl(fd, SIOCSIFHWADDR, &st1);
    if (retval == -1)
    {
        perror("ioctl SIOCSIFHWADDR()");
        exit(EXIT_FAILURE);
    }

    st1.ifr_flags |= IFF_UP | IFF_RUNNING;
    retval = ioctl(fd, SIOCSIFFLAGS, &st1);
    if (retval == -1)
    {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }

    retval = ioctl(fd, SIOCGIFMTU, &st1);
    if (retval == -1)
    {
        perror("ioctl()");
        exit(EXIT_FAILURE);
    }

    printf("\nData changed\n");

    fprintf(stdout, "MTU set to: %d\n", st1.ifr_mtu);

    close(fd);

    printf("MAC address set to : %s\n", argv[2]);

    exit(EXIT_SUCCESS);
}
