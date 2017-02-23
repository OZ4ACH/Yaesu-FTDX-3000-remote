/************************************************************************************************

Yaesu-FTDX-3000-remote
Remote control for the Yaesu FTDX-3000 usinge a ARDUINO MEGA with Atmega2560.
By OZ4ACH Kim Moltved 2016
https://github.com/OZ4ACH

It also control the tuner a MFJ-998.

The keyboard is a 3x3 matrix with scan.

Using a 20x4 LCD display with a I2C control board.

There is 9 pot connected to the analoginput to adjust value direct.

You can control all setting there can be controled from the RS-232.

************************************************************************************************/

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // https://bitbucket.org/fmalpartida/new-liquidcrystal/overview
#include "define.h"
#include "setup.h"



// Main state
word main_state;
uint32 main_time;
#define STATE_BOOT 1
#define STATE_INFO 2
#define STATE_ANALOG 3

#define STATE_LIMIT 99

#define STATE_MENU 100
#define STATE_TUNE 110
#define STATE_AUTOTUNE 111


int16 menu_point;

uint16 readradionr;
// Radio value
byte PWRPROC;
byte VOX;
byte PROC;
byte MICEQ;
byte RFSQL;
byte DNR;
byte DNF;

#define RADIOTXREQ  5
#define TUNERKEY   6
#define TUNERSTART 7
#define TXBYPASS 4



int32 LASTTUNEFRQ;
bool TUNEFAIL;
int32 TXFRQ;
int TXPOWER;
int TUNEPOWER;

#define MAXTUNETIME (uint32)20000
#define MAXTUNETIMEOUT (uint32)25000
#define MAXSWRMETER 54

#define TUNE_OK 1
#define TUNE_TIMEOUT 2
#define TUNE_TUNING 3
#define TUNE_TUNED 4


