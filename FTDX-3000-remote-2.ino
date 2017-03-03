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
#define STATE_ANALOG_RUN 4

#define STATE_LIMIT 99

#define STATE_MENU 100
#define STATE_TUNE 110
#define STATE_AUTOTUNE 111
#define STATE_PLAYDVS 112
#define STATE_RECORDDVS 114

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

bool DVSPLAY;
bool DVSPLAYNOW;
bool DVSOLDPLAY;
long DVSTIME;
long DVSNR;


#define RADIOTXREQ  5
#define TUNERKEY   6
#define TUNERSTART 7
#define TXBYPASS 4

bool txreq;

int32 LASTTUNEFRQ;
bool TUNEFAIL;
int32 TXFRQ;
int TXVFO;
int TXPOWER;
int TUNEPOWER;

#define MAXTUNETIME (uint32)20000
#define MAXTUNETIMEOUT (uint32)25000
#define MAXSWRMETER 54

#define TUNE_OK 1
#define TUNE_TIMEOUT 2
#define TUNE_TUNING 3
#define TUNE_TUNED 4
int BAND;
int32 MAXFRQDIF;
int32 FRQDIF;
bool NEEDTUNE;
bool SWITCHANT;
bool PAOFF;

#define MAXTXPOWER 12
#define MAXTUNEPOWER 0


int VOXLEVEL;
int VOXLEVEL_OLD;
int VOXANTI;
int VOXANTI_OLD;
int VOXTIME;
int VOXTIME_OLD;
int CONTOURLEVEL;
int CONTOURLEVEL_OLD;
int CONTOURWIDTH;
int CONTOURWIDTH_OLD;
int DNRNR;
int DNRNR_OLD;
int DVSRX;
int DVSRX_OLD;
int MONITOR;
int MONITOR_OLD;
int NBLEVEL;
int NBLEVEL_OLD;
int TXBPF;
int TXBPF_OLD;
long DVSSETTIME;
long DVSSETTIME_1;
long DVSSETTIME_2;
long DVSSETTIME_OLD;








