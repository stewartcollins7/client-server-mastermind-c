#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char** argv){
	int MAXGUESSES = 10;
	char legalValues[] = {'A','B','C','D','E','F'};
	char secretCode[4];
	int valid;
	time_t t;
	int i, j;
	char guess1,guess2,guess3,guess4;
	int accurateGuesses;
	char guess[4];
	int wrongLocations;
	char checkedCode[4];
	int numOfGuesses;
	
	if(argc > 1){
		for(i=0;i<4;i++){
			secretCode[i] = argv[1][i];
		}	
		i=0;
		valid = 1;
		while(valid && i<4){
			if(j>5){
				valid = 0;
			}else{
				if(secretCode[i] == legalValues[j]){
					i++;
					j=0;
				}else{
					j++;
				}
			}
		}if(!valid){
			printf("Invalid secret code entered. Program terminating");
			exit(1);
		}		
	}else{
		srand((unsigned) time(&t));
		for(i=0;i<4;i++){
		secretCode[i] = legalValues[(rand()%6)];
		}
	}
	fprintf(stdout,"Welcome to Mastermind!\nThe mastermind will choose a secret code from a set of given symbols\n");
	fprintf(stdout,"Your job is to try and guess the code. For each guess you make the mastermind will tell you\n");
	fprintf(stdout,"how many of your guesses are the right symbol and the right location, and how many are\n");
	fprintf(stdout,"the right symbol, but in the wrong location.\n");
	fprintf(stdout,"The valid symbols are: A, B, C, D, E and F\n");
	fprintf(stdout,"You have 10 guesses to try and outwit the mastermind. Good luck!\n");
	
	numOfGuesses = 0;
	while(numOfGuesses<MAXGUESSES){
		fprintf(stdout,"\nPlease enter guess number %d\n", (numOfGuesses+1));
		fflush( stdout );
		fscanf(stdin,"%c%c%c%c", &guess1,&guess2,&guess3,&guess4);
		guess[0] = guess1;
		guess[1] = guess2;
		guess[2] = guess3;
		guess[3] = guess4;
	
		
		for(i=0;i<4;i++){
			fprintf(stdout,"%c",guess[i]);
		}fprintf(stdout,"\n");
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
		}if(!valid){
			printf("Invalid guess entered. Program terminating");
			exit(1);
		}
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
		}if(accurateGuesses == 4){
			fprintf(stdout,"Guess Successful\n");
			exit(1);
		}wrongLocations=0;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				if(guess[i]==checkedCode[j]){
					wrongLocations++;
					guess[i]='z';
					checkedCode[j]='x';
					break;
				}
			}
		}
		fprintf(stdout,"Accurate:%d  Inaccurate:%d", accurateGuesses, wrongLocations);
		numOfGuesses++;
	}fprintf(stdout,"You failed to beat the mastermind. The secret code was:");
	for(i=0;i<4;i++){
		fprintf(stdout,"%c",secretCode[i]);
	}fprintf(stdout,"\n");

	
	return 0;
}