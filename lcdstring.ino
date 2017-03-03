/************************************************************************************************

Key read
By OZ4ACH Kim Moltved 2016
https://github.com/OZ4ACH

************************************************************************************************/

#include <string.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // https://bitbucket.org/fmalpartida/new-liquidcrystal/overview
#include "define.h"
#include "setup.h"

#define LCDLINE 4
#define LCDCHAR 20

String lcdline[LCDLINE];
String lcdline_old[LCDLINE];

// LCD setting and variable
LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address

void lcdstring_clear() {
	for (int c = 0; c < LCDLINE;c++) {
		lcdline[c] = "";
//		lcdline_old[c] = "";
	}
}

void lcdstring_init() {
	lcd.begin(LCDCHAR,LCDLINE);
	lcd.backlight();
	lcd.clear();
	lcd.home();
	lcd.setCursor(0,0);
	for (int c = 0; c < LCDLINE;c++) {
		lcdline[c] = "";
		lcdline_old[c] = "";
	}
}

void lcdstring_update() {
	String line;
	for (int c = 0; c < LCDLINE;c++) {
		if (lcdline[c].compareTo(lcdline_old[c])) {
			line = lcdline[c];
			while (line.length() < LCDCHAR) {
				line.concat(" ");
			}
			line = line.substring(0,LCDCHAR);
			lcd.setCursor(0,c);
			lcd.print(line);
		}
		lcdline_old[c] = lcdline[c];
	}
}

void lcd_str(int linenr, String line) {
	if ((linenr >= 0) && (linenr < LCDLINE)) {
		lcdline[linenr].concat(line);
	}
}

void lcd_uint32(int linenr,int length, uint32 value) {
	String t1;

	if ((linenr < 0) && (linenr >= LCDLINE)) return;

	t1 = String(value);

	while (t1.length() < length) {
		t1 = " " + t1;
	}
	t1 = t1.substring(0,length);

	lcdline[linenr].concat(t1);
}



