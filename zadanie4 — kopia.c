// PIC24FJ128GA010 Configuration Bit Settings
// For more on Configuration Bits, see Section 1.1
// consult your device data sheet
// CONFIG2
#pragma config POSCMOD = XT // XT Oscillator mode selected
#pragma config OSCIOFNC = ON // OSC2/CLKO/RC15 as port I/O (RC15)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor disabled
#pragma config FNOSC = PRI // Primary Oscillator (XT, HS, EC)
#pragma config IESO = ON // Int Ext Switch Over Mode enabled
// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "buttons.h"
#define TOTAL_TIME (300) // w sekundach
#define FCY 4000000UL

volatile unsigned int seconds = TOTAL_TIME;
unsigned int remainingSeconds = 0;
unsigned int wats = 100;
char* title = "";

unsigned int map(unsigned int x, unsigned int in_min, unsigned int in_max, unsigned int out_min, unsigned int out_max) {
    return (unsigned long)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ustawienie_czasu(void) {
    char buffer[16];
    LCD_ClearScreen();
 
    while (1) {
        unsigned int rawADC = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if (rawADC != 0xFFFF) {
            remainingSeconds = map(rawADC, 0, 1023, 0, 600);
        }
 
        unsigned char minutes = remainingSeconds / 60;
        unsigned char secs = remainingSeconds % 60;
        sprintf(buffer, "%02u:%02u", minutes, secs);
 
        LCD_ClearScreen();
        LCD_PutString("WYBIERZ CZAS", 12);
        LCD_PutString(buffer, strlen(buffer));
 
        __delay32(FCY / 10); // ~100 ms
 
        if (BUTTON_IsPressed(BUTTON_S6)) break;
    }
}

void ustawienie_mocy(void) {
    char buffer[16];
    LCD_ClearScreen();
 
    while (1) {
        unsigned int rawADC = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if (rawADC != 0xFFFF) {
            wats = map(rawADC, 0, 1023, 100, 1200);
        }
 
        if (wats <= 200)       title = "DEFROST";
        else if (wats <= 400)  title = "LOW";
        else if (wats <= 900)  title = "MEDIUM";
        else                   title = "HIGH";
 
        if (wats > 1200) wats = 100;
 
        sprintf(buffer, "%4dW, %s", wats, title);
 
        LCD_ClearScreen();
        LCD_PutString("WYBIERZ MOC ", 11);
        LCD_PutString(buffer, strlen(buffer));
 
        __delay32(FCY / 10);
 
        if (BUTTON_IsPressed(BUTTON_S6)) break;
    }
}

void pause_proces(void) {
    while (1) {
        if (BUTTON_IsPressed(BUTTON_S5)) break;
    }
}

void proces(void) {
    char buffer[16];
    LCD_ClearScreen();
 
    while (remainingSeconds > 0) {
        sprintf(buffer, "GRZANIE: %s", title);
        LCD_ClearScreen();
        LCD_PutString(buffer, strlen(buffer));
 
        unsigned char minutes = remainingSeconds / 60;
        unsigned char secs = remainingSeconds % 60;
        sprintf(buffer, "POZOSTALO:%02u:%02u", minutes, secs);
        LCD_PutString(buffer, strlen(buffer));
 
        if (BUTTON_IsPressed(BUTTON_S6)) break;
        if (BUTTON_IsPressed(BUTTON_S5)) pause_proces();
 
        __delay32(4000000);
        remainingSeconds--;
    }
}

void post_proces(void) {
    while (1) {
        LCD_ClearScreen();
        LCD_PutString("KONIEC CZASU!", 13);
        LCD_PutString("WYCIAGNIJ DANIE", 15);
 
        if (BUTTON_IsPressed(BUTTON_S6)) break;
 
        __delay32(FCY);
        LCD_ClearScreen();
        __delay32(FCY / 2);
    }
}

int main(void) {
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
 
    LCD_Initialize();
    LCD_ClearScreen();

    while (1) {
        ustawienie_czasu();
        ustawienie_mocy();
        proces();
        post_proces();
    }
 
    return 0;
}
