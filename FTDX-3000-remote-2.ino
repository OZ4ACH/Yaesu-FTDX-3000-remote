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
#define ANALOGCHANGE 3


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


#define KEY10 22
#define KEY11 23
#define KEY12 24
#define KEY13 25
#define KEY14 26
#define KEY15 27

#define LED1 28
#define LED2 29
#define LED3 30
#define LED4 31
#define LED5 32
#define LED6 33


// Key number
#define SENDTORADIO_VOX 10
#define SENDTORADIO_PROC 11
#define SENDTORADIO_MICEQ 12
#define SENDTORADIO_PWRPROC 13
#define SENDTORADIO_RFSQL 14
#define REQTXKEY 15
#define SENDTORADIO_DNR 7
#define SENDTORADIO_DNF 8
#define TUNEKEY 9


// Radio and Tuner interface
// Pins
#define RADIOTXREQ  5
#define TUNERKEY   6
#define TUNERSTART 7
#define TXBYPASS 4


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
int TXBPF;

int TXPOWER;
int TUNEPOWER;
bool PAOFF;
#define MAXTXPOWER 12
#define MAXTUNEPOWER 0

int32 LASTTUNEFRQ;
int32 TXFRQ;
bool NEEDTUNE;
bool TUNEFAIL;
bool SWITCHANT;
int BAND;
int32 MAXFRQDIF;
int32 FRQDIF;


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


    for(byte C = 10; C<=15; C++){
        keylast[C] = key[C];
    }
    if (digitalRead(KEY10) == LOW) {key[10] = HIGH;} else {key[10] = LOW;}
    if (digitalRead(KEY11) == LOW) {key[11] = HIGH;} else {key[11] = LOW;}
    if (digitalRead(KEY12) == LOW) {key[12] = HIGH;} else {key[12] = LOW;}
    if (digitalRead(KEY13) == LOW) {key[13] = HIGH;} else {key[13] = LOW;}
    if (digitalRead(KEY14) == LOW) {key[14] = HIGH;} else {key[14] = LOW;}
    if (digitalRead(KEY15) == LOW) {key[15] = HIGH;} else {key[15] = LOW;}
	for(byte C = 10; C<=15; C++){
		if ((keylast[C] == LOW) && (key[C] == HIGH)) {
			keypress[C] = HIGH;
		}
	}
	for(byte C = 10; C<=15; C++){
		if ((keylast[C] == HIGH) && (key[C] == LOW)) {
			keyrelease[C] = HIGH;
		}
	}


}




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


