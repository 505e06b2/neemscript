#include "neem.h"

float Neem::dosum(const char *currentchar) {
	char numberbuffer[25];
	char lastop = '+';
	double value = 0;
	int x = 0;
	void (*doop)(char *, int *, char *, double *) = [](char *op, int *x, char *buff, double *val){
		buff[*x] = '\0';
		if(*op == '+') *val += atof(buff);
		else if(*op == '-') *val -= atof(buff);
		else if(*op == '*') *val *= atof(buff);
		else if(*op == '/') *val /= atof(buff);
	};
				
	while(isspace((uint8_t) *currentchar)) currentchar++;
	for(; *currentchar; currentchar++, x++) {
		if(isspace((uint8_t) *currentchar)) { 
			x--;
			continue;
		}
				
		if(*currentchar == '+' || *currentchar == '-' || *currentchar == '*' || *currentchar == '/') {
			doop(&lastop, &x, numberbuffer, &value);
			lastop = *currentchar;
			x = -1;
		} else {
			numberbuffer[x] = *currentchar;
		}
	}
				
	doop(&lastop, &x, numberbuffer, &value);
	return value;
}