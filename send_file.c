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



send(c,sname,256,0);


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
char * buffer = (char*) malloc(sizeof(char) * lSize);
int b = 0;
printf("%d\n", lSize );
int *ptrvar;        
     ptrvar = &lSize;
send(c, ptrvar, sizeof(int), 0);


while((ch=getc(file))!=EOF){
        toSEND[0] = ch;
        send(c, toSEND, 1, 0);

}

free(buffer);
return 0;
}






int recv_file(int new_s ){


  char *fname=(char *)malloc(256);
	int s;

recv(new_s, fname, 256,0);
  
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
char * buffer = (char*) malloc(sizeof(char) * i_size);
int nbytes=0;
printf("size -%d\n",i_size );
buffer[0]=0;

while(1){
                if( ccc=recv(new_s, buffer, 1, 0) != 0 ) {        // https://stackoverflow.com/questions/15116053/some-eof-mechanism-for-send-recv-in-c
                    fwrite (buffer , sizeof(buffer[0]) , 1 , file);  //write EOF mechanism for recv 
                     printf("%d\n",ccc);
                }

                    else{
                    	printf("end\n");
 
  fclose(file);
  free(buffer);
  free(fname);
 return 0;}

}}