// TUNE
void tune(bool testswr) {
	uint32 starttime;
	uint16 status;
	uint16 swr;
	String radioread;

	lcd.setCursor(0,1);
	lcd.print("TUNE BEGIN          ");

	digitalWrite(TXBYPASS, LOW);
    delay(50);

	LASTTUNEFRQ = TXFRQ;
	TUNEFAIL = TRUE;

	starttime = millis();

	if (testswr) {
		// IS there need for tune
		lcd.setCursor(0,1);
		lcd.print("TUNE TEST SWR       ");

		digitalWrite(RADIOTXREQ, LOW);
		delay(400);

		Serial1.print("RM6;");
		radioread = Serial1.readStringUntil(';');
		radioread = radioread.substring(3,7);
		swr = radioread.toInt();

		digitalWrite(RADIOTXREQ, HIGH);

		if (swr < MAXSWRMETER) {
			lcd.setCursor(0,1);
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

 	lcd.setCursor(0,1);
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

		lcd.setCursor(0,1);
		lcd.print("TUNE RUNNING        ");

		status = TUNE_TUNING;
		while (status == TUNE_TUNING) {
			if (MAXTUNETIME < (millis() - starttime)) {
				status = TUNE_TIMEOUT;
			}
			if (digitalRead(TUNERKEY) == HIGH) {
				status = TUNE_TUNED;
			}
			lcd.setCursor(0,2);
			lcd.print((millis()-starttime));
			lcd.print("            ");
		}

		digitalWrite(RADIOTXREQ, HIGH);

		lcd.setCursor(0,2);
		lcd.print("TUNE END            ");
	}

	// timerout wait for tuner to stop TX req then exit
	if (status == TUNE_TIMEOUT) {
		lcd.setCursor(0,1);
		lcd.print("TUNE TIMEOUT        ");

		status = TUNE_TUNING;
		while (status == TUNE_TUNING) {
			if (MAXTUNETIMEOUT < (millis() - starttime)) {
				status = TUNE_TIMEOUT;
			}
			if (digitalRead(TUNERKEY) == HIGH) {
				status = TUNE_TUNED;
			}
			lcd.setCursor(0,2);
			lcd.print((millis()-starttime));
			lcd.print("            ");
		}
		return;
	}

	// Test SWR
	lcd.setCursor(0,1);
	lcd.print("TUNE TEST SWR       ");

	digitalWrite(RADIOTXREQ, LOW);
	delay(400);

	Serial1.print("RM6;");
	radioread = Serial1.readStringUntil(';');
	radioread = radioread.substring(3,7);
	swr = radioread.toInt();

	digitalWrite(RADIOTXREQ, HIGH);


	if (swr >= MAXSWRMETER) {
		lcd.setCursor(0,1);
		lcd.print("TUNE SWR FAIL       ");
		lcd.setCursor(16,2);
		lcd.print(swr);
		return;
	}

	lcd.setCursor(0,1);
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
			Serial1.print("FT;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("FT0")) {
				TXVFO = 0;
				} else {
				TXVFO = 1;
			}
			break;
		case 9:
			if (TXVFO == 0) {
				Serial1.print("FA;");
			} else {
				Serial1.print("FB;");
			}
			radioread = Serial1.readStringUntil(';');
			radioread = radioread.substring(2,11);
			TXFRQ = radioread.toInt();
			break;
		case 10:
			Serial1.print("PC;");
			radioread = Serial1.readStringUntil(';');
			radioread = radioread.substring(2,6);
			TXPOWER = radioread.toInt();
			break;
		case 11:
			Serial1.print("EX178;");
			radioread = Serial1.readStringUntil(';');
			radioread = radioread.substring(5,6);
			TUNEPOWER = radioread.toInt();
		case 12:
			Serial1.print("RI4;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("RI40")) {
				DVSPLAYNOW = FALSE;
			} else {
				DVSPLAYNOW = TRUE;
			}

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

	txreq = FALSE;

	readradionr = 0;

	DVSNR = 1;

	DVSPLAY = FALSE;
	DVSOLDPLAY = FALSE;
	DVSTIME = 0;
	DVSSETTIME = 3000;
	DVSPLAYNOW = FALSE;


	readkey();
	keyloopreset();
}


uint32 looptime;
uint32 lastlooptime;

// Loop
void loop() {
	looptime = millis()- lastlooptime;
	lastlooptime = millis();


	String radioread;

	readradio();

	led_update();
	readanalog();
	readkey();


	// Global state change
	if (keypress(KEY_VOX)) {
		if (VOX == 0) {
			VOX = 1;
			Serial1.print("VX1;");
		} else {
			VOX = 0;
			Serial1.print("VX0;");
		}
	} else
	if (keypress(KEY_PROC)) {
		if (PROC == 0) {
			PROC = 1;
			Serial1.print("PR01;");
		} else {
			PROC = 0;
			Serial1.print("PR00;");
		}
	} else
	if (keypress(KEY_DVSPLAY)) {
		if (DVSPLAY == TRUE) {
			DVSPLAY = FALSE;
		} else {
			DVSPLAY = TRUE;
		}
	} else
	if (keypress(KEY_DNR2)) {
		if (DNR == 0) {
			DNR = 1;
			Serial1.print("NR01;");
		} else {
			DNR = 0;
			Serial1.print("NR00;");
		}
	} else
	if (keypress(KEY_DNF2)) {
		if (DNF == 0) {
			DNF = 1;
			Serial1.print("BC01;");
		} else {
			DNR = 0;
			Serial1.print("BC00;");
		}
	} else
	if (keypress(KEY_15)) {
	} else
	if (keypress(KEY_DNR)) {
		if (DNR == 0) {
			DNR = 1;
			Serial1.print("NR01;");
		} else {
			DNR = 0;
			Serial1.print("NR00;");
		}
	} else
	if (keypress(KEY_DNF)) {
		if (DNF == 0) {
			DNF = 1;
			Serial1.print("BC01;");
		} else {
			DNR = 0;
			Serial1.print("BC00;");
		}
	} else

	if ((main_time + MAIN_TIMEOUT) < millis()) {
		main_state = STATE_INFO;
		main_time = millis();
	} else

	if (keypressed(KEY_FOODSWITCH) > 10) {
		Serial1.print("TX1;");
		DVSPLAY = FALSE;
	} else
	if (keyrelease(KEY_FOODSWITCH)) {
		if (keypressed_last(KEY_FOODSWITCH) < 250) {
			DVSPLAY = TRUE;
		}
		Serial1.print("TX0;");
		delay(20);
	} else
	if (	(keyreleased(KEY_FOODSWITCH) > 10)
		&& 	(keyreleased(KEY_FOODSWITCH) < 300)) {
		Serial1.print("TX0;");
	} else

	if (main_state < STATE_LIMIT) {
		if (keyrelease(KEY_ENTER)) {
			//digitalWrite(RADIOTXREQ, HIGH);
			txreq = FALSE;
		} else
		if (keypress(KEY_ENTER)) {
			//digitalWrite(RADIOTXREQ, LOW);
			txreq = TRUE;
		} else
		if (keypress(KEY_TUNE)) {
			main_state = STATE_TUNE;
			main_time = millis();
		} else
		if ((analog_change()) && (main_state != STATE_ANALOG_RUN)) {
			main_state = STATE_ANALOG;
			main_time = millis();
		} else
		if (keypress(KEY_ESC)) {
			main_state = STATE_INFO;
			main_time = millis();
			DVSPLAY = FALSE;
		} else
		if (keypress(KEY_VALUE_UP)) {
			main_time = millis();
			DVSNR--;
		} else
		if (keypress(KEY_VALUE_DOWN)) {
			main_time = millis();
			DVSNR++;
		}
		if (DVSNR < 1) DVSNR = 1;
		if (DVSNR > 5) DVSNR = 5;

		if (keypress(KEY_MENU_UP) || keypress(KEY_MENU_DOWN)) {
			main_state = STATE_MENU;
			main_time = millis();
		}
	}



// Analog calculate
//	1 4 7
//	2 5 8
//	3 6 9
	MONITOR_OLD     = MONITOR;
	DVSRX_OLD       = DVSRX;
	VOXLEVEL_OLD    = VOXLEVEL;
	VOXTIME_OLD     = VOXTIME;
	VOXANTI_OLD     = VOXANTI;
	TXBPF_OLD       = TXBPF;
	DNRNR_OLD       = DNRNR;
	DVSSETTIME_OLD  = DVSSETTIME;


	TXBPF = (analog(3)) /175;

	DVSSETTIME = (analog(2))*10 ;

	VOXLEVEL = (analog(4)) /10;
	if (VOXLEVEL > 100) VOXLEVEL = 100;

	VOXTIME = (analog(5)) /10;
	VOXTIME = VOXTIME * 30;
	VOXTIME = VOXTIME + 30;

	VOXANTI = (analog(6)) /10;
	if (VOXANTI > 100) VOXANTI = 100;

	MONITOR = (analog(7)) /10;
	if (MONITOR > 100) MONITOR = 100;

	DVSRX = (analog(8)) /10;
	if (DVSRX > 100) DVSRX = 100;

	DNRNR = (69+analog(9)) /69;




	if(MONITOR_OLD     != MONITOR) {
		if (MONITOR < 10)  radioread = "EX03500";
		else if (MONITOR < 100) radioread = "EX0350";
		else radioread = "EX035";
		radioread.concat(String(MONITOR));
		radioread.concat(';');

		Serial1.print(radioread);
	}
	if(DVSRX_OLD       != DVSRX) {
		if (DVSRX < 10)  radioread = "EX01500";
		else if (DVSRX < 100) radioread = "EX0150";
		else radioread = "EX015";
		radioread.concat(String(DVSRX));
		radioread.concat(';');

		Serial1.print(radioread);
	}
	if(VOXLEVEL_OLD    != VOXLEVEL) {
		if (VOXLEVEL < 10)  radioread = "EX18100";
		else if (VOXLEVEL < 100) radioread = "EX1810";
		else radioread = "EX181";
		radioread.concat(String(VOXLEVEL));
		radioread.concat(';');

		Serial1.print(radioread);
	}
	if(VOXTIME_OLD     != VOXTIME) {
		if (VOXTIME > 3000) VOXTIME = 3000;
		if (VOXTIME < 10)  radioread = "EX182000";
		else if (VOXTIME < 100) radioread = "EX18200";
		else if (VOXTIME < 1000) radioread = "EX1820";
		else radioread = "EX182";
		radioread.concat(String(VOXTIME));
		radioread.concat(';');

		Serial1.print(radioread);
	}
	if(VOXANTI_OLD     != VOXANTI) {
		if (VOXANTI < 10)  radioread = "EX18300";
		else if (VOXANTI < 100) radioread = "EX1830";
		else radioread = "EX183";
		radioread.concat(String(VOXANTI));
		radioread.concat(';');

		Serial1.print(radioread);
	}
	if(TXBPF_OLD       != TXBPF) {
		radioread = "EX104";
		radioread.concat(String(TXBPF));
		radioread.concat(';');

		Serial1.print(radioread);
	}
	if(DNRNR_OLD       != DNRNR) {
		if (DNRNR < 10) radioread = "RL00"; else radioread = "RL0";
		radioread.concat(String(DNRNR));
		radioread.concat(';');

		Serial1.print(radioread);
	}


	// Tuning tx req to radio
	if (	(digitalRead(TUNERKEY) == LOW)
		||	(txreq == TRUE)) {
		digitalWrite(RADIOTXREQ, LOW);
	} else {
		digitalWrite(RADIOTXREQ, HIGH);

	}

	// Short press with food switch start PLAY DVS
	if (DVSPLAY == TRUE)
	{
		Serial1.print("RI4;");
		radioread = Serial1.readStringUntil(';');
		if (radioread.equals("RI40"))
		{
			Serial1.print("TX;");
			radioread = Serial1.readStringUntil(';');
			if (radioread.equals("TX0")) {
				DVSPLAYNOW = FALSE;
				if ((DVSTIME + DVSSETTIME) < millis())
				{
					radioread = "PB0";
					radioread.concat(String(DVSNR));
					radioread.concat(';');
					Serial1.print(radioread);
					delay(50);
					DVSPLAYNOW = TRUE;
				}

				DVSOLDPLAY = TRUE;
				if ( DVSSETTIME < 40) {
					DVSPLAY = FALSE;
					DVSTIME = 0;
					DVSOLDPLAY = FALSE;
				}
			} else {
				DVSPLAY = FALSE;
			}

		} else {
			DVSTIME = millis();
			DVSPLAYNOW = TRUE;
		}
	} else {
		if (DVSOLDPLAY == TRUE)
		{
			DVSPLAYNOW = FALSE;
			DVSOLDPLAY = FALSE;
			Serial1.print("PB00;");
			DVSTIME = 0;
		}
	}


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





	if (TXFRQ > 30000000) {
		// PA not ok
		PAOFF = TRUE;
	} else if (TXPOWER > MAXTXPOWER) {
		// PA not ok
		PAOFF = TRUE;
	} else if (TUNEPOWER > MAXTUNEPOWER) {
		// PA not ok
		PAOFF = TRUE;
	} else if ((SWITCHANT == TRUE)) {
		// PA not ok
		PAOFF = TRUE;
	} else if ((NEEDTUNE == TRUE)) {
		// PA not ok
		PAOFF = TRUE;
	} else if ((TUNEFAIL == TRUE)) {
		// PA not ok
		PAOFF = TRUE;
	} else {
		// PA ok
		PAOFF = FALSE;
	}

	if (PAOFF == TRUE) {
		digitalWrite(TXBYPASS, LOW);
	} else {
		digitalWrite(TXBYPASS, HIGH);
	}



/************************************************************************************/
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
//			lcd.print("INFO                ");
			lcd.print("INFO                ");
			lcd.setCursor(10,0);
			lcd.print(looptime);

			lcd.setCursor(0,1);
			if (TXFRQ > 30000000) {
				// PA not ok
				lcd.print("PA OFF on 6m        ");
			} else if (TXPOWER > MAXTXPOWER) {
				// PA not ok
				lcd.print("PA off TXPW ");
				lcd.print(TXPOWER);
				lcd.print("W    ");
			} else if (TUNEPOWER > MAXTUNEPOWER) {
				// PA not ok
				lcd.print("PA off TUPW ");
				switch (TUNEPOWER) {
					case 0: lcd.print(" 10W"); break;
					case 1: lcd.print(" 20W"); break;
					case 2: lcd.print(" 50W"); break;
					case 3: lcd.print("100W"); break;
					default:
						lcd.print("???W");
				}
				lcd.print("    ");
			} else if ((SWITCHANT == TRUE)) {
				// PA not ok
				lcd.print("SWITCH ANTNNE       ");
			} else if ((NEEDTUNE == TRUE)) {
				// PA not ok
				lcd.print("NEED TUNE           ");
			} else if ((TUNEFAIL == TRUE)) {
				// PA not ok
				lcd.print("TUNE FAIL           ");
			} else {
				// PA ok
				lcd.print("PA OK               ");
			}
	lastlooptime = millis();

			lcd.setCursor(0,2);
			lcd.print("                    ");
			lcd.setCursor(0,3);
			lcd.print("DVS NR ");
			lcd.print(DVSNR);
			if ((DVSPLAY) || (DVSPLAYNOW == TRUE)) {
				if (DVSPLAYNOW == TRUE) {
					lcd.print(" PLAY       ");
				} else {
					lcd.print(" DELAY      ");
					lcd.setCursor(15,3);
					lcd.print((DVSTIME + DVSSETTIME ) - millis());
				}
			} else {
				lcd.print("            ");
			}
			break;

		case STATE_ANALOG:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("ANALOG              ");
			lcd.setCursor(0,1);
					// 12345678901234567890
			lcd.print("                    ");
			lcd.setCursor(0,2);
					// 12345678901234567890
			lcd.print("                    ");
			lcd.setCursor(0,3);
					// 12345678901234567890
			lcd.print("                    ");

			main_state = STATE_ANALOG_RUN;
			main_time = millis();
			//break;

		case STATE_ANALOG_RUN:
			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("ANALOG              ");

			lcd.setCursor(0,1);
			lcd.print(analog(1));lcd.print("  ");
			lcd.setCursor(0,2);
			if ( DVSSETTIME < 40) {
				lcd.print("   OFF ");
			} else {
				lcd.print(" ");
				DVSSETTIME_1 = DVSSETTIME/1000;
				if (DVSSETTIME_1 < 10) lcd.print(" ");
				lcd.print(DVSSETTIME_1);
				lcd.print(",");

				DVSSETTIME_2 = 	DVSSETTIME/100;
				DVSSETTIME_2 = DVSSETTIME_2 - (DVSSETTIME_1*10);
				lcd.print(DVSSETTIME_2);
				lcd.print("s ");
			}
			lcd.setCursor(0,3);
			switch(TXBPF) {
/*
				case 0: lcd.print("1-3000"); break;
				case 1: lcd.print("1-2900"); break;
				case 2: lcd.print("2-2800"); break;
				case 3: lcd.print("3-2700"); break;
				case 4: lcd.print("4-2600"); break;*/
				case 0: lcd.print(",1-3,0"); break;
				case 1: lcd.print(",1-2,9"); break;
				case 2: lcd.print(",2-2,8"); break;
				case 3: lcd.print(",3-2,7"); break;
				case 4: lcd.print(",4-2,6"); break;
				case 5: lcd.print(",0-4,0"); break;
			default:
				lcd.print("UKENDT");
			}

			lcd.setCursor(7,1);
			lcd.print("VOX");
			if (VOXLEVEL < 100) lcd.print(" ");
			if (VOXLEVEL < 10) lcd.print(" ");
			lcd.print(VOXLEVEL);
			lcd.setCursor(7,2);
			if (VOXTIME < 1000) lcd.print(" ");
			if (VOXTIME < 100) lcd.print(" ");
			lcd.print(VOXTIME);
			lcd.print("ms ");
			lcd.setCursor(7,3);
			lcd.print("ANT");
			if (VOXANTI < 100) lcd.print(" ");
			if (VOXANTI < 10) lcd.print(" ");
			lcd.print(VOXANTI);

			lcd.setCursor(14,1);
			lcd.print("MON");
			if (MONITOR < 100) lcd.print(" ");
			if (MONITOR < 10) lcd.print(" ");
			lcd.print(MONITOR);
			lcd.setCursor(14,2);
			lcd.print("DVS");
			if (DVSRX < 100) lcd.print(" ");
			if (DVSRX < 10) lcd.print(" ");
			lcd.print(DVSRX);
			lcd.setCursor(14,3);
			lcd.print("NR ");
			if (DNRNR < 100) lcd.print(" ");
			if (DNRNR < 10) lcd.print(" ");
			lcd.print(DNRNR);

			if (analog_change()) {
				main_time = millis();
			}
			break;

		case STATE_MENU:
			if (menu_point < 1) menu_point = 1;
			if (menu_point > MENU_COUNT) menu_point = MENU_COUNT;

			lcd.setCursor(0,0);
					// 12345678901234567890
			lcd.print("MENU ");
			if (menu_point < 100) lcd.print(" ");
			if (menu_point < 10) lcd.print(" ");
			lcd.print(menu_point);
		    // 12345678901234567890
			lcd.print("            ");

			switch (menu_point) {
				case 1:
					lcd.setCursor(0,1);
					lcd.print("AUTOTUNE            ");
					lcd.setCursor(0,2);
					lcd.print("                    ");
					lcd.setCursor(0,3);
					lcd.print("Tune the band       ");
					if (keypress(KEY_ENTER)) {
						main_state = STATE_AUTOTUNE;
						main_time = millis();
					}
					break;
				case 2:
					lcd.setCursor(0,1);
					lcd.print("RF/SQL              ");
					lcd.setCursor(0,2);
					lcd.print("                    ");
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
					lcd.setCursor(0,1);
					lcd.print("PWR/PROC            ");
					lcd.setCursor(0,2);
					lcd.print("                    ");
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
				case 4:
					lcd.setCursor(0,1);
					lcd.print("RECORD VOICE ");
					lcd.print(DVSNR);
					lcd.print("      ");
					lcd.setCursor(0,2);
					lcd.print("                    ");
					lcd.setCursor(0,3);
					lcd.print("                    ");

					if (keypress(KEY_ENTER)) {
						main_time = millis()+25000;

						radioread = "LM0";
						radioread.concat(String(DVSNR));
						radioread.concat(';');
						Serial1.print(radioread);
						lcd.setCursor(0,3);
						lcd.print("REC VOICE       ");
						delay(50);
					}

					if (keypress(KEY_VALUE_UP)) {
						main_time = millis();
						DVSNR--;
					}
					if (keypress(KEY_VALUE_DOWN)) {
						main_time = millis();
						DVSNR++;
					}
					if (DVSNR < 1) DVSNR = 1;
					if (DVSNR > 5) DVSNR = 5;
					break;


				default:
					lcd.setCursor(0,1);
					lcd.print("Default menu        ");
					lcd.setCursor(0,2);
					lcd.print("                    ");
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
	led_set(3,DVSPLAYNOW);
	led_set(4,DNR);
	led_set(5,DNF);
//	led_set(6,DVSPLAY);


	while (Serial1.available() > 0) {
		radioread = Serial1.readString();
	}

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

	while (Serial1.available() > 0) {
		radioread = Serial1.readString();
	}



	keyloopreset();
}