/************************************************************************************************

By OZ4ACH Kim Moltved 2017
https://github.com/OZ4ACH

************************************************************************************************/

#ifndef setup_h
#define setup_h


// Key pad
const word KEY_ROW_ADR[] = {10,9,8};
const word KEY_COL_ADR[] = {11,12,13};

// Keys
const word KEYS_ADR[] = {22,23,24,25,26,27,34};

#define PRELL_TIME 20

// Analog
#define ANALOGIND 9
#define ANALOGCHANGE 2
#define ANALOGTRIG 3

// LED pins
const word LED_ADR[] = {28,29,30,31,32,33};

// LCD setting and variable
LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address



// Menu
#define MAIN_TIMEOUT 5000



#define KEY_MENU_UP 1
#define KEY_MENU_DOWN 4
#define KEY_VALUE_UP 2
#define KEY_VALUE_DOWN 5
#define KEY_ESC 3
#define KEY_ENTER 6

#define KEY_DNR 7
#define KEY_DNF 8
#define KEY_TUNE 9

#define KEY_VOX 10
#define KEY_PROC 11
#define KEY_12 12
#define KEY_13 13
#define KEY_14 14
#define KEY_15 15

#define KEY_FOODSWITCH 16

#define MENU_COUNT 10






#endif