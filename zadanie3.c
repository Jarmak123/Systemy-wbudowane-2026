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
// Definicja makr tak by kod był czytelny, przejrzysty, deskryptywny i przyjazny użytkownikowi
#define FCY 4000000UL // częstotliwość robocza oscylatora jako połowa częstotliwości (FNOSC = FRC -> FCY = 4000000)
// Zdefiniowanie poszczególnych pingów jako odpowiednie makra
#define LCD_E LATDbits.LATD4
#define LCD_RW LATDbits.LATD5
#define LCD_RS LATBbits.LATB15
#define LCD_DATA LATE
// Przypisanie wartości poszczególnych komend do właściwych makr
#define LCD_CLEAR 0x01 //0b00000001
#define LCD_HOME 0x02 //0b00000010
#define LCD_ON 0x0C //0b00001100
#define LCD_OFF 0x08 //0b00001000
#define LCD_CONFIG 0x38 //0b00111000
#define LCD_CURSOR 0x80 //0b10000000
#define LINE1 0x00
#define LINE2 0x40
#define LCD_CUST_CHAR 0x40 //0b01000000
#define LCD_SHIFT_R 0x1D //0b00011100
#define LCD_SHIFT_L 0x1B //0b00011000
// Definicja funkcji delay w us i ms - operujących na jednostkach czasu zamiast cykli pracy oscylatora
void __delay_us(unsigned long us){
    __delay32(us*FCY/1000000);
}
void __delay_ms(unsigned long ms){
    __delay32(ms*FCY/1000);
}
// Definicja funkcji wysyłających komendy (RS = 0) i dane (RS = 1) za pomocą magistrali równoległej (LCD_DATA). Znaki i komendy mają 8 bitów!
void LCD_sendCommand(unsigned char command){
    LCD_RW = 0; // Zapis
    LCD_RS = 0; // Przesyłanie komend
    LCD_E = 1; // Otwarcie transmisji danych
    LCD_DATA = command;
    __delay_us(50); // Opóźnienie konieczne dla zapisania danych.
    LCD_E = 0; // Konieczne zablokowanie transmisji po przesłaniu komunikatu.
}
void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1; // Przesyłanie danych
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}
// Funkcja print wyświetlająca kolejne 8-bitowe znaki w pętli while, *oznacza przypisanie nie wartości zmiennej lecz jej adresu.
void LCD_print(unsigned char* string){
    while(*string){
    LCD_sendData(*string++);
    }
}
// Funkcja ustawiająca kursor w wybranym miejscu
void LCD_setCursor(unsigned char row, unsigned char col){
unsigned char address;
if (row == 1){
address = LCD_CURSOR + LINE1 + col;
}
if (row == 2){
address = LCD_CURSOR + LINE2 + col;
}
LCD_sendCommand(address);
}
// Funkcja ZAPISUJĄCA znak (zmienna array) do PAMIĄCI CGRAM na wybrany slocie od 0 do 7
void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
unsigned char i;
LCD_sendCommand(LCD_CUST_CHAR + (slot*8));
for(i=0;i<8;i++){
LCD_sendData(array[i]);
}
}
// Wybór znaku do wyświetlenia z tablicy
void LCD_displayCharacter(unsigned char characterCode) {
LCD_sendData(characterCode);
}
// Funkcja inicjalizująca wyświetlacz LCD. Wysyła niezbędne komendy jak LCD_CONFIG i LCD_ON
void LCD_init(){
__delay_ms(20);
LCD_sendCommand(LCD_CONFIG);
__delay_us(50); // opóźnienia wynikają ze specyfikacji wyświetlacza i czasu przetwarzania poszczególnych komend
LCD_sendCommand(LCD_ON);
__delay_us(50);
LCD_sendCommand(LCD_CLEAR);
__delay_ms(2);
}

void LCD_clearCustChars(void) {
    unsigned char slot;
    for (slot = 0; slot < 8; slot++) {
        LCD_saveCustChar(slot, emptyChar);
    }
}

// Zdefiniowanie znaku niestandardowego w postaci tablicy 8x5 (8 linii po 5 kropek)
unsigned char symbolK[8] = {
    0b11111,
    0b10101,
    0b10101,
    0b10101,
    0b10011,
    0b10101,
    0b10101,
    0b11111
};
unsigned char symbolE[8] = {
    0b11111,
    0b10001,
    0b10111,
    0b10011,
    0b10111,
    0b10111,
    0b10001,
    0b11111
};
unsigned char symbolB[8] = {
    0b11111,
    0b10011,
    0b10101,
    0b10011,
    0b10101,
    0b10101,
    0b10011,
    0b11111
};
unsigned char symbolA[8] = {
    0b11111,
    0b10001,
    0b10101,
    0b10001,
    0b10101,
    0b10101,
    0b10101,
    0b11111
};

