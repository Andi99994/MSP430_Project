/*
 * DisplayDriver.c
 *
 *  Created on: 01.02.2018
 *      Author: andre
 */

#include "DisplayDriver.h"

static void display_showSymbol(uint16_t pos, uint16_t digit);

void display_init(void){
    LCDCPCTL1 |= LCDS18 | LCDS19;           // pins for digit A1
    LCDCPCTL0 |= LCDS10 | LCDS11;           // pins for digit A2
    LCDCPCTL0 |= LCDS6 | LCDS7 | LCDS8;     // pins for digit A3 (S8 -> dot)
    LCDCPCTL2 |= LCDS36 | LCDS37;           // pins for digit A4
    LCDCPCTL1 |= LCDS28 | LCDS29;           // pins for digit A5
    LCDCPCTL0 |= LCDS14 | LCDS15;           // pins for digit A6
    LCDCPCTL1 |= LCDS30;                    // pin for degree symbol
    LCDCPCTL1 |= LCDS20;                    // pin for negative symbol

    LCDCCTL0 |= LCDMX0 | LCDMX1 | LCDDIV4;
    LCDCCTL0 |= LCDSON;
    LCDCCTL0 |= LCDON;

    DISPLAY_CLEAR;
}


static void display_showSymbol(uint16_t pos, uint16_t digit){

    uint8_t digitCode;

    switch(digit){
        case 0:     digitCode = 0xFC;   // digit = 0 -> segment A, B, C, D, E, F
                    break;
        case 1:     digitCode = 0x60;   // digit = 1 -> segment B, C
                    break;
        case 2:     digitCode = 0xDB;   // digit = 2 -> segment A, B, D, E, G, M
                    break;
        case 3:     digitCode = 0xF3;   // digit = 3 -> segment A, B, C, D, G, M
                    break;
        case 4:     digitCode = 0x67;   // digit = 4 -> segment B, C, F, G, M
                    break;
        case 5:     digitCode = 0xB7;   // digit = 5 -> segment A, C, D, F, G, M
                    break;
        case 6:     digitCode = 0xBF;   // digit = 6 -> segment A, C, D, E, F, G, M
                    break;
        case 7:     digitCode = 0xE0;   // digit = 7 -> segment A, B, C
                    break;
        case 8:     digitCode = 0xFF;   // digit = 8 -> segment A, B, C, D, E, F, G, M
                    break;
        case 9:     digitCode = 0xF7;   // digit = 9 -> segment A, B, C, D, F, G, M
                    break;
        default:    digitCode = 0x00;
                    break;
    }

    switch(pos){
        case 1:     LCDM10 = digitCode;   // LCDM10 -> S19/S18
                    break;
        case 2:     LCDM6 = digitCode;    // LCDM6 -> S11/S10
                    break;
        case 3:     LCDM4 = digitCode;    // LCDM4 -> S7/S6
                    break;
        case 4:     LCDM19 = digitCode;   // LCDM19 -> S37/S36
                    break;
        case 5:     LCDM15 = digitCode;   // LCDM15 -> S29/S28
                    break;
        case 6:     LCDM8 = digitCode;    // LCDM8 -> S15/S14
                    break;
        case 95:    LCDM5 |= 0x01;        // LCDM5 -> S9/S8, A3 decimal point
                    break;
        case 96:    LCDM8 = 0x6F;         // LCDM8 -> S15/S14, A6 symbol H
                    break;
        case 97:    LCDM11 |= 0x01;       // LCDM11 -> S21/S20, A1 decimal point
                    break;
        case 98:    LCDM11 |= 0x04;       // LCDM11 -> S21/S20, negative symbol
                    break;
        case 99:    LCDM8 = 0x9C;         // LCDM8 -> S15/S14, A6 symbol C
                    LCDM16 = 0x04;        // LCDM16 -> S31/S30, ° symbol
                    break;
        default:    break;
    }
}

// temperature will be shown on launchpad LCD display in form: ###.# °C
void display_showTemperature(Temperature_t temperature, TemperatureUnit_t unit){
    DISPLAY_CLEAR;

    if(temperature < 0) {
        temperature = -temperature;
        display_showSymbol(98, 1);
    }

    unsigned int i = 0;
    for(i = 4; i>= 1; i--){
        display_showSymbol(i, temperature % 10);
        temperature /= 10;
        if(!temperature){
            break;
        }
    }
    display_showSymbol(95, 1);        // set decimal point
    display_showSymbol(99, 1);        // set °C symbol

    /*
    int32_t temp;
    uint32_t tempAbs;
    uint16_t i;



    temp = 17572 * (int32_t) tempSensorValue;     // fixed point -> all values multiplied by 100
    temp = temp / 65536;
    temp -= 4685;

    tempAbs = (uint32_t) temp / 10;     // remove second digit after comma
*/

}
