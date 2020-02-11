#include "srv_h.h"

char* read_command_output(const char* command)
{
    const int CHUNK = BUFFSIZE;

    FILE* pipe = popen(command, "r");
    if (pipe == NULL)
    {
        puts("popen failed");
        exit(1);
    }
    int capacity = CHUNK;
    int position = 0;
    char* buffer = malloc(capacity);
    if (buffer == NULL)
    {
        puts("malloc failed");
        exit(1);
    }
    for (;;)
    {
        if (position + CHUNK > capacity)
        {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (buffer == NULL)
            {
                puts("realloc failed");
                exit(1);
            }
        }
        int bytes_read = fread(buffer + position, 1, CHUNK, pipe);
        position += bytes_read;
        if (bytes_read < CHUNK)
        {
            break;
        }
    }
    buffer = realloc(buffer, position + 1);
    if (buffer == NULL)
    {
        puts("realloc failed");
        exit(1);
    }
    buffer[position] = '\0';

    pclose(pipe);
    return buffer;
}

void cmd_child(int connfd)
{

    size_t uchwyt;
    int  zmienNazwe=0;
    int k, sndbuf;
    char* komenda;
    char buf[BUFFSIZE2], command[BUFFSIZE];
    char command2[BUFFSIZE], command3[BUFFSIZE];

    strcpy(buf, "Podaj haslo: ");
    send(connfd, buf, BUFFSIZE,0);
    recv(connfd, buf, sizeof(buf), 0);
    if (!strcmp(buf, "12345678\n"))
    {
        strcpy(buf, "Zalogowano.");
        send(connfd, buf, BUFFSIZE,0);
    }
    else
    {
        strcpy(buf, "Niepoprawne haslo, rozlaczanie.");
        send(connfd, buf, BUFFSIZE,0);
        exit(1);
    }

    /*----pobranie komendy----*/
    // chroot("~/");
    // chdir("~/");
    // chroot("~/ftp");
again:

    recv(connfd, buf, sizeof(buf), 0);// > 0//  if ( ) {
    //Rozpoznanie komendy

    sscanf(buf, "%s %s %s", command, command2, command3);//WYBOR CZYNNOSCI W TAKI SPOSOB
    if(!strcmp(command, "ls"))//TAK SPRAWDZAMY CO MA SIE STAC, nie musi byc to komenda
    {
        strcat(command," ");
        strcat(command, command2);
        komenda=read_command_output(command);
        strcpy(buf, komenda);
        send(connfd, buf, BUFFSIZE,0);
    }
    else if(!strcmp(command, "szukaj"))  //TAK SPRAWDZAMY, nie musi byc to komenda
    {
        strcpy(command,"find -name ");
        strcat(command,command2);
        komenda=read_command_output(command);
        sprintf(buf, "%s", komenda);
        send(connfd, buf, BUFFSIZE,0);
    }
    else if(!strcmp(command, "nazwa"))
    {
        zmienNazwe=rename(command2, command3);
        if (zmienNazwe==-1)
            strcpy(buf,"Nie ma takiego pliku lub inny blad.\n");
        else strcpy(buf,"Sukces!\n");
        send(connfd, buf, BUFFSIZE,0);
    }
    else if(!strcmp(command, "pwd"))
    {
        // chroot("~/");
        // chdir("~/");
        getcwd(cwd, sizeof(cwd));
        // komenda=read_command_output(command);

        strcpy(buf, cwd);
        send(connfd, buf, BUFFSIZE,0);
    }
    else if(!strcmp(command, "pobierz"))
    {
        printf("Sending... \n" );
        send_file( command2, connfd);
        printf("OK \n" );
    }
    else if(!strcmp(command, "wyslij"))
    {
        printf("Receiving... \n" );
        /*---- Read the message from the server into the buffer ----*/
        recv_file( connfd );
        printf("OK \n" );
    }
    else if(!strcmp(command, "cd"))
    {
        // chroot(command2);
        chdir(command2);
        getcwd(cwd, sizeof(cwd));
        strcpy(buf, cwd);
        send(connfd, buf, BUFFSIZE,0);
    }

    for (int a=0; a<BUFFSIZE2; a++)
        buf[a]=0;

    goto again;
};

void child_main(int i, int listenfd, int addrlen)
{
    int connfd;

    socklen_t clilen;
    struct sockaddr *cliaddr;
    cliaddr = malloc(addrlen);
// printf("child %ld starting\n", (long) getpid());
    for ( ; ; )
    {
        clilen = addrlen;
        connfd = accept(listenfd, cliaddr, &clilen);
        cmd_child(connfd); /* process the request */
        close(connfd);
    }
}

pid_t child_make(int i, int listenfd, int addrlen)
{
    pid_t pid;

    void child_main(int, int, int);

    if ( (pid = fork()) > 0)
        return (pid); /* parent */

    child_main(i, listenfd, addrlen);
}  /* never returns */

int daemon_init(const char *pname, int facility, uid_t uid, int socket)
{
    int     i, p;
    pid_t   pid;

    if ( (pid = fork()) < 0)
        return (-1);
    else if (pid)
        exit(0);            /* parent terminates */

    /* child 1 continues... */

    if (setsid() < 0)           /* become session leader */
        return (-1);

    signal(SIGHUP, SIG_IGN);
    if ( (pid = fork()) < 0)
        return (-1);
    else if (pid)
        exit(0);            /* child 1 terminates */

    /* child 2 continues... */

    // chdir("/tmp");              /* change working directory  or chroot()*/
    chroot(getenv("HOME"));
    chdir(getenv("HOME"));
    /* close off file descriptors */
    for (i = 0; i < MAXFD; i++)
    {
        if(socket != i )
            close(i);
    }

    /* redirect stdin, stdout, and stderr to /dev/null */
    p= open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    openlog(pname, LOG_PID, facility);

    syslog(LOG_ERR," STDIN =   %i\n", p);
    setuid(uid); /* change user */

    return (0);             /* success */
}
