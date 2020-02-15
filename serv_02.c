#include "srv_h.h"

static int nchildren;
static pid_t *pids;

void multicast (char adres[20], char port[20])
{
    struct in_addr localInterface;
    struct sockaddr_in groupSock;
    int sd;
    char databuf[20];//ADRES DO PRZESYLU DANYCH
    int datalen = sizeof(databuf);
    char adres_multicast[20];
    char adres_serwera[20];

    strcpy(adres_multicast,"226.1.1.1");
    strcpy(adres_serwera,"10.0.2.15");//TRZEBA USTAWIC ADRES KARTY SIECIOWEJ!!!
    strcpy(databuf, adres);
    strcat(databuf, " ");
    strcat(databuf, port);

    /* Create a datagram socket on which to send. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("Opening datagram socket error");
        exit(1);
    }
    else
        printf("Opening the datagram socket...OK.\n");
    /* Initialize the group sockaddr structure with a */
    /* group address of 225.1.1.1 and port 5555. */
    memset((char *) &groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr(adres_multicast);
    groupSock.sin_port = htons(4321);

    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */
    localInterface.s_addr = inet_addr(adres_serwera);
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
    {
        perror("Setting local interface error");
        exit(1);
    }
    else

        printf("Setting the local interface...OK\n");
    /* Send a message to the multicast group specified by the*/
    /* groupSock sockaddr structure. */
    /*int datalen = 1024;*/
    while(1)
    {
        if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
        {
            perror("Sending datagram message error");
        }
        else
            //printf("Sending datagram message...OK\n");
        usleep(3000000);
    }
    return;
}

int  main(int argc, char **argv)
{
    nchildren=3;

    int listenfd, connfd;
    struct sockaddr_in serverAddr, cliAddr;
    int k, sndbuf;
    socklen_t clilen;
    char* komenda;
    pid_t pid;
    char port[20];
    char adres[20];

    strcpy(port, "7891");
    strcpy(adres, "127.0.0.1");

    int i;
    socklen_t addrlen;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        printf("Socket creation failed");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;

    /* Set port number, using htons function to use proper byte order */
    int ii;
    sscanf(port, "%d", &ii);
    serverAddr.sin_port = htons(ii);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr(adres);
    sndbuf = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sndbuf, sizeof(sndbuf)) < 0)
    {
        printf("setsockopt error");
    }
    /*---- Bind the address struct to the socket ----*/
    k = bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (k == -1)
    {
        printf("Binding error");
        exit(1);
    }
    /*---- Listen on the socket, with 5 max connection requests queued ----*/
    if (listen(listenfd, LISTENQ) == -1)
    {
        printf("Listening error\n");
        exit(1);
    }
    /*---- Accept call creates a new socket for the incoming connection ----*/
    /*---- Send message to the socket of the incoming connection ----*/
    /*----Koniec inicjalizacji, poczatek obslugi komend ----*/
    // daemon_init(argv[0], LOG_USER, 1000, listenfd);
    // syslog (LOG_NOTICE, "Program started by User %d", getuid ());
    // syslog (LOG_INFO,"Waiting for clients ... ");
    printf("Serwer jest uruchomiony.\n");

    addrlen = sizeof(cliAddr);
    pids = calloc(nchildren, sizeof(pid_t));

    for (i = 0; i < nchildren; i++)
        pids[i] = child_make(i, listenfd, addrlen); /* parent returns */

    multicast(adres, port);

// signal(SIGINT, sig_int);
    for ( ; ; )
        pause(); /* everything done by children */
}
