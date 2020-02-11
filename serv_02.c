#include "srv_h.h" 




 static int nchildren;
 static pid_t *pids;





 // if (argc == 3)
 // listenfd = Tcp_listen(NULL, argv[1], &addrlen);
 // else if (argc == 4)
 // listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
 // else
 // err_quit("usage: serv02 [ <host> ] <port#> <#children>");
 // nchildren = atoi(argv[argc - 1]);

  int  main(int argc, char **argv)
 {


 	nchildren=3;

    int listenfd, connfd;
    struct sockaddr_in serverAddr, cliAddr;
    int k, sndbuf;
    socklen_t clilen;
    char* komenda;
    pid_t pid;


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
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
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

 // signal(SIGINT, sig_int);
 for ( ; ; )
 pause(); /* everything done by children */
 }