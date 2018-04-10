#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <semaphore.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/shm.h>
#include "restart.h"
#include "matrix.h"

#define Cfilename "log/clients.log"
#define MAXLISTENO 200
#define BUF_SIZE 256
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

void sighandeler(int signo);
void signal_callback_handler(int signum);
int isValidArguments(int argc, char argv[]);
int open_socket(int* sock, unsigned short portNo) ;
void Error(char *msg);
void* processHelper(void *new_sockfd);
int detachandremove(int shmid, void *shmaddr) ;
void* thread1(void* value);
void* thread2(void* value);
void* thread3(void* value);
/*   global variables*/
int iSockfd;
int socketCount=0;
pthread_t threadClient[256];
pthread_t threadCount=0;
pid_t pidP1[BUF_SIZE], pidP2[BUF_SIZE], pidP3[BUF_SIZE];
int Cp1=0, Cp2=0, Cp3=0;
int thpool;
/*                        main                        **/
/*******************************************************/
int main(int argc, char *argv[])
{
	int iportID;
	int iCommunSockFd[MAXLISTENO], iClilen;
	struct sockaddr_in SServ_addr, SCli_addr;
	int i=0, status, error1;
	ssize_t rc;
	
	if (isValidArguments(argc, argv[1]) < 0){
		return 1;
	}
	system("clear");
	iportID= atoi(argv[1]);
	thpool=atoi(argv[2]);
	/* open log file*/
	remove(Cfilename);
	
	signal(SIGINT,sighandeler);
	/* Catch Signal Handler SIGPIPE */
	signal(SIGPIPE, signal_callback_handler);
	/* server oldugunu bilmek icin :)*/
	printf(" I am Server with port ID %d...  :]\n", iportID);
	
	if (open_socket(&iSockfd,iportID) == -1){
		return -1;
	}

	puts("Waiting for incoming connections...");
	while(1)
	{
		while((iCommunSockFd[i] = accept(iSockfd,(struct sockaddr *) &SCli_addr, &iClilen)) ==-1 && errno== EINTR);
	    if(iCommunSockFd[i] == -1){
	    	r_close(iSockfd);
			Error("ERROR on accept");
		}
		printf("number of clients currently being served >> %d \n", i+1);
	    if(status=pthread_create(threadClient+i, NULL, processHelper, (void*)&iCommunSockFd[i]) ){
			r_close(iSockfd);
			r_close(iCommunSockFd[i]);
			Error("Faild to create thread" );
		}
		i++;
		socketCount++;
	}
	for (i = 0; i < socketCount; i++){
		  if (error1 = pthread_join(threadClient[i], NULL)) {
		   Error("Failed to join thread: ");
		  }
	}
	return 0;
}
	
