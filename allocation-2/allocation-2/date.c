/*
 * David MacNeill
 * 2381795m
 * SP Exercise 1a
 * This is my own work as defined in the Academic
 * Ethics agreement I have signed.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include <string.h>

struct date{ int day; int month; int year;};

Date *date_create(char *datestr){
	if(datestr[2]=='/' && datestr[5]=='/'){
		Date * date_ptr = malloc(sizeof(struct date));
		int day = (datestr[0]*10)+(datestr[1]);
		int month = (datestr[3]*10)+(datestr[4]);
		int year = (datestr[6]*1000)+(datestr[7]*100)+(datestr[8]*10)+(datestr[9]);
		date_ptr->day = day; 
		date_ptr->month = month;
		date_ptr->year = year;
		return date_ptr;
	}
	else{
		return NULL;
	}
}

Date *date_duplicate(Date *d){
	Date * d_copy;
	if((d_copy = malloc(sizeof(struct date)))!=NULL){
		d_copy->day = d->day;
		d_copy->month = d->month;
		d_copy->year = d->year;
		return d_copy;
	}
	else{return NULL;}

}

int date_compare(Date *date1, Date *date2){
	if ((date1->year)==(date2->year)){
		if((date1->month)==(date2->month)){
			return ((date1->day)-(date2->day));
		}
		else{
		return ((date1->month)-(date2->month));
		}
	}
	else{
		return ((date1->year)-(date2->year));
	}
}

void date_destroy(Date *d){
	free(d);
}