void autotune() {

uint32 startfrq;
uint32 stopfrq;
uint32 stepfrq;
uint32 freq;

	Serial1.print("FA;");
	radioread = Serial1.readStringUntil(';');
	radioread = radioread.substring(2,11);
	TXFRQ = radioread.toInt();

	if 		(TXFRQ <  2800000) { startfrq =  1810000;	stopfrq =  2000000;		stepfrq = 2000;}
	else if (TXFRQ <  4600000) { startfrq =  3500000;	stopfrq =  3800000;		stepfrq = 4000;}
	else if (TXFRQ <  6000000) { startfrq =  5250000;	stopfrq =  5450000;		stepfrq = 5000;}
	else if (TXFRQ <  8500000) { startfrq =  7000000;	stopfrq =  7200000;		stepfrq = 7000;}
	else if (TXFRQ < 12000000) { startfrq = 10100000;	stopfrq = 10150000;		stepfrq =10000;}
	else if (TXFRQ < 16000000) { startfrq = 14000000;	stopfrq = 14350000;		stepfrq =14000;}
	else if (TXFRQ < 20000000) { startfrq = 18068000;	stopfrq = 18168000;		stepfrq =18000;}
	else if (TXFRQ < 23000000) { startfrq = 21000000;	stopfrq = 21450000;		stepfrq =21000;}
	else if (TXFRQ < 26500000) { startfrq = 24890000;	stopfrq = 24990000;		stepfrq =25000;}
	else if (TXFRQ < 30000000) { startfrq = 28000000;	stopfrq = 29700000;		stepfrq =28000;}
	else if (TXFRQ < 53000000) { startfrq = 50000000;	stopfrq = 52000000;		stepfrq =50000;}
	else                       { return; }

	for (freq = startfrq; freq <= stopfrq; freq = freq + stepfrq) {
		lcd.setCursor(0,3);
		lcd.print("FRQ:                ");
		lcd.setCursor(5,3);
		lcd.print(freq);
		lcd.print(" Hz");


		radioread = "FA";
		if (freq < 10000000) {
			radioread.concat("0");
		}
		radioread.concat(String(freq));
		radioread.concat(';');

		Serial1.print(radioread);

		//delay(500);
		tune(false);

		if (TUNEFAIL == TRUE) {
			exit;
		}
	}

	freq = TXFRQ;
	radioread = "FA";
	if (freq < 10000000) {
		radioread.concat("0");
	}
	radioread.concat(String(freq));
	radioread.concat(';');

	Serial1.print(radioread);
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
    pinMode(TXBYPASS, OUTPUT);

    // Tuner default
    digitalWrite(TUNERSTART, HIGH);

    // Radio default
    digitalWrite(RADIOTXREQ, HIGH);

    // TX Rele Bypass default
    digitalWrite(TXBYPASS, LOW);



    pinMode(KEY10, INPUT_PULLUP);
    pinMode(KEY11, INPUT_PULLUP);
    pinMode(KEY12, INPUT_PULLUP);
    pinMode(KEY13, INPUT_PULLUP);
    pinMode(KEY14, INPUT_PULLUP);
    pinMode(KEY15, INPUT_PULLUP);



	// Set LED6
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);
    pinMode(LED6, OUTPUT);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, HIGH);
    digitalWrite(LED6, HIGH);




	// Show start text
	lcd.begin(20,4);
	lcd.backlight();
	lcd.clear();
	lcd.home();
	lcd.setCursor(0,0);
	lcd.print("YAESU  FTDX-3000");
	lcd.setCursor(0,1);
	lcd.print("REMOTE (C)OZ4ACH");
	lcd.setCursor(0,2);
	lcd.print("2017-02-02");
	lcd.setCursor(0,3);
	lcd.print("01234567890123456789");
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

	PAOFF = TRUE;
	NEEDTUNE = TRUE;

	LASTTUNEFRQ = 0;
	TXFRQ = 0;
	SWITCHANT = TRUE;


    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
    digitalWrite(LED6, LOW);

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
			lcd.setCursor(0,1);
			lcd.print("VOX ON          ");
			} else {
			Serial1.print("VX0;");
			lcd.setCursor(0,1);
			lcd.print("VOX OFF         ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_PROC] == HIGH) {
		keyrelease[SENDTORADIO_PROC] = LOW;
		if (PROC == 0) {
			Serial1.print("PR01;");
			lcd.setCursor(0,1);
			lcd.print("PROC ON         ");
			} else {
			Serial1.print("PR00;");
			lcd.setCursor(0,1);
			lcd.print("PROC OFF        ");
		}
		readradio = READRADIONOW;
	}


	if (keyrelease[SENDTORADIO_MICEQ] == HIGH) {
		keyrelease[SENDTORADIO_MICEQ] = LOW;
		if (MICEQ == 0) {
			Serial1.print("PR11;");
			lcd.setCursor(0,1);
			lcd.print("MIC EQ ON       ");
			} else {
			Serial1.print("PR10;");
			lcd.setCursor(0,1);
			lcd.print("MIC EQ OFF      ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_PWRPROC] == HIGH) {
		keyrelease[SENDTORADIO_PWRPROC] = LOW;
		if (PWRPROC == 0) {
			Serial1.print("EX1771;");
			lcd.setCursor(0,1);
			lcd.print("PWR/PROC: PROC  ");
			} else {
			Serial1.print("EX1770;");
			lcd.setCursor(0,1);
			lcd.print("PWR/PROC: TX PWR");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_RFSQL] == HIGH) {
		keyrelease[SENDTORADIO_RFSQL] = LOW;
		if (RFSQL == 0) {
			Serial1.print("EX0361;");
			lcd.setCursor(0,1);
			lcd.print("RF/SQL: SQL     ");
			} else {
			Serial1.print("EX0360;");
			lcd.setCursor(0,1);
			lcd.print("RF/SQL: RF      ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_DNF] == HIGH) {
		keyrelease[SENDTORADIO_DNF] = LOW;
		if (DNF == 0) {
			Serial1.print("BC01;");
			lcd.setCursor(0,1);
			lcd.print("DNF ON          ");
			} else {
			Serial1.print("BC00;");
			lcd.setCursor(0,1);
			lcd.print("DNF OFF         ");
		}
		readradio = READRADIONOW;
	}

	if (keyrelease[SENDTORADIO_DNR] == HIGH) {
		keyrelease[SENDTORADIO_DNR] = LOW;
		if (DNR == 0) {
			Serial1.print("NR01;");
			lcd.setCursor(0,1);
			lcd.print("DNR ON          ");
			} else {
			Serial1.print("NR00;");
			lcd.setCursor(0,1);
			lcd.print("DNR OFF         ");
		}
		readradio = READRADIONOW;
	}

	// Start tune in tuner
    if (keyrelease[TUNEKEY] == HIGH) {
    	keyrelease[TUNEKEY] = LOW;
    	tune(true);
    }
    // Tuner tune
//	if (digitalRead(TUNERKEY) == LOW) {
//		LASTTUNEFRQ = TXFRQ;
//		NEEDTUNE = FALSE;
//		SWITCHANT = FALSE;
//	}

	// Send TX req til radio
    if (	(key[REQTXKEY] == HIGH)
		|	(digitalRead(TUNERKEY) == LOW)) {
		digitalWrite(RADIOTXREQ, LOW);
		lcd.setCursor(0,1);
		lcd.print("TX REQ          ");
	} else {
		digitalWrite(RADIOTXREQ, HIGH);
    }



	// Start autotune in tuner
    if (keyrelease[6] == HIGH) {
    	keyrelease[6] = LOW;
    	autotune();
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
			lcd.setCursor(0,3);

			switch (analogpin) {
				case 6:
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
				case 7:
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
				case 0:
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
					lcd.print("dB");
					break;
				case 1:
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
				case 3:
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
				case 4:
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
					lcd.print("ms");
					break;
				case 5:
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
				case 2:
					TXBPF = (analognew[analogpin]) /175;
					radioread = "EX104";
					radioread.concat(String(TXBPF));
					radioread.concat(';');

					Serial1.print(radioread);

					lcd.print("TX BPF ");
					//lcd.print(TXBPF);
					switch(TXBPF) {
						case 0: lcd.print("100-3000"); break;
						case 1: lcd.print("100-2900"); break;
						case 2: lcd.print("200-2800"); break;
						case 3: lcd.print("300-2700"); break;
						case 4: lcd.print("400-2600"); break;
						case 5: lcd.print("TTBF 4000"); break;
					default:
						lcd.print("UKENDT");
					}
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
				// not used
				case 100:
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

		// Read analog value
		Serial1.print("FA;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(2,11);
		TXFRQ = radioread.toInt();

		Serial1.print("PC;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(2,6);
		TXPOWER = radioread.toInt();

		Serial1.print("EX178;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(5,6);
		TUNEPOWER = radioread.toInt();


		// Find band
		if      (TXFRQ <  1600000) { BAND = 200; MAXFRQDIF =     0; }
		else if (TXFRQ <  2800000) { BAND = 160; MAXFRQDIF =  2000; }
		else if (TXFRQ <  4600000) { BAND = 80;  MAXFRQDIF =  4000; }
		else if (TXFRQ <  6000000) { BAND = 60;  MAXFRQDIF =  5000; }
		else if (TXFRQ <  8500000) { BAND = 40;  MAXFRQDIF =  7000; }
		else if (TXFRQ < 12000000) { BAND = 30;  MAXFRQDIF = 10000; }
		else if (TXFRQ < 16000000) { BAND = 20;  MAXFRQDIF = 14000; }
		else if (TXFRQ < 20000000) { BAND = 17;  MAXFRQDIF = 18000; }
		else if (TXFRQ < 23000000) { BAND = 15;  MAXFRQDIF = 21000; }
		else if (TXFRQ < 26500000) { BAND = 12;  MAXFRQDIF = 25000; }
		else if (TXFRQ < 30000000) { BAND = 10;  MAXFRQDIF = 28000; }
		else if (TXFRQ < 53000000) { BAND = 6;   MAXFRQDIF = 10000; }
		else                       { BAND = 4;   MAXFRQDIF =     0; }

		// Is there need for TUNE
		FRQDIF = abs(TXFRQ - LASTTUNEFRQ);
		if (FRQDIF > MAXFRQDIF) {
			NEEDTUNE = TRUE;
		} else {
			NEEDTUNE = FALSE;
		}

		if (	((TXFRQ < 12000000) && (LASTTUNEFRQ > 12000000))
			||	((TXFRQ > 12000000) && (LASTTUNEFRQ < 12000000))) {
			SWITCHANT = TRUE;
		} else {
			SWITCHANT = FALSE;
		}


		// Set PA on or off depend of the power setting in the radio
		lcd.setCursor(0,0);
		if (TXFRQ > 30000000) {
			// PA not ok
			digitalWrite(TXBYPASS, LOW);
			PAOFF = TRUE;
			//         0123456789012345
			lcd.print("PA off NON 6m   ");
		} else if (TXPOWER > MAXTXPOWER) {
			// PA not ok
			digitalWrite(TXBYPASS, LOW);
			PAOFF = TRUE;
			//         0123456789012345
			lcd.print("PA off TXPW ");
			lcd.print(TXPOWER);
			lcd.print("W    ");
		} else if (TUNEPOWER > MAXTUNEPOWER) {
			// PA not ok
			digitalWrite(TXBYPASS, LOW);
			PAOFF = TRUE;
			//         0123456789012345
			lcd.print("PA off TUPW ");
			switch (TUNEPOWER) {
				case 0: lcd.print(" 10W"); break;
				case 1: lcd.print(" 20W"); break;
				case 2: lcd.print(" 50W"); break;
				case 3: lcd.print("100W"); break;
				default:
					lcd.print("???W");
			}
		} else if ((NEEDTUNE == TRUE)) {
			// PA not ok
			digitalWrite(TXBYPASS, LOW);
			PAOFF = TRUE;
			//         0123456789012345
			if (SWITCHANT == TRUE) {
				lcd.print("SKIFT ANTENNE    ");
			} else {
				lcd.print("PA off NEED TUNE ");
			}
		} else if ((TUNEFAIL == TRUE)) {
			// PA not ok
			digitalWrite(TXBYPASS, LOW);
			PAOFF = TRUE;
			lcd.print("PA off NEED TUNE ");
		} else {
			// PA ok
			digitalWrite(TXBYPASS, HIGH);
			if (PAOFF == TRUE) {
				lcd.print("PA on           ");
				PAOFF = FALSE;
			}
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

    digitalWrite(LED1, VOX);
    digitalWrite(LED2, PROC);
    digitalWrite(LED3, MICEQ);
    digitalWrite(LED4, PWRPROC);
    digitalWrite(LED5, RFSQL);
    if (	(key[REQTXKEY] == HIGH)
		|	(digitalRead(TUNERKEY) == LOW)) {
	    digitalWrite(LED6, HIGH);
	} else {
    	digitalWrite(LED6, LOW);
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