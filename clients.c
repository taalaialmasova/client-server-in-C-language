/*clients.c                */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h> 
#include <errno.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include "restart.h"

#define PERMS (mode_t)(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define FLAGS (O_CREAT | O_EXCL)
#define Localhost "127.0.0.1"
#define Cfilename "log/clients.log"
#define BUF_SIZE 256
/**** prototypes */
int connectToServer(int* sockFd);
void Error(char *msg);
int isValidArguments(int argc, char argv[]);
void *CommClientServer(void *args);
double calculateSD();
/***    global variables*/
int irow_size, icol_size, iclients_size, iportID;
pthread_t  *threadArr;
int threadcount=0;
double *Time;
int timecount=0;
int main(int argc, char *argv[])
{
	/* sem and thread */
	sem_t semlock;
	int i, error, error1;
	double SD;
	double sum=0.0;
	FILE *logFile;
	
	if (isValidArguments(argc, argv[1]) < 0){
		return 1;
	}
	
	system("clear");
	iportID = atoi(argv[1]);
	irow_size = atoi(argv[2]);
	icol_size = atoi(argv[3]);
	iclients_size = atoi(argv[4]);
	threadArr= (pthread_t*)malloc(sizeof(pthread_t)*iclients_size);
   	if (threadArr == NULL) {
      	Error("Failed to allocate memory for thread IDs :");
      	exit(1);
   	}
   	if (sem_init(&semlock, 0, 1) == -1) {
		perror("Failed to initialize semaphore");
		exit(1);
	}
	Time=(double*)malloc(sizeof(double)*iclients_size);
	for (i=0; i<iclients_size; i++) {
		
         error = pthread_create(threadArr+i, NULL, CommClientServer, &semlock);
	      if (error == -1) {
	         fprintf(stderr, "Failed to create thread:%s\n", strerror(error));
	         return 1;
	      }
	      
	     
    }
    
    /*if (sem_close(&semlock) == -1)
	{
		Error("failed to close semaphore");
	}*/
    logFile=fopen(Cfilename,"a");
    for (i = 0; i < iclients_size; i++){
		  if ((error1 = pthread_join(threadArr[i], NULL))==-1) {
		    Error("Faild to create thread" );
		  }
	}
	for (i = 0; i < iclients_size; ++i)
    {
    	sum+=Time[i];
    }
    SD=calculateSD();
    fprintf(logFile, "The average connection time\t and the standard deviation,\n" );
    fprintf(logFile, "%.6f\t %.6f\n", (sum/(double)timecount),SD );
    fclose(logFile);
	free(Time);
	free(threadArr);
	
	return 0;

}

/*
* Initiate communication with a remote server.
*
* parameters:
*    		  port = well-known port on remote server
*			  hostn = character string giving the Internet name of remote host
* returns: a communication file descriptor if successful
*			-1 on error with errno set
*/
int connectToServer(int* sockFd){
	int error;
	int retval ;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	struct in_addr ipv4addr;
	fd_set sockset;
	inet_pton(AF_INET, Localhost, &ipv4addr);
	server = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);

	if(server == NULL){
		Error("NO such a host!\n");
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(iportID);
	
	if (((retval = connect(*sockFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1) &&
		((errno == EINTR) || (errno == EALREADY))) {
		FD_ZERO(&sockset);
		FD_SET(*sockFd, &sockset);
		while ( ((retval = select(*sockFd+1, NULL, &sockset, NULL, NULL)) == -1) &&
			(errno == EINTR) ) {
			FD_ZERO(&sockset);
			FD_SET(*sockFd, &sockset);
		}
	}
	if (retval == -1) {
		if(errno == EADDRNOTAVAIL)
			fprintf(stderr, "EADDRNOTAVAIL\n");
		if(errno == EAFNOSUPPORT)
			fprintf(stderr, "EAFNOSUPPORT\n");
		if(errno == ECONNREFUSED)
			fprintf(stderr, "ECONNREFUSED\n");

		error = errno;
		while ((close(*sockFd) == -1) && (errno == EINTR));
		errno = error;
		return -1;
	}
	return *sockFd;
}
void *CommClientServer(void *value) {
    int isockFd, i , j;
    char  buffer[15];
    char buf[25];
    double number;
    time_t start_t;
    FILE *logFile;
	
   sem_t *semlockp;
   semlockp = (sem_t *)value;
  	start_t=time(NULL);
	while (sem_wait(semlockp) == -1)        /* Entry section */
      if(errno != EINTR) {
         fprintf(stderr, "Thread failed to lock semaphore\n");
         return NULL;
      }
      
     if ( ((isockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)){
		Error("ERROR opening socket!");
	}
	if (connectToServer(&isockFd) == -1){
		Error("Connection faild.");
	}
   sprintf(buf, "I am client with number #");
	if (write(isockFd, buf, sizeof(buf)) != sizeof(buf)){
		Error("partial/failed write");
	}
	
	printf("Sending message to server... \n\n...\n\n" );
	 
	if (write(isockFd, (void*)&irow_size, sizeof(irow_size)) != sizeof(irow_size)){
		Error("partial/failed write");
	}
	if (write(isockFd, (void*)&icol_size, sizeof(icol_size)) != sizeof(icol_size)){
		Error("partial/failed write");
	}
	 read(isockFd, buffer, sizeof(buffer));
	 printf("Message from SERVER: >> %s\n", buffer);
	 logFile=fopen(Cfilename,"a");
	 fprintf(logFile, "%s\n","Matrix" );
	for(i=0; i<icol_size; i++)
	{
		for (j = 0; j < irow_size; ++j)
		{
			read(isockFd, (void*)&number, sizeof(number));
			fprintf(logFile,"%fx%d   ", number, j);
		}

		read(isockFd, (void*)&number, sizeof(number));
		fprintf(logFile, " =  %f\n", number);
	}
	 	
	 	 
	 fclose(logFile);
	 Time[timecount]=difftime(time(NULL), start_t)*1000;
	 timecount++;
    close(isockFd);
   /****************** exit section ********************************/
   if (sem_post(semlockp) == -1)         /* Exit section */
      fprintf(stderr, "Thread failed to unlock semaphore\n");
   	return NULL;
}
int isValidArguments(int argc, char argv[]){
	if(argc!=5){
		fprintf(stderr, "Invalid arguments!\n");
		fprintf(stderr, "Usage: ./Client PortId rowSize columnSize sizeOFclients\n");
		return -1;
	}

	return 0;
}

void Error(char *msg){
	perror(msg);
	exit(1);
}
double calculateSD()
{
    double sum = 0.0, mean, standardDeviation = 0.0;

    int i;

    for(i=0; i<10; ++i)
    {
        sum += Time[i];
    }

    mean = sum/10;

    for(i=0; i<10; ++i)
        standardDeviation += pow(Time[i] - mean, 2);

    return sqrt(standardDeviation/10);
}
