#include    <sys/types.h>   /* basic system data types */
#include    <sys/socket.h>  /* basic socket definitions */
#include    <sys/sendfile.h>
#include    <sys/time.h>    /* timeval{} for select() */
#include    <time.h>                /* timespec{} for pselect() */
#include	<netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>   /* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>               /* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<wait.h>
#include	<unistd.h>
#include	<syslog.h>



#define LISTENQ 5
#define BUFFSIZE 1024
#define BUFFSIZE2 80000
#define MAXFD   64
char    str[INET_ADDRSTRLEN+1];

char cwd[BUFFSIZE];


char* read_command_output(const char* command);

void child_main(int i, int listenfd, int addrlen);

pid_t child_make(int i, int listenfd, int addrlen);

void cmd_child(int connfd);


int daemon_init(const char *pname, int facility, uid_t uid, int socket);


int send_file( char *fname, int c);
int recv_file(int new_s );