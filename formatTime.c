/*
**Created by Stewart Collins - scollins2 326206
*/

#include "formatTime.h"

/*Formats the system date time information
**into the format required for the log
**returns a pointer to a string with the formated information
*/
char* formatDateTime(){
	char* dateTime;
	char tempInt[4];
	dateTime = (char*)malloc(sizeof(char[19]));
	assert(dateTime);
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	dateTime[0] = '[';
	//If day is single digit make double digit
	sprintf(tempInt, "%d", tm.tm_mday);
	if(tm.tm_mday < 10){
		dateTime[1] = '0';
		dateTime[2] = tempInt[0];
	}else{
		dateTime[1] = tempInt[0];
		dateTime[2] = tempInt[1];
	}dateTime[3] = ' ';
	sprintf(tempInt, "%d", tm.tm_mon);
	//If month is single digit make double digit
	if(tm.tm_mon < 10){
		dateTime[4] = '0';
		dateTime[5] = tempInt[0];
	}else{
		dateTime[4] = tempInt[0];
		dateTime[5] = tempInt[1];
	}dateTime[6] = ' ';
	sprintf(tempInt, "%d", tm.tm_year);
	//This was to fix an error where year was being generated as 116
	if(tm.tm_year < 1000){
		dateTime[7] = '2';
		dateTime[8] = '0';
		dateTime[9] = tempInt[1];
		dateTime[10] = tempInt[2];
	}else{
		dateTime[7] = '2';
		dateTime[8] = '0';
		dateTime[9] = tempInt[2];
		dateTime[10] = tempInt[3];
	}dateTime[11] = ' ';
	//If hour is signle digit make double digit
	sprintf(tempInt, "%d", tm.tm_hour);
	if(tm.tm_hour < 10){
		dateTime[12] = '0';
		dateTime[13] = tempInt[0];
	}else{
		dateTime[12] = tempInt[0];
		dateTime[13] = tempInt[1];
	}dateTime[14] = ':';
	//If minute is single digit make double digit
	sprintf(tempInt, "%d", tm.tm_min);
	if(tm.tm_min < 10){
		dateTime[15] = '0';
		dateTime[16] = tempInt[0];
	}else{
		dateTime[15] = tempInt[0];
		dateTime[16] = tempInt[1];
	}dateTime[17] = ']';
	dateTime[18] = '\0';
		
	return dateTime;
}