unsigned char symbolK_neg[8] = {
    0b00000,
    0b01010,
    0b01010,
    0b01010,
    0b01100,
    0b01010,
    0b01010,
    0b00000
};

unsigned char symbolE_neg[8] = {
    0b00000,
    0b01110,
    0b01000,
    0b01100,
    0b01000,
    0b01000,
    0b01110,
    0b00000
};

unsigned char symbolB_neg[8] = {
    0b00000,
    0b01100,
    0b01010,
    0b01100,
    0b01010,
    0b01010,
    0b01100,
    0b00000
};

unsigned char symbolA_neg[8] = {
    0b00000,
    0b01110,
    0b01010,
    0b01110,
    0b01010,
    0b01010,
    0b01010,
    0b00000
};

unsigned char butelka[8] = {
    0b01110,
    0b00100,
    0b00100,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b01110
};

unsigned char noz[8] = {
    0b00100,
    0b00110,
    0b00110,
    0b00110,
    0b01110,
    0b00100,
    0b00100,
    0b00100
};

unsigned char widelec[8] = {
    0b10101,
    0b10101,
    0b10101,
    0b01110,
    0b00100,
    0b00100,
    0b00100,
    0b00100
};

unsigned char szklanka[8] = {
    0b01000,
    0b01100,
    0b10101,
    0b10001,
    0b11111,
    0b11111,
    0b01110,
    0b01110
};
 
void pierwsza_linijka(){
    LCD_sendData(4);
    LCD_print(" ");
    LCD_sendData(0);
    LCD_sendData(1);
    LCD_sendData(2);
    LCD_sendData(3);
    LCD_sendData(2);
    LCD_print("TANIO");
    LCD_print(" ");
    LCD_sendData(6);
}
 
void druga_linijka(){
    LCD_print(" ");
    LCD_sendData(5);
    LCD_print("ZAPRA SZAMY");
    LCD_sendData(7);
    LCD_print(" ");
}

void trzecia_linijka(){
    LCD_print(" ");
    LCD_sendData(5);
    LCD_sendData(0);
    LCD_sendData(1);
    LCD_sendData(2);
    LCD_sendData(3);
    LCD_sendData(2);
    LCD_print("TANIO");
    LCD_sendData(7);
    LCD_print(" ");
}
 
void czwarta_linijka(){
    LCD_sendData(4);
    LCD_print(" ");
    LCD_print("ZAPRA SZAMY");
    LCD_print(" ");
    LCD_sendData(6);
}

void save_negative(){
    LCD_saveCustChar(0, symbolK_neg); // Zapisanie znaku 'symbol1' do pamięci CGRAM
    LCD_saveCustChar(1, symbolE_neg);
    LCD_saveCustChar(2, symbolB_neg);
    LCD_saveCustChar(3, symbolA_neg);
    LCD_saveCustChar(4, butelka);// Zapisanie znaku 'symbol2' do pamięci CGRAM
    LCD_saveCustChar(5, szklanka);// Zapisanie znaku 'symbol2' do pamięci CGRAM
    LCD_saveCustChar(6, noz);// Zapisanie znaku 'symbol2' do pamięci CGRAM
    LCD_saveCustChar(7, widelec);// Zapisanie znaku 'symbol2' do pamięci CGRAM
}

void save_positive(){
    LCD_saveCustChar(0, symbolK); // Zapisanie znaku 'symbol1' do pamięci CGRAM
    LCD_saveCustChar(1, symbolE);
    LCD_saveCustChar(2, symbolB);
    LCD_saveCustChar(3, symbolA);
    LCD_saveCustChar(4, butelka);// Zapisanie znaku 'symbol2' do pamięci CGRAM
    LCD_saveCustChar(5, szklanka);// Zapisanie znaku 'symbol2' do pamięci CGRAM
    LCD_saveCustChar(6, noz);// Zapisanie znaku 'symbol2' do pamięci CGRAM
    LCD_saveCustChar(7, widelec);// Zapisanie znaku 'symbol2' do pamięci CGRAM
}
 
int main(void) {
    TRISB = 0x7FFF; 
    TRISD = 0x0000;
    TRISE = 0x0000;
    
    LCD_init(); 

    while(1){
        save_positive();
        LCD_setCursor(1,0);
        pierwsza_linijka();
        LCD_setCursor(2,0);
        druga_linijka();
        __delay_ms(2000);

        save_negative();
        __delay_ms(2000);

        LCD_sendCommand(LCD_CLEAR);
        __delay_ms(100);
        
        LCD_setCursor(1,0);
        trzecia_linijka();
        LCD_setCursor(2,0);
        czwarta_linijka();
        __delay_ms(2000);

    }
    
    return 0;
}

