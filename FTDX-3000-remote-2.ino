/************************************************************************************************

Yaesu-FTDX-3000-remote
Remote control for the Yaesu FTDX-3000 usinge a ARDUINO MEGA with Atmega2560.
By OZ4ACH Kim Moltved 2016
https://github.com/OZ4ACH

It also control the tuner a MFJ-998.

The keyboard is a 3x3 matrix with scan.

Using a 16x2 LCD display with a I2C control board.

There is 9 pot connected to the analoginput to adjust value direct.

You can control all setting there can be controled from the RS-232.

************************************************************************************************/

#include <Wire.h>
#include <LiquidCrystal_I2C.h> // https://bitbucket.org/fmalpartida/new-liquidcrystal/overview
#include "define.h"


// LCD setting and variable
LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address


// Analog setting and variable
#define ANALOGIND 9
int16 analognew[ANALOGIND];
int16 analogold[ANALOGIND];
#define ANALOGCHANGE 2


// Keyboard setting, IO and variable
// Pins
#define KEYC1 11
#define KEYC2 12
#define KEYC3 13
#define KEYR1 10
#define KEYR2 9
#define KEYR3 8

#define KEYRMAX 3

uint16 keyR;
#define KEYCOUNT 20
byte key[KEYCOUNT];
byte keylast[KEYCOUNT];
byte keypress[KEYCOUNT];
byte keyrelease[KEYCOUNT];

// Key number
#define SENDTORADIO_VOX 1
#define SENDTORADIO_PROC 2
#define SENDTORADIO_MICEQ 3
#define SENDTORADIO_PWRPROC 4
#define SENDTORADIO_RFSQL 5
#define REQTXKEY 6
#define SENDTORADIO_DNR 7
#define SENDTORADIO_DNF 8
#define TUNEKEY 9


// Radio and Tuner interface
// Pins
#define RADIOTXREQ  5
#define TUNERKEY   6
#define TUNERSTART 7

long readradio;
#define READRADIOCOUNT 40
#define READRADIONOW 1
String radioread;


// Radio value
byte PWRPROC;
byte VOX;
byte PROC;
byte MICEQ;
byte RFSQL;
byte DNR;
byte DNF;

int VOXLEVEL;
int VOXANTI;
int VOXTIME;
int CONTOURLEVEL;
int CONTOURWIDTH;
int DNRNR;
int DVSRX;
int MONITOR;
int NBLEVEL;


// Scan and Read the keyboard with 3x3 keys
void readkey() {
    switch (keyR) {
        case 1:
            keyR = 2;
            pinMode(KEYR1, OUTPUT);
            pinMode(KEYR2, INPUT);
            pinMode(KEYR3, INPUT);
            digitalWrite(KEYR1, LOW);
            for(byte C = 1; C<=3; C++){
                keylast[C] = key[C];
            }
            if (digitalRead(KEYC1) == LOW) {key[1] = HIGH;} else {key[1] = LOW;}
            if (digitalRead(KEYC2) == LOW) {key[2] = HIGH;} else {key[2] = LOW;}
            if (digitalRead(KEYC3) == LOW) {key[3] = HIGH;} else {key[3] = LOW;}
			for(byte C = 1; C<=3; C++){
				if ((keylast[C] == LOW) && (key[C] == HIGH)) {
					keypress[C] = HIGH;
				}
			}
			for(byte C = 1; C<=3; C++){
				if ((keylast[C] == HIGH) && (key[C] == LOW)) {
					keyrelease[C] = HIGH;
				}
			}
            break;
        case 2:
            keyR = 3;
            pinMode(KEYR1, INPUT);
            pinMode(KEYR2, OUTPUT);
            pinMode(KEYR3, INPUT);
            digitalWrite(KEYR2, LOW);
            for(byte C = 4; C<=6; C++){
                keylast[C] = key[C];
            }
            if (digitalRead(KEYC1) == LOW) {key[4] = HIGH;} else {key[4] = LOW;}
            if (digitalRead(KEYC2) == LOW) {key[5] = HIGH;} else {key[5] = LOW;}
            if (digitalRead(KEYC3) == LOW) {key[6] = HIGH;} else {key[6] = LOW;}
			for(byte C = 4; C<=6; C++){
				if ((keylast[C] == LOW) && (key[C] == HIGH)) {
					keypress[C] = HIGH;
				}
			}
			for(byte C = 4; C<=6; C++){
				if ((keylast[C] == HIGH) && (key[C] == LOW)) {
					keyrelease[C] = HIGH;
				}
			}
            break;
        case 3:
            keyR = 1;
            pinMode(KEYR1, INPUT);
            pinMode(KEYR2, INPUT);
            pinMode(KEYR3, OUTPUT);
            digitalWrite(KEYR3, LOW);
            for(byte C = 7; C<=9; C++){
                keylast[C] = key[C];
            }
            if (digitalRead(KEYC1) == LOW) {key[7] = HIGH;} else {key[7] = LOW;}
            if (digitalRead(KEYC2) == LOW) {key[8] = HIGH;} else {key[8] = LOW;}
            if (digitalRead(KEYC3) == LOW) {key[9] = HIGH;} else {key[9] = LOW;}
			for(byte C = 7; C<=9; C++){
				if ((keylast[C] == LOW) && (key[C] == HIGH)) {
					keypress[C] = HIGH;
				}
			}
			for(byte C = 7; C<=9; C++){
				if ((keylast[C] == HIGH) && (key[C] == LOW)) {
					keyrelease[C] = HIGH;
				}
			}
            break;
        default:
            keyR = 1;
    }
}


