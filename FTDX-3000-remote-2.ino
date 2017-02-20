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
#define STATE_BOOT 1
#define STATE_INFO 2
#define STATE_ANALOG 3
#define STATE_KNAP 4
#define STATE_MENU 5
#define STATE_AUTOTUNE 6
uint32 main_time;


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
}


// Loop
void loop() {

	led_update();
	readanalog();
	readkey();


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

			if ((main_time + MAIN_TIMEOUT) < millis()) {
				main_state = STATE_INFO;
			}
			break;

		case STATE_KNAP:

			if ((main_time + MAIN_TIMEOUT) < millis()) {
				main_state = STATE_INFO;
			}
			break;

		case STATE_MENU:
			break;

		case STATE_AUTOTUNE:
			break;


		default:
			main_state = STATE_BOOT;
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