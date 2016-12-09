#include <Wire.h>
#include <LiquidCrystal_I2C.h> // https://bitbucket.org/fmalpartida/new-liquidcrystal/overview
#include "define.h"

LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address


#define ANALOGIND 9
int16 analognew[ANALOGIND];
int16 analogold[ANALOGIND];
#define ANALOGCHANGE 2
















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


void setup() {
    readkeyinit();

    pinMode(RADIOTXREQ, OUTPUT);

    pinMode(TUNERKEY, INPUT_PULLUP);
    pinMode(TUNERSTART, OUTPUT);

    // Tuner default
    digitalWrite(TUNERSTART, HIGH);

    // Radio default
    digitalWrite(RADIOTXREQ, HIGH);

	lcd.begin(16,2);

	lcd.backlight();

	lcd.clear();
	lcd.home();
	lcd.setCursor(0,0);
	 		// 0123456789012345
	lcd.print("YAESU  FTDX-3000");
	lcd.setCursor(0,1);
	 		// 0123456789012345
	lcd.print("REMOTE (C)OZ4ACH");

	delay(1000);

	sendtoradio = 0;
	Serial.begin(38400);
	Serial1.begin(38400);

	Serial.setTimeout(10);
	Serial1.setTimeout(10);

	lcd.clear();
	lcd.home();

	readradio = READRADIONOW;


}


void loop() {

	// Read key pad
    readkey();

	// Set radio
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
	for (int c=0;c<ANALOGIND;c++) {
		analognew[c] = analogRead(c);
	}

	for (int c=0;c<ANALOGIND;c++) {
		if (	(analognew[c] < (analogold[c]-ANALOGCHANGE))
			||	(analognew[c] > (analogold[c]+ANALOGCHANGE))) {
			lcd.setCursor(0,1);
			lcd.print("                ");
			lcd.setCursor(0,1);

			switch (c) {
				case 0:
					MONITOR = (analognew[c]) /10;
					if (MONITOR > 100) MONITOR = 100;
					rb = MONITOR;
					if (rb < 10)  radioread = "EX03500";
					else if (rb < 100) radioread = "EX0350";
					else radioread = "EX035";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("MONITOR ");
					lcd.print(MONITOR);
					break;
				case 1:
					DVSRX = (analognew[c]) /10;
					if (DVSRX > 100) DVSRX = 100;
					rb = DVSRX;
					if (rb < 10)  radioread = "EX01500";
					else if (rb < 100) radioread = "EX0150";
					else radioread = "EX015";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("DVS RX ");
					lcd.print(DVSRX);
					break;
				case 2:
					CONTOURLEVEL = (analognew[c]) /16;
					CONTOURLEVEL = CONTOURLEVEL - 40;
					if (CONTOURLEVEL > 20) CONTOURLEVEL = 20;
					if (CONTOURLEVEL < -40) CONTOURLEVEL = -40;
					rb = CONTOURLEVEL;

					if (rb < 0) radioread = "EX108-";
					else if (rb > 0) radioread = "EX108+";
					else 	radioread = "EX108+";

					if ((rb > -10) && (rb < 10))  radioread.concat("0");

					radioread.concat(String(abs(rb)));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("CON LEV ");
					lcd.print(CONTOURLEVEL);
					break;
				case 3:
					CONTOURWIDTH = (analognew[c]) /92;
					if (CONTOURWIDTH > 11) CONTOURWIDTH = 11;
					if (CONTOURWIDTH < 1) CONTOURWIDTH = 1;
					rb = CONTOURWIDTH;
					if (rb < 10)  radioread = "EX1090";
					else radioread = "EX109";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("CON WID ");
					lcd.print(CONTOURWIDTH);
					break;
				case 4:
					VOXLEVEL = (analognew[c]) /10;
					if (VOXLEVEL > 100) VOXLEVEL = 100;
					rb = VOXLEVEL;
					if (rb < 10)  radioread = "EX18100";
					else if (rb < 100) radioread = "EX1810";
					else radioread = "EX181";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("VOX LEV ");
					lcd.print(VOXLEVEL);
					break;
				case 5:
					VOXTIME = (analognew[c]) /10;
					VOXTIME = VOXTIME * 30;
					VOXTIME = VOXTIME + 30;
					if (VOXTIME > 3000) VOXTIME = 3000;
					rb = VOXTIME;
					if (rb < 10)  radioread = "EX182000";
					else if (rb < 100) radioread = "EX18200";
					else if (rb < 1000) radioread = "EX1820";
					else radioread = "EX182";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("VOX TIME ");
					lcd.print(VOXTIME);
					break;
				case 6:
					VOXANTI = (analognew[c]) /10;
					if (VOXANTI > 100) VOXANTI = 100;
					rb = VOXANTI;
					if (rb < 10)  radioread = "EX18300";
					else if (rb < 100) radioread = "EX1830";
					else radioread = "EX183";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("VOX ANTI ");
					lcd.print(VOXANTI);
					break;
				case 7:
					lcd.setCursor(0,1);
					lcd.print(analognew[c]);
					break;
				case 8:
					DNRNR = (69+analognew[c]) /69;
					rb = DNRNR;
					if (rb < 10) radioread = "RL00"; else radioread = "RL0";
					radioread.concat(String(rb));
					radioread.concat(';');

					Serial1.print(radioread);
					readradio = READRADIONOW;

					lcd.print("DNR ");
					lcd.print(DNRNR);
					break;
			}
//			lcd.setCursor(12,1);
//			lcd.print(analognew[c]);
		}
	}

	for (int c=0;c<ANALOGIND;c++) {
		analogold[c] = analognew[c];
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



	radioread = Serial.readStringUntil(CR);

	// Filter MD = mode from the PC command
	if (!radioread.substring(0,2).equals("MD")) {
		Serial1.print(radioread);
	}

	radioread = Serial1.readStringUntil(CR);
	Serial.print(radioread);

//	delay(10);
}