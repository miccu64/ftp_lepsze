#include "srv_h.h"

int send_file( char *fname, int c){

int ch;
   char toSEND[1];
   	   char *sname;
   char *nm = strtok (fname,"/");

      while (nm != 0){
        sname =nm;
        nm = strtok (0,"/");
   }    printf("%s\n", sname );

send(c,sname,BUFFSIZE,0);

     FILE *file;
  file = fopen( fname, "rb" );
  if (!file)
  {
    printf(" No such file \n" );
    return -1;
  }
  fseek(file , 0 , SEEK_END);                          // устанавливаем позицию в конец файла
  int lSize = ftell(file);                            // получаем размер в байтах
  rewind (file);                                      // устанавливаем указатель в конец файла
char * buffer = malloc(lSize);
int b = 0;
printf("%d\n", lSize );
int *ptrvar;
     ptrvar = &lSize;

int file_desc = fileno(file);

 // struct stat  obj;
 //  stat(fname, &obj);
 // int file_desc = open(fname, O_RDONLY| O_BINARY,3);

 // int file_size = obj.st_size;
 // int file_size = file_desc.st_size;
// printf("%d\n", file_size );
 send(c, ptrvar, sizeof(int), 0);
 fread(buffer, lSize, 1, file);
 printf("send\n");
// fwrite(buffer,lSize, 1,file);
 // sendfile(c, file_desc, NULL, lSize);
 int nbytes = send(c, buffer, lSize, 0);

 printf("sended %d\n",nbytes );
// while((ch=getc(file))!=EOF){
//         toSEND[0] = ch;
//         send(c, toSEND, 1, 0);
        // printf("%s\n",toSEND );
 printf("close\n");
// }
      // if(feof(file)){
      // 	printf("feof");
      // 	// break;
      // }
fclose(file);
close(file_desc);
free(buffer);
return 0;
}


int recv_file(int new_s ){


  char *fname=(char *)malloc(BUFFSIZE);
	int s;

recv(new_s, fname, BUFFSIZE,0);

//sscanf(argv[2], "%s", fname);
printf("%s\n",fname );
FILE *file;
file = fopen( fname, "wb" );
  if (!file)
  {
    printf("Can't create a file \n" );
    return -1;
  }

rewind (file);

int *file_size;        // объявление указателя
int i_size;
file_size=&i_size;
recv(new_s, file_size, sizeof(int),0);  //size of recivied file

 //printf("size%d --- %d \n ",i_size, *file_size );
int ccc;
// char * buffer = (char*) malloc(sizeof(char) * i_size);
int nbytes=0;
int recivied=0;
char toRECV[1];
printf("size- %d\n",i_size );
// buffer[0]=0;


    char * buffer = malloc(i_size);
    // int file_desc = fileno(file);
    while(recivied<i_size){
    nbytes=recv(new_s, buffer, i_size, 0);
    printf("recivied %d\n",nbytes );
    recivied=nbytes+recivied;
    // write(file_desc, buffer, i_size);
    fwrite (buffer , nbytes , 1 , file);}

    fclose(file);
    free(buffer);
    free(fname);

    return 0;

}

/*while(1){



                if(i_size >= -1 & recv(new_s, toRECV, 1, 0) !=0 ) {        // https://stackoverflow.com/questions/15116053/some-eof-mechanism-for-send-recv-in-c

                    fwrite (toRECV , sizeof(toRECV[0]) , 1 , file);  //write EOF mechanism for recv
                     i_size=i_size-sizeof(toRECV[0]);
                    // if (i_size<-1)
                    // {
                    // 	break;
                    // }
                     printf("%d\n",i_size);
                }

                    else{
                    	printf("end\n");

  fclose(file);
  free(buffer);
  free(fname);
 return 0;}

}*/
// }