// Init keyboard read
void readkeyinit() {
    pinMode(KEYR1, INPUT);
    pinMode(KEYR2, INPUT);
    pinMode(KEYR3, INPUT);
    pinMode(KEYC1, INPUT_PULLUP);
    pinMode(KEYC2, INPUT_PULLUP);
    pinMode(KEYC3, INPUT_PULLUP);
    keyR = 1;
    for(byte C = 1; C<=9; C++){
        keylast[C] = LOW;
        keypress[C] = LOW;
        keyrelease[C] = LOW;
    }
}


// Setup
void setup() {
    readkeyinit();

    pinMode(RADIOTXREQ, OUTPUT);
    pinMode(TUNERKEY, INPUT_PULLUP);
    pinMode(TUNERSTART, OUTPUT);

    // Tuner default
    digitalWrite(TUNERSTART, HIGH);

    // Radio default
    digitalWrite(RADIOTXREQ, HIGH);

	// Show start text
	lcd.begin(16,2);
	lcd.backlight();
	lcd.clear();
	lcd.home();
	lcd.setCursor(0,0);
	lcd.print("YAESU  FTDX-3000");
	lcd.setCursor(0,1);
	lcd.print("REMOTE (C)OZ4ACH");
	delay(1000);

	// Set serial port
	Serial.begin(38400);
	Serial1.begin(38400);

	// Set time out
	Serial.setTimeout(10);
	Serial1.setTimeout(10);

	lcd.clear();
	lcd.home();

	// force a read from radio
	readradio = READRADIONOW;
}


