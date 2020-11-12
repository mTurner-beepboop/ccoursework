#include "date.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
/*
 * Created by James Campbell on 18/10/2020.
 * SP Exercise 1a
 * 2398100c
 * This is my own work as defined in the Academic Ethics agreement I have signed.
 */


// struct to hold the data in the first place
struct date{
    int day;
    int month;
    int year;
};

/*
 * date_create creates a Date structure from `datestr`
 * `datestr' is expected to be of the form "dd/mm/yyyy"
 * returns pointer to Date structure if successful,
 *         NULL if not (syntax error)
 */
Date *date_create(char *datestr){
    Date *d;
    /* iterate over the string to check if he formatting is what we're expecting
     * we're expecting a / in positions 2, and 5, and numbers elsewhere */
    int i;
    for (i = 0; *(datestr + i) != '\0'; i++){
        if (((i == 2 || i == 5) && *(datestr+i) != '/') || ((i != 2 && i != 5) && (isdigit(*(datestr+i)) == 0))){
            fprintf(stderr, "Illegal date input format: %s", datestr);
            return NULL;
        }
    }
    /* use the value of i to make sure the string is the length we're expecting which is 10 */
    if ( i != 10){
        fprintf(stderr, "Illegal date input format: %s", datestr);
        return NULL;
    }

    /* if the string is as expected, then we allocate memory, and if successful, assign values before returning */
    if ((d = (struct date *)malloc(sizeof(struct date))) != NULL) {
       sscanf(datestr,"%d/%d/%d", &(d->day), &(d->month), &(d->year));
    }
    return d;
}

/*
 * date_duplicate creates a duplicate of `d'
 * returns pointer to new Date structure if successful,
 *         NULL if not (memory allocation failure)
 */
Date *date_duplicate(Date *d){
    struct date *duplidate;
    if ((duplidate = (struct date *)malloc(sizeof(struct date))) != NULL) {
        duplidate->day = d->day;
        duplidate->month = d->month;
        duplidate->year = d->year;
    }
    return d;
}

/*
 * date_compare compares two dates, returning <0, 0, >0 if
 * date1<date2, date1==date2, date1>date2, respectively
 */

int date_compare(Date *date1, Date * date2){
    int date1total = date1->year * 10000 + date1->month * 100 + date1->day;
    int date2total = date2->year * 10000 + date2->month * 100 + date2->day;
    return date1total-date2total;

}

void date_destroy(Date *d){
    free(d);
}