// TUNE
void tune(bool testswr) {
	uint32 starttime;
	uint16 status;
	uint16 swr;
	String radioread;

	lcd.setCursor(0,2);
	lcd.print("TUNE BEGIN          ");

	digitalWrite(TXBYPASS, LOW);
    delay(50);

	LASTTUNEFRQ = TXFRQ;
	TUNEFAIL = TRUE;

	starttime = millis();

	if (testswr) {
		// IS there need for tune
		lcd.setCursor(0,2);
		lcd.print("TUNE TEST SWR       ");

		digitalWrite(RADIOTXREQ, LOW);
		delay(400);

		Serial1.print("RM6;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(3,7);
		swr = radioread.toInt();

		digitalWrite(RADIOTXREQ, HIGH);

		if (swr < MAXSWRMETER) {
			lcd.setCursor(0,2);
			lcd.print("TUNE SWR OK         ");
			lcd.setCursor(16,2);
			lcd.print(swr);

			TUNEFAIL = FALSE;
			return;
		}

		delay(500);
	}

	// Start tune
    digitalWrite(TUNERSTART, LOW);

 	lcd.setCursor(0,2);
	lcd.print("TUNE START          ");

    delay(600);
    digitalWrite(TUNERSTART, HIGH);

	// Wait for tuner to tune
	status = TUNE_TUNING;
	while (status == TUNE_TUNING) {
		if (MAXTUNETIME < (millis() - starttime)) {
			status = TUNE_TIMEOUT;
		}
		if (digitalRead(TUNERKEY) == LOW) {
			status = TUNE_TUNED;
		}
	}

	// Tune TX req if tuber req.
	if (digitalRead(TUNERKEY) == LOW){
		digitalWrite(RADIOTXREQ, LOW);

		lcd.setCursor(0,2);
		lcd.print("TUNE RUNNING        ");

		status = TUNE_TUNING;
		while (status == TUNE_TUNING) {
			if (MAXTUNETIME < (millis() - starttime)) {
				status = TUNE_TIMEOUT;
			}
			if (digitalRead(TUNERKEY) == HIGH) {
				status = TUNE_TUNED;
			}
			lcd.setCursor(0,1);
			lcd.print((millis()-starttime));
			lcd.print("            ");
		}

		digitalWrite(RADIOTXREQ, HIGH);

		lcd.setCursor(0,2);
		lcd.print("TUNE END            ");
	}

	// timerout wait for tuner to stop TX req then exit
	if (status == TUNE_TIMEOUT) {
		lcd.setCursor(0,2);
		lcd.print("TUNE TIMEOUT        ");

		status = TUNE_TUNING;
		while (status == TUNE_TUNING) {
			if (MAXTUNETIMEOUT < (millis() - starttime)) {
				status = TUNE_TIMEOUT;
			}
			if (digitalRead(TUNERKEY) == HIGH) {
				status = TUNE_TUNED;
			}
			lcd.setCursor(0,1);
			lcd.print((millis()-starttime));
			lcd.print("            ");
		}
		return;
	}

	// Test SWR
	lcd.setCursor(0,2);
	lcd.print("TUNE TEST SWR       ");

	digitalWrite(RADIOTXREQ, LOW);
	delay(400);

	Serial1.print("RM6;");
	radioread = Serial1.readStringUntil(';');
	radioread = radioread.substring(3,7);
	swr = radioread.toInt();

	digitalWrite(RADIOTXREQ, HIGH);


	if (swr >= MAXSWRMETER) {
		lcd.setCursor(0,2);
		lcd.print("TUNE SWR FAIL       ");
		lcd.setCursor(16,2);
		lcd.print(swr);
		return;
	}

	lcd.setCursor(0,2);
	lcd.print("TUNE SWR OK         ");
	lcd.setCursor(16,2);
	lcd.print(swr);

	TUNEFAIL = FALSE;
}




void readradio() {
	String radioread;

	readradionr++;
	switch (readradionr) {
		case 1:
			Serial1.print("VX;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("VX0")) {
				VOX = 0;
				} else {
				VOX = 1;
			}
			break;
		case 2:
			Serial1.print("PR0;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("PR00")) {
				PROC = 0;
				} else {
				PROC = 1;
			}
			break;
		case 3:
			Serial1.print("PR1;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("PR10")) {
				MICEQ = 0;
				} else {
				MICEQ = 1;
			}
			break;
		case 4:
			Serial1.print("BC0;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("BC00")) {
				DNF = 0;
				} else {
				DNF = 1;
			}
			break;
		case 5:
			Serial1.print("NR0;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("NR00")) {
				DNR = 0;
				} else {
				DNR = 1;
			}
			break;
		case 6:
			Serial1.print("EX177;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("EX1770")) {
				PWRPROC = 0;
				} else {
				PWRPROC = 1;
			}
			break;
		case 7:
			Serial1.print("EX036;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("EX0360")) {
				RFSQL = 0;
				} else {
				RFSQL = 1;
			}
			break;
		case 8:
			Serial1.print("FA;");
			radioread = Serial1.readStringUntil(';');
			radioread = radioread.substring(2,11);
			TXFRQ = radioread.toInt();
			break;
		case 9:
			Serial1.print("PC;");
			radioread = Serial1.readStringUntil(';');
			radioread = radioread.substring(2,6);
			TXPOWER = radioread.toInt();
			break;
		case 10:
			Serial1.print("EX178;");
			radioread = Serial1.readStringUntil(';');
			radioread = radioread.substring(5,6);
			TUNEPOWER = radioread.toInt();


		default:
			readradionr = 0;
	}

}




// Setup
void setup() {

    pinMode(RADIOTXREQ, OUTPUT);
    pinMode(TUNERKEY, INPUT_PULLUP);
    pinMode(TUNERSTART, OUTPUT);
    pinMode(TXBYPASS, OUTPUT);

    // Tuner default
    digitalWrite(TUNERSTART, HIGH);

    // Radio default
    digitalWrite(RADIOTXREQ, HIGH);

    // TX Rele Bypass default
    digitalWrite(TXBYPASS, LOW);



	led_init();
	readanalog_init();
	readkey_init();

	lcd.begin(20,4);
	lcd.backlight();
	lcd.clear();
	lcd.home();
	lcd.setCursor(0,0);

	main_state = STATE_BOOT;
	main_time = millis();

	menu_point = 1;

	// Set serial port
	Serial.begin(38400);
	Serial1.begin(38400);
	Serial2.begin(38400);
	Serial3.begin(38400);

	// Set time out
	Serial.setTimeout(100);
	Serial1.setTimeout(100);
	Serial2.setTimeout(100);
	Serial3.setTimeout(100);

	readradionr = 0;
}


// Loop
void loop() {
	String radioread;

	readradio();

	led_update();
	readanalog();
	readkey();

	// Global state change
	if (keypress(KEY_VOX)) {
		if (VOX == 0) {
			Serial1.print("VX1;");
		} else {
			Serial1.print("VX0;");
		}
	} else
	if (keypress(KEY_PROC)) {
		if (PROC == 0) {
			Serial1.print("PR01;");
		} else {
			Serial1.print("PR00;");
		}
	} else
	if (keypress(KEY_12)) {
	} else
	if (keypress(KEY_13)) {
	} else
	if (keypress(KEY_14)) {
	} else
	if (keypress(KEY_15)) {
	} else
	if (keypress(KEY_DNR)) {
		if (DNR == 0) {
			Serial1.print("NR01;");
		} else {
			Serial1.print("NR00;");
		}
	} else
	if (keypress(KEY_DNF)) {
		if (DNF == 0) {
			Serial1.print("BC01;");
		} else {
			Serial1.print("BC00;");
		}
	} else

	if ((main_time + MAIN_TIMEOUT) < millis()) {
		main_state = STATE_INFO;
		main_time = millis();
	} else

	if (keypress(KEY_FOODSWITCH)) {
		Serial1.print("MX1;");
	} else
	if (keyrelease(KEY_FOODSWITCH)) {
		Serial1.print("MX0;");
	} else

	if (main_state < STATE_LIMIT) {
		if (keypress(KEY_ENTER)) {
			digitalWrite(RADIOTXREQ, LOW);
		} else
		if (keyrelease(KEY_ENTER)) {
			digitalWrite(RADIOTXREQ, HIGH);
		} else
		if (keypress(KEY_TUNE)) {
			main_state = STATE_TUNE;
			main_time = millis();
		} else
		if (analog_change()) {
			main_state = STATE_ANALOG;
			main_time = millis();
		} else
		if (keypress(KEY_ESC)) {
			main_state = STATE_INFO;
			main_time = millis();
		}
		if (keypress(KEY_MENU_UP) || keypress(KEY_MENU_DOWN)) {
			main_state = STATE_MENU;
			main_time = millis();
		}
	}



	switch (main_state) {

		case STATE_BOOT:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("  REMOTE BY OZ4ACH  ");
			lcd.setCursor(0,1);
			lcd.print("  YAESU  FTDX-3000  ");
			lcd.setCursor(0,2);
			lcd.print("                    ");
			lcd.setCursor(0,3);
			lcd.print(__DATE__);
			lcd.print(" ");
			lcd.print(__TIME__);
			if ((main_time + MAIN_TIMEOUT) < millis()) {
				main_state = STATE_INFO;
			}
			break;

		case STATE_INFO:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("INFO                ");
			lcd.setCursor(0,1);
			if (DNR == 0) lcd.print("    "); else lcd.print("DNR ");
			if (DNF == 0) lcd.print("    "); else lcd.print("DNF ");
			lcd.print("            ");
			lcd.setCursor(0,2);
			lcd.print("                    ");
			lcd.setCursor(0,3);
			lcd.print("                    ");

			break;

		case STATE_ANALOG:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("ANALOG              ");

			lcd.setCursor(0,1);
			lcd.print(analog(1));lcd.print("  ");
			lcd.setCursor(0,2);
			lcd.print(analog(2));lcd.print("  ");
			lcd.setCursor(0,3);
			lcd.print(analog(3));lcd.print("  ");

			lcd.setCursor(7,1);
			lcd.print(analog(4));lcd.print("  ");
			lcd.setCursor(7,2);
			lcd.print(analog(5));lcd.print("  ");
			lcd.setCursor(7,3);
			lcd.print(analog(6));lcd.print("  ");

			lcd.setCursor(14,1);
			lcd.print(analog(7));lcd.print("  ");
			lcd.setCursor(14,2);
			lcd.print(analog(8));lcd.print("  ");
			lcd.setCursor(14,3);
			lcd.print(analog(9));lcd.print("  ");

			break;

		case STATE_MENU:
			if (menu_point < 1) menu_point = 1;
			if (menu_point > MENU_COUNT) menu_point = MENU_COUNT;

			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("MENU                ");

			lcd.setCursor(0,1);
			lcd.print(menu_point);
			lcd.print("       ");
			switch (menu_point) {
				case 1:
					lcd.setCursor(0,2);
					lcd.print("AUTOTUNE            ");
					lcd.setCursor(0,3);
					lcd.print("Tune the band       ");
					if (keypress(KEY_ENTER)) {
						main_state = STATE_AUTOTUNE;
						main_time = millis();
					}
					break;
				case 2:
					lcd.setCursor(0,2);
					lcd.print("RF/SQL              ");
					lcd.setCursor(0,3);

					if (RFSQL == 0) {
							    // 12345678901234567890
						lcd.print("RF                  ");
					} else {
						lcd.print("SQL                 ");
					}

					if (keypress(KEY_VALUE_UP)) {
						main_time = millis();
						Serial1.print("EX0361;");
					}
					if (keypress(KEY_VALUE_DOWN)) {
						main_time = millis();
						Serial1.print("EX0360;");
					}
					break;
				case 3:
					lcd.setCursor(0,2);
					lcd.print("PWR/PROC            ");
					lcd.setCursor(0,3);

					if (PWRPROC == 0) {
							    // 12345678901234567890
						lcd.print("TX PWR              ");
					} else {
						lcd.print("PROC                ");
					}

					if (keypress(KEY_VALUE_UP)) {
						main_time = millis();
						Serial1.print("EX1771;");
					}
					if (keypress(KEY_VALUE_DOWN)) {
						main_time = millis();
						Serial1.print("EX1770;");
					}
					break;


				default:
					lcd.setCursor(0,2);
					lcd.print("Default menu        ");
					lcd.setCursor(0,3);
					lcd.print("No menu point!      ");
			}

			if (keypress(KEY_MENU_UP)) {
				main_time = millis();
				menu_point--;
			}
			if (keypress(KEY_MENU_DOWN)) {
				main_time = millis();
				menu_point++;
			}
			if (keypress(KEY_ESC)) {
				main_state = STATE_INFO;
				main_time = millis();
			}
			break;

		case STATE_AUTOTUNE:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("AUTOTUNE            ");
			lcd.setCursor(0,1);
			lcd.print("                    ");
			lcd.setCursor(0,2);
			lcd.print("                    ");
			lcd.setCursor(0,3);
			lcd.print("                    ");
			if (keypress(KEY_ESC)) {
				main_state = STATE_INFO;
				main_time = millis();
			}
			break;

		case STATE_TUNE:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("TUNE                ");
			lcd.setCursor(0,1);
			lcd.print("                    ");
			lcd.setCursor(0,2);
			lcd.print("                    ");
			lcd.setCursor(0,3);
			lcd.print("                    ");

			tune(TRUE);

			main_state = STATE_INFO;
			main_time = millis();
			break;

		default:
			main_state = STATE_BOOT;
			main_time = millis();
			keyreset();
	}



	led_set(1,VOX);
	led_set(2,PROC);
//	led_set(3,key(1));
//	led_set(4,key(4));
//	led_set(5,key(5));
//	led_set(6,key(6));



	// Send data to and from radio and PC
	if (Serial.available() >= 3) {
		radioread = Serial.readStringUntil(CR);
		// Filter MD = mode from the PC command
		if (!radioread.substring(0,2).equals("MD")) {
			Serial1.print(radioread);
			radioread = Serial1.readStringUntil(CR);
			Serial.print(radioread);
		}
	}

	if (Serial2.available() >= 3) {
		radioread = Serial2.readStringUntil(CR);
		Serial1.print(radioread);
		radioread = Serial1.readStringUntil(CR);
		Serial2.print(radioread);
	}

	if (Serial3.available() >= 3) {
		radioread = Serial3.readStringUntil(CR);
		Serial1.print(radioread);
		radioread = Serial1.readStringUntil(CR);
		Serial3.print(radioread);
	}





//	led_set(1,keypress(1));
//	led_set(2,keyrelease(1));
//	led_set(3,key(1));
//	led_set(4,key(4));
//	led_set(5,key(5));
//	led_set(6,key(6));
//
//
//	lcd.setCursor(0,0);
//	lcd.print(analog(0));
//	lcd.print("        ");
//	lcd.setCursor(0,1);
//	lcd.print(analog(1));
//	lcd.print("        ");
//	lcd.setCursor(0,2);
//	lcd.print(analog(2));
//	lcd.print("        ");
//	lcd.setCursor(0,3);
//	lcd.print(1);
//	lcd.print("        ");


}