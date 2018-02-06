/*
 * DisplayDriver.c
 *
 * This file implements all functionality of the display required for the application.
 *
 */

#include "DisplayDriver.h"

//Defines some display constants to displayed in certain segments.
#define NEGATIVE_POS        101
#define DEGREE_POS          102
#define UNIT_POS            103
#define DECIMAL_POS         104

//Defines all currently required symbols. Note: Not every symbol is compatible with every segment.
typedef enum {
    SYMBOL_0,
    SYMBOL_1,
    SYMBOL_2,
    SYMBOL_3,
    SYMBOL_4,
    SYMBOL_5,
    SYMBOL_6,
    SYMBOL_7,
    SYMBOL_8,
    SYMBOL_9,
    SYMBOL_A,
    SYMBOL_B,
    SYMBOL_C,
    SYMBOL_D,
    SYMBOL_E,
    SYMBOL_F,
    SYMBOL_G,
    SYMBOL_H,
    SYMBOL_I,
    SYMBOL_J,
    SYMBOL_K,
    SYMBOL_L,
    SYMBOL_M,
    SYMBOL_N,
    SYMBOL_O,
    SYMBOL_P,
    SYMBOL_Q,
    SYMBOL_R,
    SYMBOL_S,
    SYMBOL_T,
    SYMBOL_U,
    SYMBOL_V,
    SYMBOL_W,
    SYMBOL_X,
    SYMBOL_Y,
    SYMBOL_Z,
    SYMBOL_NEGATIVE,
    SYMBOL_DEGREE,
    SYMBOL_DECIMAL
} Symbol_t;

//Defines all symbol codes required. Note: These have to be in the same order as the Symbol_t enum.
static uint8_t symbolTable[39] = {
                   0xFC,
                   0x60,
                   0xDB,
                   0xF3,
                   0x67,
                   0xB7,
                   0xBF,
                   0xE0,
                   0xFF,
                   0xF7,
                   0xEF,
                   0xF1,
                   0x9C,
                   0xF0,
                   0x9F,
                   0x8F,
                   0xBD,
                   0x6F,
                   0x90,
                   0x78,
                   0x0E,
                   0x1C,
                   0x6C,
                   0x6C,
                   0xFC,
                   0xCF,
                   0xFC,
                   0xCF,
                   0xB7,
                   0x80,
                   0x7C,
                   0x0C,
                   0x6C,
                   0x00,
                   0x00,
                   0x90,
                   0x04,
                   0x04,
                   0x01
};

/**
 * Displays a single symbol on the specified segment index.
 */
static void displayDriver_showSymbol(uint16_t segmentIndex, Symbol_t symbol);

/**
 * Converts an integer to a Symbol. Any number exceeding 9 will be shown as 0.
 */
static Symbol_t displayDriver_resolveDigitToSymbol(uint8_t digit);

/**
 * Converts a unit to a symbol.
 */
static Symbol_t displayDriver_resolveUnitToSymbol(TemperatureUnit_t unit);

/**
 * Initializes all required LCD segments and clears the display initially.
 */
void displayDriver_init(void){
    LCDCPCTL1 |= LCDS18 | LCDS19;
    LCDCPCTL0 |= LCDS10 | LCDS11;
    LCDCPCTL0 |= LCDS6 | LCDS7 | LCDS8;
    LCDCPCTL2 |= LCDS36 | LCDS37;
    LCDCPCTL1 |= LCDS28 | LCDS29;
    LCDCPCTL0 |= LCDS14 | LCDS15;
    LCDCPCTL1 |= LCDS30;
    LCDCPCTL1 |= LCDS20;

    LCDCCTL0 |= LCDMX0 | LCDMX1 | LCDDIV4;
    LCDCCTL0 |= LCDSON;
    LCDCCTL0 |= LCDON;

    DISPLAY_CLEAR;
}

/**
 * Displays a single symbol on the specified segment index.
 */
static void displayDriver_showSymbol(uint16_t segmentIndex, Symbol_t symbol){

    uint8_t digitCode = symbolTable[symbol];                //Resolve symbol to symbol code

    switch(segmentIndex){
        case 1:
            LCDM10 = digitCode;
            break;
        case 2:
            LCDM6 = digitCode;
            break;
        case 3:
            LCDM4 = digitCode;
            break;
        case 4:
            LCDM19 = digitCode;
            break;
        case DECIMAL_POS:
            LCDM5 |= digitCode;
            break;
        case NEGATIVE_POS:
            LCDM11 |= digitCode;
            break;
        case DEGREE_POS:
            LCDM16 = digitCode;
            break;
        case UNIT_POS:
            LCDM8 = digitCode;
            break;
        default:
            break;
    }
}

/**
 * Display a temperature of specified unit on the LCD display. The temperature has one decimal place multiplied by 10 (20,1°C = 201 here) and can range
 * from -9999 to +9999 (= -999.9° to +999.9°). Any overflowing places will be truncated and not displayed.
 */
void displayDriver_showTemperature(Temperature_t temperature, TemperatureUnit_t unit){
    DISPLAY_CLEAR;                                                                          //Clear display initially

    if(temperature < 0) {                                                                   //If temperature is negative, show negative symbol and invert temperature
        temperature = -temperature;
        displayDriver_showSymbol(NEGATIVE_POS, SYMBOL_NEGATIVE);
    }

    unsigned int i = 0;
    for(i = 4; i > 0; i--){                                                                 //Iterate through 4 numbers displayable
        displayDriver_showSymbol(i, displayDriver_resolveDigitToSymbol(temperature % 10));  //Show the rightmost digit
        temperature /= 10;                                                                  //Truncate the temperature to remove the already displaying digit
        if(temperature == 0){                                                               //If there are no digits left, stop
            break;
        }
    }
    displayDriver_showSymbol(DECIMAL_POS, SYMBOL_DECIMAL);                                  //Display decimal point
    displayDriver_showSymbol(DEGREE_POS, SYMBOL_DEGREE);                                    //Display degree symbol
    displayDriver_showSymbol(UNIT_POS, displayDriver_resolveUnitToSymbol(unit));            //Display unit symbol
}

/**
 * Converts an integer to a Symbol. Any number exceeding 9 will be shown as 0.
 */
Symbol_t displayDriver_resolveDigitToSymbol(uint8_t digit) {
    switch(digit) {
    case 0:
        return SYMBOL_0;
    case 1:
        return SYMBOL_1;
    case 2:
        return SYMBOL_2;
    case 3:
        return SYMBOL_3;
    case 4:
        return SYMBOL_4;
    case 5:
        return SYMBOL_5;
    case 6:
        return SYMBOL_6;
    case 7:
        return SYMBOL_7;
    case 8:
        return SYMBOL_8;
    case 9:
        return SYMBOL_9;
    default:
        return SYMBOL_0;
    }
}

/**
 * Converts a unit to a symbol.
 */
Symbol_t displayDriver_resolveUnitToSymbol(TemperatureUnit_t unit) {
    switch(unit) {
    case CELSIUS:
        return SYMBOL_C;
    case FAHRENHEIT:
        return SYMBOL_F;
    default:
        return SYMBOL_C;
    }
}
