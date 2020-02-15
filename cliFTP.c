#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "srv_h.h"

void multicast(char* buf, int count)
{
    struct sockaddr_in localSock;
    struct ip_mreq group;
    int sd;
    int datalen;
    char databuf[20];
    char adres_multicast[20];
    char adres_klienta[20];

    strcpy(adres_multicast,"226.1.1.1");
    strcpy(adres_klienta,"10.0.2.15");//TRZEBA USTAWIC ADRES KARTY SIECIOWEJ!!!
    /* Create a datagram socket on which to receive. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
        perror("Opening datagram socket error");
        exit(1);
    }
    else
        printf("Opening datagram socket....OK.\n");

    /* Enable SO_REUSEADDR to allow multiple instances of this */
    /* application to receive copies of the multicast datagrams. */
    {
        int reuse = 1;
        if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
        {
            perror("Setting SO_REUSEADDR error");
            close(sd);
            exit(1);
        }
        else
            printf("Setting SO_REUSEADDR...OK.\n");
    }
    /* Bind to the proper port number with the IP address */
    /* specified as INADDR_ANY. */
    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(4321);
    localSock.sin_addr.s_addr = INADDR_ANY;
    if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
    {
        perror("Binding datagram socket error");
        close(sd);
        exit(1);
    }
    else
        printf("Binding datagram socket...OK.\n");

    /* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */
    /* interface. Note that this IP_ADD_MEMBERSHIP option must be */
    /* called for each local interface over which the multicast */
    /* datagrams are to be received. */
    group.imr_multiaddr.s_addr = inet_addr(adres_multicast);//adres multicast
    group.imr_interface.s_addr = inet_addr(adres_klienta);//adres karty sieciowej klienta
    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
    {
        perror("Adding multicast group error");
        close(sd);
        exit(1);
    }
    else
        printf("Adding multicast group...OK.\n");
    /* Read from the socket. */
    datalen = sizeof(databuf);
    if(read(sd, databuf, datalen) < 0)
    {
        perror("Reading datagram message error");
        close(sd);
        exit(1);
    }
    else
    {
        printf("Reading datagram message...OK.\n");
        printf("The message from multicast server is: \"%s\"\n", databuf);
    }
    for(int i = 0; i < count; ++i)
        buf[i] = databuf[i];
    return;
}

void pomoc()
{
    printf("\n\nCo chcesz zrobic? \nsend [file_name] - przeslij plik \nreceive [file_name] - pobierz plik ");
    printf("\nnazwa [obecna nazwa] [nowa nazwa] - zmien nazwe/przenies plik \nszukaj [fraza] - wyszukaj w obecnym folderze");
    printf("\nls - wylistuj pliki ");
    printf("\npwd - obecny folder \ncd [path] - zmien folder \nhelp - for help  \nexit - for exit \n");
    return;
}

int main()
{
    char buffer[BUFFSIZE], kopia [BUFFSIZE], nazwaPliku[255], option[255];
    char command2[BUFFSIZE], command[BUFFSIZE], command3[BUFFSIZE];
    char bufor[60]={0};
    char adres[30]={0};
    char port[30]={0};
    int clientSocket;
    size_t uchwyt=0;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    multicast(bufor, sizeof(bufor));
    sscanf(bufor,"%s %s", adres, port);

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    int ii;
    sscanf(port, "%d", &ii);
    serverAddr.sin_port = htons(ii);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr(adres);
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s",buffer);
    fgets(buffer, BUFFSIZE, stdin );
    send(clientSocket, buffer, BUFFSIZE,0);
    recv(clientSocket, buffer, BUFFSIZE, 0);
    printf("%s",buffer);
    if(strcmp(buffer, "Zalogowano."))
    {
        close(clientSocket);
        return 0;
    }

    pomoc();

    while(1)
    {
        printf("\n=> ");
        fgets( option, 255, stdin );
        sscanf(option,"%s %s %s", command, command2, command3);//WYBOR CZYNNOSCI W TAKI SPOSOB

        if(!strcmp(command, "send"))//przeslij plik
        {
            strcpy(buffer, "wyslij ");
            send(clientSocket, buffer, sizeof(buffer),0);
            printf("Sending... \n" );
            send_file( command2, clientSocket);
            printf("OK \n" );
        }
        else if(!strcmp(command, "receive"))//pobierz plik
        {
            strcpy(buffer, "pobierz ");
            strcat(buffer, command2);
            send(clientSocket, buffer, sizeof(buffer),0);
            printf("Receiving... \n" );
            /*---- Read the message from the server into the buffer ----*/
            recv_file( clientSocket );
            printf("OK \n" );
        }
        else if(!strcmp(command, "help"))//przeslij plik
        {
            pomoc();
        }
        else if(!strcmp(command, "ls"))//wyswietl pliki
        {
            strcpy(buffer, "ls -l");
            send(clientSocket, buffer, BUFFSIZE,0);
            /*---- Read the message from the server into the buffer ----*/
            recv(clientSocket, buffer, BUFFSIZE, 0);
            /*---- Print the received message ----*/
            printf("%s", buffer);
        }
        else if(!strcmp(command, "szukaj"))
        {
            strcpy(buffer,"szukaj \"*");
            strcat(buffer, command2);
            strcat(buffer, "*\"*");
            send(clientSocket, buffer, BUFFSIZE,0);
            /*---- Read the message from the server into the buffer ----*/
            recv(clientSocket, buffer, BUFFSIZE, 0);
            /*---- Print the received message ----*/
            printf("%s", buffer);
        }
        else if(!strcmp(command, "nazwa"))
        {
            strcpy(buffer,command);
            strcat(buffer," ");
            strcat(buffer,command2);
            strcat(buffer," ");
            strcat(buffer,command3);
            send(clientSocket, buffer, BUFFSIZE,0);
            /*---- Read the message from the server into the buffer ----*/
            recv(clientSocket, buffer, BUFFSIZE, 0);
            /*---- Print the received message ----*/
            printf("%s", buffer);
        }
        else if(!strcmp(command, "pwd"))
        {
            strcpy(buffer,"pwd");
            send(clientSocket, buffer, BUFFSIZE,0);
            /*---- Read the message from the server into the buffer ----*/
            recv(clientSocket, buffer, BUFFSIZE, 0);
            /*---- Print the received message ----*/
            printf("%s", buffer);
        }
        else if(!strcmp(command, "cd"))//wyswietl pliki
        {
            strcpy(buffer,"cd ");
            strcat(buffer, command2);
            send(clientSocket, buffer, BUFFSIZE,0);
            /*---- Read the message from the server into the buffer ----*/
            recv(clientSocket, buffer, BUFFSIZE, 0);
            /*---- Print the received message ----*/
            printf("%s", buffer);
        }
        else if(!strcmp(command, "exit"))//wyswietl pliki
        {
            close(clientSocket);
            return 0;
        }
        else printf("Nieprawidlowa komenda.\n");
        for (int a=0; a<BUFFSIZE; a++)
            buffer[a]=0;//czyszczenie bufora
    }
    return 0;
}