int isValidArguments(int argc, char argv[]){
	unsigned long int portno=0;
	char* endp;
	if(argc != 3){
		fprintf(stderr, "Invalid arguments!\n");
		fprintf(stderr, "Usage: ./server portID thpoolSize\n");
		return -1;
	}
	portno=strtoul (argv, &endp, 0);

	if(portno==0 || *endp!='\0' || portno<0 || portno>65535 || errno==ERANGE){
		fprintf(stderr, "Invalid port id!\n");
		fprintf(stderr, "Usage: port id should be between 1 ~ 65535.\n");
		return -1;
	}
}
/*
*
* Return a file descriptor, which is bound to the given port.
*
* parameter:
*			s = number of port to bind to
* returns: file descriptor if successful
*			-1 on error and sets errno
* (referenced from  the course book)
*/
int open_socket(int* sock, unsigned short portNo) {
	int error;
	struct sockaddr_in  serv_addr;
	//int sock;
	int true = 1;
	
	if (((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)){
		Error("ERROR opening socket");
	}
	if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (char *)&true, sizeof(true)) == -1) {
		error = errno;
		while ((close(*sock) == -1) && (errno == EINTR));
		errno = error;
		return -1;
	}
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portNo);
	if ((bind(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) ||
		(listen(*sock, MAXLISTENO) == -1)) {
		error = errno;
		while ((close(*sock) == -1) && (errno == EINTR));
		errno = error;
		Error("ERROR on binding");
	}
	return 0;
}
void* processHelper(void *new_sockfd){
	/*char buff[BUF_SIZE];*/
	int buff[2];
	int row, column, shmid1, shmid2, j, o, i, error, temp, sum=0;;
	char buf[25];
	double **Matrix;
	double *B;
	int sock = *(int*)new_sockfd;
	pid_t childP1, childP2, childP3;
	ssize_t rc, rc1;
	double *shm1, *shm2;
	double **s1, **s2;
	pthread_t threadP2[3];
	/**************************************************************/
	printf("\n...\n");
	puts("Connection accepted");
	read(sock, buf, 25);
     
    printf(" Receive message : *   %s*\n",buf);
    read(sock, (void*)&row, sizeof(row));
	read(sock, (void*)&column, sizeof(column));
	printf("Message :>>> %d %d\n", row, column);
	write(sock, "i am server  :)", 15);
	/* proccess**/
	Matrix = (double**) malloc(row*sizeof(double*));
	for (i = 0; i < row; i++){
		Matrix[i] = (double*)malloc(column*sizeof(double));
	}
	B = (double*)malloc(column*sizeof(double));
	double s11[column][row];
	childP1=fork();
	if(childP1==-1)
	{
		Error("Failed to fork:");
	}
	if(childP1==0)
	{
		shmid1 = shmget(2009, sizeof(double)*row*column,  0666 | IPC_CREAT);
      	shm1 = shmat(shmid1, 0, 0);
      	s1 = (double**) shm1;
      	fprintf(stderr, "%s\n","generating matrix..." );
		srand(time(NULL));
		for(o = 0; o<row; o++){
			
			/*fprintf(stderr,"|");*/
        	for(j = 0; j<column; j++){
        		sum+=o+j;
        		temp=(rand()%9);
            	Matrix[o][j] = (double)(temp);/*generates a random number 0-9*/
            	/*fprintf(stderr,"%.3f, ", Matrix[o][j]);*/
            	s11[o][j]=Matrix[o][j];
            	write(sock, (void*)&Matrix[o][j], sizeof(Matrix[o][j]));
        	}
        	B[o]=(double)(rand()%9);
        	write(sock, (void*)&B[o], sizeof(B[o]));
        	/*fprintf(stderr,"|  =|%.3f|",B[o] );
        	fprintf(stderr,"\n");*/

		}
		
		shmdt(shm1);
		exit(0);

	}
	pidP1[Cp1]=childP1;
	Cp1++;
	childP2=fork();
	if(childP2==-1){
		Error("Failed to fork:");
	}
	if(childP2==0){
		/*solving methods*/
		printf("solve with three methods...\n");
	    if ((error = pthread_create(&threadP2[0], NULL, thread1, NULL))) {
           Error("Failed to create thread(1) ");
        }
        if ((error = pthread_create(&threadP2[1], NULL, thread2, NULL))) {
           Error("Failed to create thread(2) ");
        }
        if ((error = pthread_create(&threadP2[2], NULL, thread3, NULL))) {
           Error("Failed to create thread(3) ");
        }
         
		 /* shared memory*/
		shmid1 = shmget(2009, sizeof(double)*row*column, 0666 | IPC_CREAT);
	    shm1 = shmat(shmid1, 0, 0);  
	    wait(NULL);
	    shmdt(shm1);
	    shmctl(shmid1, IPC_RMID, NULL);
	    if (error = pthread_join(threadP2[0], NULL)) {
		    Error("Failed to join thread(1): ");
		 }
		 if (error = pthread_join(threadP2[1], NULL)) {
		    Error("Failed to join thread(2): ");
		 }
		 if (error = pthread_join(threadP2[2], NULL)) {
		    Error("Failed to join thread(3): ");
		 }

	    /* create anew shared memory*/
	    shmid2 = shmget(2009, sizeof(double)*row*column,  0666 | IPC_CREAT);
      	shm2 = shmat(shmid2, 0, 0);
      	s2 = (double**) shm2;
      	for ( i = 0; i < column; ++i)
	    {
	    	for (j = 0; j < row; ++j)
	    	{
	    		s2[o][j] =Matrix[o][j]; 
    		}
	    }
      	
		shmdt(shm2);
		exit(0);
	}
	pidP2[Cp2]=childP2;
	Cp2++;
	childP3=fork();
	if(childP3==-1){
		Error("Failed to fork:");
	}
	if(childP3==0){
		printf("Verify...\n");
		shmid2 = shmget(2009, sizeof(double)*row*column, 0666 | IPC_CREAT);
	    shm2 = shmat(shmid2, 0, 0); 
	    wait(NULL); 
	    
	    shmdt(shm2);
	    shmctl(shmid2, IPC_RMID, NULL);
	    exit(0);
	}
	pidP3[Cp3]=childP3;
	Cp3++;
	/*while(wait(NULL)>0);*/
	for (i  = 0; i < 3; ++i)
	{
		wait(NULL);
	}
	
	for (i = 0; i < row; i++){
	   free(Matrix[i]);
	}
	free(Matrix);
	/* burda tane fork ve islemler*/
	return NULL;
}
void Error(char *msg){
	perror(msg);
	exit(1);
} 
int detachandremove(int shmid, void *shmaddr) 
{
   int error = 0; 

   if (shmdt(shmaddr) == -1)
      error = errno;
   if ((shmctl(shmid, IPC_RMID, NULL) == -1) && !error)
      error = errno;
   if (!error)
      return 0;
   errno = error;
   return -1;
}
void sighandeler(int signo){
	int i, error1;
	for(i=0; i<Cp1;++i){
		kill(pidP1[i],SIGINT);
	}
	for(i=0; i<Cp2;++i){
		kill(pidP2[i],SIGINT);
	}
	for(i=0; i<Cp3;++i){
		kill(pidP3[i],SIGINT);
	}
	for (i = 0; i < socketCount; ++i)
	{
		 if (error1 = pthread_join(threadClient[i], NULL)) {
		   Error("Failed to join thread: ");
		  }
	}
	


	fprintf(stderr, "\nCTRL-C has caught IntegralGen is shutting down properly\n");
	
	exit(signo);
}
/* Catch Signal Handler functio */
void signal_callback_handler(int signum){

        printf("Caught signal SIGPIPE %d\n",signum);
        /*exit(1);*/
}
void* thread1(void* value)
{

}
void* thread2(void* value)
{
	
}
void* thread3(void* value)
{
	
}