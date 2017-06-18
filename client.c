/*
**Created by Stewart Collins - scollins2 326206
**Adapted from client-2.c code by Michael Kirley
*/

#include "client.h"

/*Handles the client connecting for the mastermind game
*/
int main(int argc, char * argv[])
{
	char buffer[512];	
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	int len, numberofbytes, s, server_port, counter;

	//Get host ip and port from arguments
	if(argc==3){
		host = argv[1];
		server_port = atoi(argv[2]);
	}
	else {
		fprintf(stderr, "Usage :client host server_port\n");
		exit(1);
	}

	/* translate host name into peer's IP address ; This is name translation service by the operating system */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "Unknown host %s  \n",host);
		exit(1);
	}
	/* Building data structures for socket */

	bzero( (char *)&sin,sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port =htons(server_port);

	/* Active open */
	/* Preliminary steps: Setup: creation of active open socket*/

	
	if ( (s = socket(AF_INET, SOCK_STREAM, 0 )) < 0 ){
		perror("Error in creating a socket to the server");
		exit(1);
	}

	if(connect(s, (struct sockaddr *)&sin, sizeof(sin))  < 0 ) {
		perror("Error in connecting to the host");
		close(s);
		exit(1);
	}
	
	//Get welcome message
	len=recv(s,&buffer,sizeof(buffer),0);
	printf("%s",buffer);
	
	memset(&buffer,sizeof(buffer),'\0');
	
	//While connection is still open recieve messages from the server
	while(len = recv(s,&buffer,sizeof(buffer),0)){
		//Print message
		printf("%s",buffer);
		bzero(&buffer,sizeof(buffer));
		//Get client guess
		scanf("%c %c %c %c",&buffer[0],&buffer[1],&buffer[2],&buffer[3]);
		buffer[4] = '\n';
		//Clear stdin
		while(getchar() != '\n');
		//Send guess to server
		send(s,&buffer,sizeof(buffer),0);
		bzero(&buffer,sizeof(buffer));
		len = recv(s,&buffer,sizeof(buffer),0);
		//Display results of guess
		if(strncmp(buffer,"INVALID",7)==0){
			printf("Invalid guess entered, please use only the symbols A,B,C,D,E and F\n");	
		}else if(strncmp(buffer,"FAILURE",7)==0){
			printf("You are out of guesses. The mastermind wins. The secret code was %s\n", buffer+8);
			break;
		}else if(strncmp(buffer,"SUCCESS",7)==0){
			printf("Guess Successful, You Are The True Mastermind!\n");
			break;
		}else{
			printf("Right location, right symbol:%c     Right symbol, wrong location:%c\n", buffer[1], buffer[3]);
		}
	}
	//Close socket connection
	close(s);
}

