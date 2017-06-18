/*
**Created by Stewart Collins - scollins2 326206
**Adapted from server-2.c by Michael Kirley
*/

#include "server.h"

/*The main method sets up a listening socket
**and uses pthreads to deal with concurrent clients
*/
int main (int argc, char *argv[])
{
	
	struct sockaddr_in server, client;
	char legalValues[] = {'A','B','C','D','E','F'};
	int i, j;
	int valid;
	int len;
	struct socketInfo* socketI;
	int new_s, count,server_port;
	char* dateTime;
	char ip4[INET_ADDRSTRLEN];
	pthread_t thread_id;
	int* socketFD;
	char* ip4Copy;

	//Initialise mutex
	pthread_mutex_init(&mutex1,NULL);
	
	//Create log file
	logFile = fopen("log.txt","w");
	if(logFile == NULL){
		printf("Error creating log file\n");
		exit(1);
	}serverStartTime = formatDateTime();

	//On ctrl-c, close log file
	signal(SIGINT,finaliseLog);
	
	//If user code not entered then set to default
	if(argc==2){
		server_port = atoi(argv[1]);
		for(i=0;i<4;i++){
			userCode[i] = 'x';
		}
	}//Otherwise check if valid
	else if(argc==3){
		server_port = atoi(argv[1]);
		for(i=0;i<4;i++){
			userCode[i] = argv[2][i];
		}
		i=0;
		valid = 1;
		j=0;
		while(valid && i<4){
			if(j>5){
				valid = 0;
			}else{
				//If valid copy to global userCode for use by pthreads
				if(userCode[i] == legalValues[j]){
					i++;
					j=0;
				}else{
					j++;
				}
			}
		}if(!valid){
			printf("Invalid secret code entered. Program terminating\n");
			exit(1);
		}//Write user code to log
		pthread_mutex_lock(&mutex1);
		dateTime = formatDateTime();		
		fprintf(logFile, "%s(0.0.0.0) user set secret = %c%c%c%c\n", dateTime, userCode[0],userCode[1],userCode[2],userCode[3]);
		free(dateTime);
		pthread_mutex_unlock(&mutex1);
	}else{
		fprintf(stderr, "Usage :server [port_number] [default_secret_code]\n");
		exit(1);
	}
	
	//Wipe socket structures
	memset (&server,0, sizeof (server));
	memset (&client,0, sizeof (client));
	 
	//Set server connection settings
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons (server_port); 

	//Create listening socket
	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0){
      perror ("Error creating socket");
      exit (1);
	}

	//Bind the socket to local address
	if (bind (s, (struct sockaddr *) &server, sizeof (server)) < 0){
      perror ("Error in binding to the specified port");
      exit (1);
	}

	//Sets the maximum number of pending connections to be allowed
	if ( listen (s, 30) < 0){
        perror("listen() failed with error");
        exit(1);
	}
	else{
		printf("Listening on port %d...\n", ntohs(server.sin_port));
	}
	
	len=sizeof(client);

	numberOfClients=0;
	numberOfWinningClients=0;
	
	//Loop for accepting new clients, will continue until program terminated with ctrl-c or ctrl-z
	while((new_s = accept (s, (struct sockaddr *) &client, &len)))
	{
		//Collect sage data on clients
		numberOfClients++;
		
		//Turn address into ip4 string for log
		inet_ntop(AF_INET,&(client.sin_addr), ip4, INET_ADDRSTRLEN);
		printf("connection accepted from client %s\n",ip4);
		
		//Write connection to log
		dateTime = formatDateTime();
		pthread_mutex_lock(&mutex1);		
		fprintf(logFile, "%s(%s)(%d) client connected\n", dateTime, ip4, new_s);
		free(dateTime);
		pthread_mutex_unlock(&mutex1);
		
		//Malloc free space for new socketFD and ip4 information
		//not sure if this is necessary but read somewhere if this
		//is not done could create a race condition with multiple
		//concurrent clients connecting
		socketFD = (int*)malloc(sizeof(int));
		assert(socketFD);
		*socketFD = new_s;
		ip4Copy = (char*)malloc(sizeof(ip4));
		strcpy(ip4Copy,ip4);
		socketI = (struct socketInfo*)malloc(sizeof(socketInfo));
		socketI->socketFD = socketFD;
		socketI->ip4 = ip4Copy;
		
		//Create new client thread
		if( pthread_create( &thread_id , NULL ,  mastermind, (void*)socketI) < 0)
		{
			perror("could not create thread");
			return 1;
		}//Free thread resources once complete
		pthread_detach(thread_id);
		
	}
	//Close listening connection, should not be able to be reached unless error accepting
	close(s);

	return 1;
}

