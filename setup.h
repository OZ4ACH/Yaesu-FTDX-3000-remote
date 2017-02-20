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

// LED pins
const word LED_ADR[] = {28,29,30,31,32,33};

// LCD setting and variable
LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address



// Menu
#define MAIN_TIMEOUT 3000








#endif