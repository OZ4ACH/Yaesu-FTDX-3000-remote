/************************************************************************************************

Key read
By OZ4ACH Kim Moltved 2016
https://github.com/OZ4ACH

************************************************************************************************/

#include "define.h"
#include "setup.h"

int16 analognew[ANALOGIND];

void readanalog_init() {
	//analog_count = 0;
	for (word c = 0; c < ANALOGIND; c++) {
		analognew[c] = 0;
	}
}

void readanalog() {
	for (word c = 0; c < ANALOGIND; c++) {
		analognew[c] = analogRead(c);
	}
}

int16 analog(word keynr) {
	return analognew[keynr];
}

