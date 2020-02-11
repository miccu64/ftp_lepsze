#include <stdio.h>
#include <string.h>
#include <sys/sendfile.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "srv_h.h"

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
    int clientSocket;
    size_t uchwyt=0;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
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
