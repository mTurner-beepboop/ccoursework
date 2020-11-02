//Mark Turner, 2386300t, SP Exercise 1A
//This is my own work as defined in the Academic Ethics agreement I have signed

#include "date.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct date {
	unsigned char day; //max value for the data will be 31, hence char, min 1 hence unsigned
	unsigned char month; //max value for data will be 12, hence char, min will be 1, hence unsigned;
	unsigned int year; //year cannot be less than zero, hence unsigned, for long term use, int will cover all possible values where char would not;
};

Date *date_create(char *datestr){
	//Check if the date string has enough letters to be valid
	if (strlen(datestr)!=10){ return NULL; }

	//set up some initial values
	unsigned char day;
	unsigned char month;
	unsigned int year; 
	
	//Parse the string into variables
	int section = 0;
	int pos = 0;
	char temp[5];
	for (int i=0; i<10; i++){
		if (datestr[i] == '/'){
			//log the day
			if (section == 0){
				temp[pos] = '\0'; //Add termination char to string
				day = atoi(temp);
			}
			//log the month
			if (section == 1){
				temp[pos] = '\0';
				month = atoi(temp);
			}
			
			strcpy(temp, ""); //empty the temp char array
			pos = 0; //return to start of temp
			section = section + 1; //iterate section of date
			
			continue;	
		}
		//iterate position in date
		temp[pos] = datestr[i];
		pos = pos + 1;	
	}
	//log the year
	temp[pos] = '\0';
	year = atoi(temp);

	//validate - only the basics at least for now, just trying to get something working
	if (day > 31){ return NULL; }
	if (day < 1){ return NULL; }
	if (month > 12){ return NULL; }
	if (month < 1){ return NULL; }

	//finally allocate the data for the structure and return the pointer
	Date * date_ptr = malloc(sizeof(Date)); //will return null if some problem occurs
	date_ptr->day = day; date_ptr->month = month; date_ptr->year = year;
	return date_ptr;				
}

Date *date_duplicate(Date *d){
	Date * date_ptr = malloc(sizeof(Date));
	date_ptr->day = d->day; date_ptr->month = d->month; date_ptr->year = d->year;
	return date_ptr;
}

int date_compare(Date *date1, Date *date2){
	if (date1->year<date2->year){
		return -1;
	}
	if (date1->year>date2->year){
		return 1;
	}
	//at this point the year is the same
	if (date1->month<date2->month){
		return -1;
	}
	if (date1->month>date2->month){
		return 1;
	}
	//at this point the year and month is the same
	if (date1->day<date2->day){
		return -1;
	}
	if (date1->day>date2->day){
		return 1;
	}
	//at this point the date is the same
	return 0;
}

void date_destroy(Date *d){
	free(d);
}