// Loop
void loop() {

	// Read key pad
    readkey();

	// Set radio if keypress
	if (keyrelease[SENDTORADIO_VOX] == HIGH) {
		keyrelease[SENDTORADIO_VOX] = LOW;
		if (VOX == 0) {
			Serial1.print("VX1;");
			lcd.setCursor(0,0);
			lcd.print("VOX ON          ");
			} else {
			Serial1.print("VX0;");
			lcd.setCursor(0,0);
			lcd.print("VOX OFF         ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_PROC] == HIGH) {
		keyrelease[SENDTORADIO_PROC] = LOW;
		if (PROC == 0) {
			Serial1.print("PR01;");
			lcd.setCursor(0,0);
			lcd.print("PROC ON         ");
			} else {
			Serial1.print("PR00;");
			lcd.setCursor(0,0);
			lcd.print("PROC OFF        ");
		}
		readradio = READRADIONOW;
	}


	if (keyrelease[SENDTORADIO_MICEQ] == HIGH) {
		keyrelease[SENDTORADIO_MICEQ] = LOW;
		if (MICEQ == 0) {
			Serial1.print("PR11;");
			lcd.setCursor(0,0);
			lcd.print("MIC EQ ON       ");
			} else {
			Serial1.print("PR10;");
			lcd.setCursor(0,0);
			lcd.print("MIC EQ OFF      ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_PWRPROC] == HIGH) {
		keyrelease[SENDTORADIO_PWRPROC] = LOW;
		if (PWRPROC == 0) {
			Serial1.print("EX1771;");
			lcd.setCursor(0,0);
			lcd.print("PWR/PROC: PROC  ");
			} else {
			Serial1.print("EX1770;");
			lcd.setCursor(0,0);
			lcd.print("PWR/PROC: TX PWR");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_RFSQL] == HIGH) {
		keyrelease[SENDTORADIO_RFSQL] = LOW;
		if (RFSQL == 0) {
			Serial1.print("EX0361;");
			lcd.setCursor(0,0);
			lcd.print("RF/SQL: SQL     ");
			} else {
			Serial1.print("EX0360;");
			lcd.setCursor(0,0);
			lcd.print("RF/SQL: RF      ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_DNF] == HIGH) {
		keyrelease[SENDTORADIO_DNF] = LOW;
		if (DNF == 0) {
			Serial1.print("BC01;");
			lcd.setCursor(0,0);
			lcd.print("DNF ON          ");
			} else {
			Serial1.print("BC00;");
			lcd.setCursor(0,0);
			lcd.print("DNF OFF         ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_DNR] == HIGH) {
		keyrelease[SENDTORADIO_DNR] = LOW;
		if (DNR == 0) {
			Serial1.print("NR01;");
			lcd.setCursor(0,0);
			lcd.print("DNR ON          ");
			} else {
			Serial1.print("NR00;");
			lcd.setCursor(0,0);
			lcd.print("DNR OFF         ");
		}
		readradio = READRADIONOW;
	}

	// Start tune in tuner
    if (key[TUNEKEY] == HIGH) {
	    digitalWrite(TUNERSTART, LOW);
		lcd.setCursor(0,0);
		lcd.print("TUNE            ");
	} else {
	    digitalWrite(TUNERSTART, HIGH);
    }

	// Send TX req til radio
    if (	(key[REQTXKEY] == HIGH)
		|	(digitalRead(TUNERKEY) == LOW)) {
		digitalWrite(RADIOTXREQ, LOW);
		lcd.setCursor(0,0);
		lcd.print("TX REQ          ");
	} else {
		digitalWrite(RADIOTXREQ, HIGH);
    }


	// Read analog
	for (int analogpin=0;analogpin<ANALOGIND;analogpin++) {
		analognew[analogpin] = analogRead(analogpin);
	}

	for (int analogpin=0;analogpin<ANALOGIND;analogpin++) {
		if (	(analognew[analogpin] < (analogold[analogpin]-ANALOGCHANGE))
			||	(analognew[analogpin] > (analogold[analogpin]+ANALOGCHANGE))) {
//			lcd.setCursor(0,1);
//			lcd.print("                ");
			lcd.setCursor(0,1);

			switch (analogpin) {
				case 0:
					MONITOR = (analognew[analogpin]) /10;
					if (MONITOR > 100) MONITOR = 100;
					if (MONITOR < 10)  radioread = "EX03500";
					else if (MONITOR < 100) radioread = "EX0350";
					else radioread = "EX035";
					radioread.concat(String(MONITOR));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("MONITOR ");
					lcd.print(MONITOR);
					break;
				case 1:
					DVSRX = (analognew[analogpin]) /10;
					if (DVSRX > 100) DVSRX = 100;
					if (DVSRX < 10)  radioread = "EX01500";
					else if (DVSRX < 100) radioread = "EX0150";
					else radioread = "EX015";
					radioread.concat(String(DVSRX));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("DVS RX ");
					lcd.print(DVSRX);
					break;
				case 2:
					CONTOURLEVEL = (analognew[analogpin]) /16;
					CONTOURLEVEL = CONTOURLEVEL - 40;
					if (CONTOURLEVEL > 20) CONTOURLEVEL = 20;
					if (CONTOURLEVEL < -40) CONTOURLEVEL = -40;

					if (CONTOURLEVEL < 0) radioread = "EX108-";
					else if (CONTOURLEVEL > 0) radioread = "EX108+";
					else 	radioread = "EX108+";

					if ((CONTOURLEVEL > -10) && (CONTOURLEVEL < 10))  radioread.concat("0");

					radioread.concat(String(abs(CONTOURLEVEL)));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("CON LEVEL ");
					lcd.print(CONTOURLEVEL);
					break;
				case 3:
					CONTOURWIDTH = (analognew[analogpin]) /92;
					if (CONTOURWIDTH > 11) CONTOURWIDTH = 11;
					if (CONTOURWIDTH < 1) CONTOURWIDTH = 1;

					if (CONTOURWIDTH < 10)  radioread = "EX1090";
					else radioread = "EX109";
					radioread.concat(String(CONTOURWIDTH));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("CON WIDTH ");
					lcd.print(CONTOURWIDTH);
					break;
				case 4:
					VOXLEVEL = (analognew[analogpin]) /10;
					if (VOXLEVEL > 100) VOXLEVEL = 100;
					if (VOXLEVEL < 10)  radioread = "EX18100";
					else if (VOXLEVEL < 100) radioread = "EX1810";
					else radioread = "EX181";
					radioread.concat(String(VOXLEVEL));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("VOX LEVEL ");
					lcd.print(VOXLEVEL);
					break;
				case 5:
					VOXTIME = (analognew[analogpin]) /10;
					VOXTIME = VOXTIME * 30;
					VOXTIME = VOXTIME + 30;
					if (VOXTIME > 3000) VOXTIME = 3000;
					if (VOXTIME < 10)  radioread = "EX182000";
					else if (VOXTIME < 100) radioread = "EX18200";
					else if (VOXTIME < 1000) radioread = "EX1820";
					else radioread = "EX182";
					radioread.concat(String(VOXTIME));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("VOX TIME ");
					lcd.print(VOXTIME);
					break;
				case 6:
					VOXANTI = (analognew[analogpin]) /10;
					if (VOXANTI > 100) VOXANTI = 100;
					if (VOXANTI < 10)  radioread = "EX18300";
					else if (VOXANTI < 100) radioread = "EX1830";
					else radioread = "EX183";
					radioread.concat(String(VOXANTI));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("VOX ANTI ");
					lcd.print(VOXANTI);
					break;
				case 7:
					NBLEVEL = (analognew[analogpin]) /10;
					if (NBLEVEL > 100) NBLEVEL = 100;
					if (NBLEVEL < 10)  radioread = "EX03300";
					else if (NBLEVEL < 100) radioread = "EX0330";
					else radioread = "EX033";
					radioread.concat(String(NBLEVEL));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("NB LEVEL ");
					lcd.print(NBLEVEL);
					break;
				case 8:
					DNRNR = (69+analognew[analogpin]) /69;
					if (DNRNR < 10) radioread = "RL00"; else radioread = "RL0";
					radioread.concat(String(DNRNR));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("DNR LEVEL ");
					lcd.print(DNRNR);
					break;
			}
			lcd.print("                ");
//			lcd.setCursor(12,1);
//			lcd.print(analognew[analogpin]);
			readradio = READRADIONOW;
		}
	}

	for (int analogpin=0;analogpin<ANALOGIND;analogpin++) {
		analogold[analogpin] = analognew[analogpin];
	}





	// Read radio value
	readradio--;
	if (readradio == 0) {
		readradio = READRADIOCOUNT;

		Serial1.print("VX;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("VX0")) {
			VOX = 0;
			} else {
			VOX = 1;
		}

		Serial1.print("PR0;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("PR00")) {
			PROC = 0;
			} else {
			PROC = 1;
		}

		Serial1.print("PR1;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("PR10")) {
			MICEQ = 0;
			} else {
			MICEQ = 1;
		}

		Serial1.print("BC0;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("BC00")) {
			DNF = 0;
			} else {
			DNF = 1;
		}

		Serial1.print("NR0;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("NR00")) {
			DNR = 0;
			} else {
			DNR = 1;
		}

		Serial1.print("EX177;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("EX1770")) {
			PWRPROC = 0;
			} else {
			PWRPROC = 1;
		}

		Serial1.print("EX036;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("EX0360")) {
			RFSQL = 0;
			} else {
			RFSQL = 1;
		}

/*
		// Read analog value
		Serial1.print("EX035;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(5,6);
		MONITOR = radioread.toInt();


		Serial1.print("RL0;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(3,5);
		DNRNR = radioread.toInt();
*/
	}


	// Interface to PC using the USB connection
	radioread = Serial.readStringUntil(CR);

	// Filter MD = mode from the PC command
	if (!radioread.substring(0,2).equals("MD")) {
		Serial1.print(radioread);
	}

	radioread = Serial1.readStringUntil(CR);
	Serial.print(radioread);

}