/*
**Created by Stewart Collins - scollins2 326206
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "formatTime.h"

//Global variables for use in pThreads
char userCode[4];
pthread_mutex_t mutex1;

//Global variables for use in collection usage date
FILE *logFile;
int s;
int numberOfClients;
int numberOfWinningClients;
char* serverStartTime;

struct socketInfo {
	int* socketFD;
	char* ip4;
}socketInfo;



void *mastermind(void *socketStruct);

void finaliseLog();

int main (int argc, char *argv[]);