/*Runs the mastermind program
**socketStruct - contains the information about the client the game is communicating with
*/
void *mastermind(void *socketStruct){ 
	char buffer[512];
	//The welcome message displayed at the start of the game
	char welcomeMessage[] = "Welcome to Mastermind!\nThe mastermind will choose a secret code from a set of given symbols\nYour job is to try and guess the code. For each guess you make the mastermind\nwill tell you how many of your guesses are the right symbol and the right\nlocation, and how many are the right symbol, but in the wrong location.\nThe valid symbols are: A, B, C, D, E and F\nYou have 10 guesses to try and outwit the mastermind. Good luck\n";
	int MAXGUESSES = 10;
	//The legal symbols for the game
	char legalValues[] = {'A','B','C','D','E','F'};
	int valid;
	//Used for generating random numbers
	time_t t;
	int i, j;
	//Array for holding the codes and guesses
	char secretCode[4];
	char checkedCode[4];
	char guess[4];
	//Counter for the number of accurate guesses
	int accurateGuesses;
	//The amount of symbols in the wrong location
	int wrongLocations;
	//The number of guesses made
	int numOfGuesses;
	//Information about the current connection
	struct socketInfo *socket = (struct socketInfo *)socketStruct;
	int socketFD = *((int*)socket->socketFD);
	//The formated date/time for the log
	char* dateTime;
	
	
	//Send the welcome message to client
	send(socketFD,&welcomeMessage,strlen(welcomeMessage),0);
	
	//Use the user code if set, otherwise generate random code
	if(userCode[0]=='x'){
		srand((unsigned) time(&t));
		for(i=0;i<4;i++){
			secretCode[i] = legalValues[(rand()%6)];
		}dateTime = formatDateTime();
		//Write the random code to the log
		pthread_mutex_lock(&mutex1);
		fprintf(logFile, "%s(0.0.0.0) server secret = %c%c%c%c\n", dateTime, secretCode[0],secretCode[1],secretCode[3],secretCode[4]);
		pthread_mutex_unlock(&mutex1);
		free(dateTime);
	}else{
		for(i=0;i<4;i++){
			secretCode[i]=userCode[i];	
		}
	}
	
	
	numOfGuesses = 0;
	accurateGuesses = 0;	
	//Allow the client to make guesses until correct or out of guesses 
	while(numOfGuesses<MAXGUESSES && accurateGuesses<4){
		numOfGuesses++;
		//Wipe the buffer
		bzero(&buffer, sizeof(buffer));
		//Send the game instructions
		sprintf(buffer, "\nPlease enter guess number %d\n", numOfGuesses);
		send(socketFD,&buffer,sizeof(buffer),0);
		bzero(&buffer, sizeof(buffer));
		//Get the guess from the client
		recv(socketFD,&buffer,sizeof(buffer),0);
		//Write the guess to the log
		dateTime = formatDateTime();
		pthread_mutex_lock(&mutex1);
		fprintf(logFile, "%s(%s)(soc_id %d) client's guess = %c%c%c%c\n", dateTime, socket->ip4, socketFD, buffer[0],buffer[1],buffer[2],buffer[3]);
		pthread_mutex_unlock(&mutex1);
		free(dateTime);
		//Read in the guess
		for(i=0;i<4;i++){
			guess[i] = buffer[i];
		}
		//Check whether guess is valid
		i=0;
		j=0;
		valid = 1;
		while(valid && i<4){
			if(j>5){
				valid = 0;
			}else{
				if(guess[i] == legalValues[j]){
					i++;
					j=0;
				}else{
					j++;
				}
			}
		}//Check for accurate locations
		if(valid){
			for(i=0;i<4;i++){
				checkedCode[i] = secretCode[i];
			}
			accurateGuesses = 0;
			for(i=0;i<4;i++){
				if(guess[i] == checkedCode[i]){
					checkedCode[i] = 'x';
					guess[i] = 'y';
					accurateGuesses++;
				}
			}//Check for symbols in the wrong location
			wrongLocations=0;
			for(i=0;i<4;i++){
				for(j=0;j<4;j++){
					if(guess[i]==checkedCode[j]){
						wrongLocations++;
						guess[i]='z';
						checkedCode[j]='x';
						break;
					}
				}
			}//If game not over send guess results
			if(numOfGuesses<MAXGUESSES && accurateGuesses<4){
				sprintf(buffer, "[%d:%d]\n", accurateGuesses, wrongLocations);
				send(socketFD,&buffer,sizeof(buffer),0);
				dateTime = formatDateTime();
				pthread_mutex_lock(&mutex1);
				fprintf(logFile, "%s(0.0.0.0) server's hint = %c%c%c%c%c\n", formatDateTime(), buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
				pthread_mutex_unlock(&mutex1);
				free(dateTime);
			}
			
		}else{
			//If guess invalid send invalid message
			if(numOfGuesses<MAXGUESSES){
				sprintf(buffer, "INVALID\n");
				send(socketFD,&buffer,sizeof(buffer),0);
				dateTime = formatDateTime();
				pthread_mutex_lock(&mutex1);
				fprintf(logFile, "%s(0.0.0.0) INVALID move not valid\n", dateTime);
				pthread_mutex_unlock(&mutex1);
				free(dateTime);
			}
			
		}
		
	}//Wipe the buffer
	bzero(&buffer, sizeof(buffer));
	//If success or failure send appropriate message
	if(accurateGuesses == 4){
		sprintf(buffer, "SUCCESS\n");
		dateTime = formatDateTime();
		pthread_mutex_lock(&mutex1);
		fprintf(logFile, "%s(0.0.0.0) SUCCESS game over\n", dateTime);
		pthread_mutex_unlock(&mutex1);
		free(dateTime);
		numberOfWinningClients++;
	}else{
		sprintf(buffer, "FAILURE\n%c%c%c%c\n", secretCode[0],secretCode[1],secretCode[2],secretCode[3]);
		dateTime = formatDateTime();
		pthread_mutex_lock(&mutex1);
		fprintf(logFile, "%s(0.0.0.0) FAILURE game over\n", dateTime);
		pthread_mutex_unlock(&mutex1);
		free(dateTime);
	}send(socketFD,&buffer,sizeof(buffer),0);
	
	//Close client socket and free socket information
	close(socketFD);
	free(socket->ip4);
	free(socket->socketFD);
	free(socket);
	return NULL;
}

/*Finalises the log information on closure of the server program
*/
void finaliseLog(){
	struct rusage selfUsageData;
	struct rusage childrenUsageData;
	getrusage(RUSAGE_SELF,&selfUsageData);
	if(numberOfClients>0){
		getrusage(RUSAGE_CHILDREN,&childrenUsageData);
	}char* serverEndTime = formatDateTime();
	
	pthread_mutex_lock(&mutex1);
	fprintf(logFile, "\n\nSession Data\n\n");
	fprintf(logFile, "Total Number Of Clients:%d\nNumber Of Winning Clients:%d\n",numberOfClients, numberOfWinningClients);
	fprintf(logFile, "Server started:%s    Server closed:%s\n", serverStartTime, serverEndTime);
	fprintf(logFile, "Server Process Information\nNumber of voluntary context switches:%ld   Number of involuntary context switches:%ld\n", selfUsageData.ru_inblock, selfUsageData.ru_oublock, selfUsageData.ru_nvcsw, selfUsageData.ru_nivcsw); 
	if(numberOfClients>0){
		fprintf(logFile, "Child Process Information\nNumber of voluntary context switches:%ld   Number of involuntary context switches:%ld\n", childrenUsageData.ru_inblock, childrenUsageData.ru_oublock, childrenUsageData.ru_nvcsw, childrenUsageData.ru_nivcsw);
	}fclose(logFile);
	pthread_mutex_unlock(&mutex1);
	close(s);
	exit(1);	
}




