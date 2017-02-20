/************************************************************************************************

LED control
By OZ4ACH Kim Moltved 2017
https://github.com/OZ4ACH

************************************************************************************************/

#include "define.h"
#include "setup.h"

#define LEDCOUNT (sizeof(LED_ADR)/sizeof(word))

struct led_t {
	bool light;    // ON / OFF
//	uint32 flash;  // in millisec.
};

led_t led[LEDCOUNT];

void led_init() {
	for (word c = 0; c < LEDCOUNT; c++) {
		led[c].light = OFF;
		//led[c].flash = 0;

	    pinMode(LED_ADR[c], OUTPUT);
    	digitalWrite(LED_ADR[c], ON);
	}
}

void led_update() {
	for (word c = 0; c < LEDCOUNT; c++) {
    	digitalWrite(LED_ADR[c], led[c].light);
	}
}

void led_set(int lednr, bool set) {
	led[lednr-1].light = set;
}

