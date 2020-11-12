#include "date.h"
#include <stdio.h>

static char *testString[] = {
	"01/01/1960",
	"31/01/1962",
	"01/02/1964",
	"28/02/1966",
	"01/03/1968",
	"31/03/1970",
	"01/04/1972",
	"30/04/1974",
	"01/05/1976",
	"31/05/1978",
	"01/06/1980",
	"30/06/1982",
	"01/07/1984",
	"31/07/1986",
	"01/08/1988",
	"31/08/1990",
	"01/09/1992",
	"30/09/1994",
	"01/10/1996",
	"31/10/1998",
	"01/11/2000",
	"30/11/2002",
	"01/12/2004",
	"31/12/2006",
};
#define NDATES sizeof(testString)/sizeof(char *)
static Date *date[NDATES];

int main() {
	Date *d1, *d2;
	unsigned i, j;
	int retval = 1;

	d1 = date_create("15/03/2009");
	if (d1 == NULL) {
		printf("ERROR: date_create(\"15/03/2009\") failed\n");
		return retval;
	}
	d2 = date_duplicate(d1);
	if (d2 == NULL) {
		printf("ERROR: date_duplicate() returns NULL\n");
		date_destroy(d1);
		return retval;
	}
	if (date_compare(d1, d2) != 0) {
		printf("ERROR: date_compare() returns non-zero for identical dates\n");
		date_destroy(d2);
		date_destroy(d1);
		return retval;
	}
	date_destroy(d1);
	date_destroy(d2);
	d1 = date_create("bogus string");
	if (d1 != NULL) {
		printf("ERROR date_create(\"bogus string\") returns non-NULL\n");
		date_destroy(d1);
		return retval;
	}
	for (i = 0; i < NDATES; i++) {
		date[i] = date_create(testString[i]);
		if (date[i] == NULL) {
			printf("ERROR: date_create() failed for string %u/%lu\n", i+1, (unsigned long int)NDATES);
			for (j = 0; j < i; j++)
				date_destroy(date[j]);
			return retval;
		}
	}
	for (i = 1; i < NDATES-1; i++) {
		for (j = 0; j < i; j++) {
			if (date_compare(date[j], date[i]) >= 0) {
				printf("ERROR: date_compare() did not return <0\n");
				goto done;
			}
		}
		if (date_compare(date[i], date[i]) != 0) {
			printf("ERROR: date_compare() did not return 0\n");
			goto done;
		}
		for (j = i+1; j < NDATES; j++) {
			if (date_compare(date[j], date[i]) <= 0) {
				printf("ERROR: date_compare() did not return > 0\n");
				goto done;
			}
		}
	}
	retval = 0;
done:
	for (i = 0; i < NDATES; i++)
		date_destroy(date[i]);
	return retval;
}
