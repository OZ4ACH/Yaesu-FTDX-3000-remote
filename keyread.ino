/************************************************************************************************

Key read
By OZ4ACH Kim Moltved 2016
https://github.com/OZ4ACH

************************************************************************************************/

#include "define.h"
#include "setup.h"


#define KEY_ROW_COUNT (sizeof(KEY_ROW_ADR)/sizeof(word))
#define KEY_COL_COUNT (sizeof(KEY_COL_ADR)/sizeof(word))

#define KEYS_COUNT (sizeof(KEYS_ADR)/sizeof(word))

#define KEYS_START_COUNT (KEY_ROW_COUNT * KEY_COL_COUNT)
#define KEY_TOTAL_COUNT (KEYS_START_COUNT + KEYS_COUNT)

struct key_t {
	bool key;
	bool last;
	bool press;
	bool release;
	bool last_press;
	bool last_release;
	uint32 pressed;
	uint32 last_pressed;
	uint32 time_pressed;
	uint32 released;
	uint32 last_released;
	uint32 time_released;
};

key_t keys[KEY_TOTAL_COUNT];

//word key_row;
//word key_col;




void keyloopreset() {
	for (word c = 0; c < KEY_TOTAL_COUNT; c++) {
        keys[c].press = LOW;
        keys[c].release = LOW;
	}
}


// Init keyboard read
void keyreset() {
	for (word c = 0; c < KEY_TOTAL_COUNT; c++) {
        keys[c].key = LOW;
        keys[c].last = LOW;
        keys[c].press = LOW;
        keys[c].release = LOW;
        keys[c].last_press = LOW;
        keys[c].last_release = LOW;

        keys[c].time_pressed = millis()+1000;
        keys[c].time_released = millis()+1000;
        keys[c].pressed =  0;
        keys[c].last_pressed = 0;
        keys[c].released =  0;
        keys[c].last_released = 0;
	}
}

void readkey_init() {

	// Key pad
	for (word c = 0; c < KEY_ROW_COUNT; c++) {
	    pinMode(KEY_ROW_ADR[c], INPUT);
	}
	for (word c = 0; c < KEY_COL_COUNT; c++) {
	    pinMode(KEY_COL_ADR[c], INPUT_PULLUP);
	}

	// Keys
	for (word c = 0; c < KEYS_COUNT; c++) {
	    pinMode(KEYS_ADR[c], INPUT_PULLUP);
	}

	//
	keyreset();
}


// Scan and Read the keyboard with 3x3 keys
void readkey() {

word keynr;

	for (word kr = 0; kr < KEY_ROW_COUNT; kr++) {
        pinMode(KEY_ROW_ADR[kr], OUTPUT);
        digitalWrite(KEY_ROW_ADR[kr], LOW);

		for (word kc = 0; kc < KEY_COL_COUNT; kc++) {
			keynr = (kr*3) + kc;

	        keys[keynr].last = keys[keynr].key;
	        keys[keynr].key = ! digitalRead(KEY_COL_ADR[kc]);
		}

	    pinMode(KEY_ROW_ADR[kr], INPUT);
	}

	// Read keys
	for (word k = 0; k < KEYS_COUNT; k++) {
		keynr = k + KEYS_START_COUNT;

        keys[keynr].last = keys[keynr].key;
        keys[keynr].key = ! digitalRead(KEYS_ADR[k]);
	}


	for (word keynr = 0; keynr < KEY_TOTAL_COUNT; keynr++ ) {
		if (keys[keynr].key == LOW) {
	        keys[keynr].time_pressed = millis();
	        keys[keynr].pressed = 0;

	        keys[keynr].released = millis() - keys[keynr].time_released;
	        keys[keynr].last_released = keys[keynr].released;
		} else {
	        keys[keynr].time_released = millis();
	        keys[keynr].released = 0;

	        keys[keynr].pressed = millis() - keys[keynr].time_pressed;
	        keys[keynr].last_pressed = keys[keynr].pressed;
		}


		if ((keys[keynr].last_press == LOW) && (keys[keynr].pressed > PRELL_TIME)) {
			keys[keynr].last_release = LOW;
			keys[keynr].last_press = HIGH;
			keys[keynr].press = HIGH;
		}
		if ((keys[keynr].last_release == LOW) && (keys[keynr].released > PRELL_TIME)) {
			keys[keynr].last_press = LOW;
			keys[keynr].last_release = HIGH;
			keys[keynr].release = HIGH;
		}
	}
}


bool key(word keynr) {
	return keys[keynr-1].key;
}

bool keypress(word keynr) {
	bool kt;
	kt = keys[keynr-1].press;
	keys[keynr-1].press = LOW;
	return kt;
}

bool keyrelease(word keynr) {
	bool kt;
	kt = keys[keynr-1].release;
	keys[keynr-1].release = LOW;
	return kt;
}

uint32 keypressed(word keynr) {
	return keys[keynr-1].pressed;
}
uint32 keyreleased(word keynr) {
	return keys[keynr-1].released;
}
uint32 keypressed_last(word keynr) {
	return keys[keynr-1].last_pressed;
}
uint32 keyreleased_last(word keynr) {
	return keys[keynr-1].last_released;
}


