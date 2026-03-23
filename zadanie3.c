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
#include "buttons.h"
#include <libpic30.h>
#include "adc.h"
#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "buttons.h"
#define TOTAL_TIME (300) // w sekundach


volatile unsigned int seconds = TOTAL_TIME;
int main(void) {
char buffer[33]; // bufor na znaki do wyświetlenia na ekranie
LCD_Initialize(); // inicjalizacja LCD
unsigned int remainingSeconds = seconds;
//while(remainingSeconds > 1) {
//if (BUTTON_IsPressed ( BUTTON_S3 ) == true) {
//remainingSeconds = remainingSeconds + 30;
//} else if ((BUTTON_IsPressed ( BUTTON_S4 ) == true)) {
//remainingSeconds = remainingSeconds - 30;
//}
//unsigned char minutes = remainingSeconds / 60;
//unsigned char secs = remainingSeconds % 60;
//sprintf(buffer, "%02u:%02u", minutes, secs); // zapis czasu do bufora
//// wyświetlenie na ekranie
//LCD_ClearScreen();
//LCD_PutString(buffer, strlen(buffer));
//__delay32(4000000); //czekamy sekundę
//remainingSeconds--;
//}
sprintf(buffer, "%12s", "12345678912345678912345678912345");
LCD_ClearScreen();
LCD_PutString(buffer, strlen(buffer));
__delay32(40000000); //czekamy 10 sekund
return 0;
}
