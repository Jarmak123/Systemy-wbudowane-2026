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

#define TOTAL_TIME (300) // w sekundach
volatile unsigned int seconds = TOTAL_TIME;
unsigned int remainingSeconds = 0; // Domyślnie 1 minuta
unsigned int wats = 100;
char* title = "";
unsigned char emptyChar[8] = {0,0,0,0,0,0,0,0};

unsigned int map(unsigned int x, unsigned int in_min, unsigned int in_max, unsigned int out_min, unsigned int out_max) {
    return (unsigned long)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

void ustawienie_czasu(void){
    LCD_sendCommand(LCD_CLEAR);
    char buffer[16];
    LCD_setCursor(1, 0);
    LCD_print("WYBIERZ CZAS    ");

    while(1){
        unsigned int rawADC = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if (rawADC != 0xFFFF) {
            remainingSeconds = map(rawADC, 0, 1023, 0, 600);
        }

        unsigned char minutes = remainingSeconds / 60;
        unsigned char secs = remainingSeconds % 60;
        sprintf(buffer, "%02u:%02u          ", minutes, secs);
        LCD_setCursor(2, 0);
        LCD_print(buffer);

        __delay_ms(100);

        if (BUTTON_IsPressed(BUTTON_S6)) break;
    }
}

void ustawienie_mocy(void){
    LCD_sendCommand(LCD_CLEAR);
    char buffer[16];
    // POPRAWKA 2: tekst statyczny wyświetlamy raz przed pętlą — nie migocze
    LCD_setCursor(1, 0);
    LCD_print("WYBIERZ MOC     ");

    while(1){
        unsigned int rawADC = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
        if (rawADC != 0xFFFF) {
            wats = map(rawADC, 0, 1023, 100, 1200);
        }

        if (wats <= 200)       title = "DEFROST";
        else if (wats <= 400)  title = "LOW    ";
        else if (wats <= 900)  title = "MEDIUM ";
        else                   title = "HIGH   ";

        if (wats > 1200) wats = 100;

        sprintf(buffer, "%4dW, %s", wats, title);
        LCD_setCursor(2, 0);
        LCD_print(buffer);

        __delay_ms(100);

        if (BUTTON_IsPressed(BUTTON_S6)) break;
    }
}

void pause_proces(void){
    while(1){
        if (BUTTON_IsPressed(BUTTON_S5)) break;
    }
}

void proces(void){
    LCD_sendCommand(LCD_CLEAR);
    char buffer[16];

    while(remainingSeconds > 0) {
        LCD_setCursor(1, 0);
        sprintf(buffer, "GRZANIE: %-6s", title);
        LCD_print(buffer);

        unsigned char minutes = remainingSeconds / 60;
        unsigned char secs = remainingSeconds % 60;
        LCD_setCursor(2, 0);
        sprintf(buffer, "POZOSTALO:%02u:%02u", minutes, secs);
        LCD_print(buffer);

        if (BUTTON_IsPressed(BUTTON_S6)) break;
        if (BUTTON_IsPressed(BUTTON_S5)) pause_proces();

        __delay32(4000000);
        remainingSeconds--;
    }
}

void post_proces(void){
    LCD_sendCommand(LCD_CLEAR);
    while(1){
        LCD_setCursor(1, 0);
        LCD_print("KONIEC CZASU!");
        LCD_setCursor(2, 0);
        LCD_print("WYCIAGNIJ DANIE");
        if (BUTTON_IsPressed(BUTTON_S6)) break;
        __delay_ms(1000);
        LCD_sendCommand(LCD_CLEAR);
        __delay_ms(500);
    }
}

 
int main(void) {
    AD1PCFG = 0xFFFB;
    TRISB = 0x7FFF; 
    TRISD = 0x0000;
    TRISE = 0x0000;
    
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);

    LCD_init(); 
    LCD_sendCommand(LCD_CLEAR);

    while(1) {
        ustawienie_czasu();
        ustawienie_mocy();
        proces();
        post_proces();
    }
    return 0;
}




