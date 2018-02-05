/*
 * DisplayDriver.c
 *
 *  Created on: 01.02.2018
 *      Author: andre
 */

#include "DisplayDriver.h"

#define NEGATIVE_POS        101
#define DEGREE_POS          102
#define UNIT_POS            103
#define DECIMAL_POS         104

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

static void displayDriver_showSymbol(uint16_t pos, Symbol_t symbol);
static Symbol_t displayDriver_resolveDigitToSymbol(uint8_t digit);
static Symbol_t displayDriver_resolveUnitToSymbol(TemperatureUnit_t unit);

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


static void displayDriver_showSymbol(uint16_t pos, Symbol_t symbol){

    uint8_t digitCode = symbolTable[symbol];

    switch(pos){
        case 1:
            LCDM10 = digitCode;   // LCDM10 -> S19/S18
            break;
        case 2:
            LCDM6 = digitCode;    // LCDM6 -> S11/S10
            break;
        case 3:
            LCDM4 = digitCode;    // LCDM4 -> S7/S6
            break;
        case 4:
            LCDM19 = digitCode;   // LCDM19 -> S37/S36
            break;
        case 5:
            LCDM15 = digitCode;   // LCDM15 -> S29/S28
            break;
        case 6:
            LCDM8 = digitCode;    // LCDM8 -> S15/S14
            break;
        case DECIMAL_POS:
            LCDM5 |= digitCode;        // LCDM5 -> S9/S8, A3 decimal point
            break;
        case NEGATIVE_POS:
            LCDM11 |= digitCode;       // LCDM11 -> S21/S20, negative symbol
            break;
        case DEGREE_POS:
            LCDM16 = digitCode;        // LCDM16 -> S31/S30, ° symbol
            break;
        case UNIT_POS:
            LCDM8 = digitCode;         // LCDM8 -> S15/S14, A6 symbol unit
            break;
        default:
            break;
    }
}


void displayDriver_showTemperature(Temperature_t temperature, TemperatureUnit_t unit){
    DISPLAY_CLEAR;

    if(temperature < 0) {
        temperature = -temperature;
        displayDriver_showSymbol(NEGATIVE_POS, SYMBOL_NEGATIVE);
    }

    unsigned int i = 0;
    for(i = 4; i>= 1; i--){
        displayDriver_showSymbol(i, displayDriver_resolveDigitToSymbol(temperature % 10));
        temperature /= 10;
        if(!temperature){
            break;
        }
    }
    displayDriver_showSymbol(DECIMAL_POS, SYMBOL_DECIMAL);
    displayDriver_showSymbol(DEGREE_POS, SYMBOL_DEGREE);
    displayDriver_showSymbol(UNIT_POS, displayDriver_resolveUnitToSymbol(unit));
}

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
