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


// Setup
void setup() {

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

}


// Loop
void loop() {

	led_update();
	readanalog();
	readkey();

	// Global state change
	if (keypress(KEY_10)) {
	} else
	if (keypress(KEY_11)) {
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
	} else
	if (keypress(KEY_DNF)) {
	} else

	if ((main_time + MAIN_TIMEOUT) < millis()) {
		main_state = STATE_INFO;
		main_time = millis();
	} else

	if (main_state < STATE_LIMIT) {
		if (keypress(KEY_ENTER)) {
			// TX for tune
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
			lcd.print("                    ");
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
			if (keypress(KEY_ESC)) {
				main_state = STATE_INFO;
				main_time = millis();
			}
			break;

		default:
			main_state = STATE_BOOT;
			main_time = millis();
			keyreset();
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