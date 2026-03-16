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

void alarm(){
    int counter = 0;
    while(1){
        LATA = 0b10000000;
        __delay32(1000000);
        LATA = 0b00000000;
        __delay32(1000000);
        counter++;
        if(counter == 10) break;
    }
    while(1){
        LATA = 0b11111111;
    }
}

int main(void) {
 // Domyślna konfiguracja ADC
 ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
 // ADC ma czytać potencjometr
 ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
 // output LEDów PORT A
 TRISA = 0x0000;
 unsigned int value;
 while (1)
 {
// // Czytanie 10-bit wartości z potencjometru
// value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
// // Błąd?
// if (value == 0xFFFF) {
// // W razie czego przechodzimy do kolejnej iteracji
// continue;
// }
// // normalizacja zakresu do 0-255
// unsigned char normalizedValue = value >> 2;
// 
// if(normalizedValue == 0b10000000) break; 
// LATA = normalizedValue;
     alarm();
 }
 return 0;
}