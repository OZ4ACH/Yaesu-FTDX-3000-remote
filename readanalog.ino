/************************************************************************************************

Key read
By OZ4ACH Kim Moltved 2016
https://github.com/OZ4ACH

************************************************************************************************/

#include "define.h"
#include "setup.h"

int16 analognew[ANALOGIND];
int16 analogold[ANALOGIND];

uint16 analogchange;
bool analogchangetrig;

void readanalog_init() {
	//analog_count = 0;
	for (word c = 0; c < ANALOGIND; c++) {
		analognew[c] = 0;
		analogold[c] = 0;
	}
	analogchange = 0;
	analogchangetrig = FALSE;
}

void readanalog() {
	analogchangetrig = FALSE;

	for (word c = 0; c < ANALOGIND; c++) {
		analogold[c] = analognew[c];
		analognew[c] = analogRead(c);
		if ( abs(analogold[c] - analognew[c]) > ANALOGCHANGE) {
			analogchangetrig = TRUE;
		}
	}
	if (analogchangetrig) {
		analogchange++;
	} else {
		analogchange = 0;
	}
}

int16 analog(word keynr) {
	return analognew[keynr-1];
}

bool analog_change() {
	return (analogchange > ANALOGTRIG);
}
