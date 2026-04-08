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

int player1 = 0;
int player2 = 0;
unsigned int chess_mode = 0;

unsigned int map(unsigned int x, unsigned int in_min, unsigned int in_max, unsigned int out_min, unsigned int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

unsigned char czarny[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

unsigned char bialy[8] = {
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111
};

unsigned char strzalkaLewo[8] = {
    0b00100,
    0b01100,
    0b11100,
    0b11111,
    0b11111,
    0b11100,
    0b01100,
    0b00100
};

unsigned char strzalkaPrawo[8] = {
    0b00100,
    0b00110,
    0b00111,
    0b11111,
    0b11111,
    0b00111,
    0b00110,
    0b00100
};


void remis(void){
    LCD_sendCommand(LCD_CLEAR);
    bool p1_agree = false;
    bool p2_agree = false;

    while(1){
        LCD_setCursor(1, 0);
        LCD_print("REMIS?");

        LCD_setCursor(2, 0);
        LCD_print("G1:");
        if(p1_agree){
            LCD_sendData(0);
        }
        else {
            LCD_sendData(1);
        }

        LCD_print(" G2:");
        if(p2_agree){
            LCD_sendData(0);
        }
        else {
            LCD_sendData(1);
        }

        if(p1_agree == true && p2_agree == true){
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(1, 0);
            LCD_print("KONIEC GRY");
            LCD_setCursor(2, 0);
            LCD_print("REMIS");
            break;
        }
    }
    
    while(1){
        if (BUTTON_IsPressed(BUTTON_S6)) break;
    }
}

bool poddanie_G1(void){
    LCD_sendCommand(LCD_CLEAR);
    while (1)
    {
        LCD_sendCommand(LCD_CLEAR);
        LCD_setCursor(1, 0);
        LCD_print("G1 - PODDANIE");
        LCD_setCursor(2, 0);
        LCD_print("POTWIERDZENIE?");
        if(BUTTON_IsPressed(BUTTON_S4)==true)
        {
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(1, 0);
            LCD_print("G1 - PODDANIE");
            LCD_setCursor(2, 0);
            LCD_print("WYGRANA G2");
            return true;
        }
        if(BUTTON_IsPressed(BUTTON_S3)==true)
        {
            return false;
        }
    }
}

bool poddanie_G2(void){
    LCD_sendCommand(LCD_CLEAR);
    while (1)
    {
        LCD_sendCommand(LCD_CLEAR);
        LCD_setCursor(1, 0);
        LCD_print("G1 - PODDANIE");
        LCD_setCursor(2, 0);
        LCD_print("POTWIERDZENIE?");
        if(BUTTON_IsPressed(BUTTON_S5)==true)
        {
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(1, 0);
            LCD_print("G2 - PODDANIE");
            LCD_setCursor(2, 0);
            LCD_print("WYGRANA G1");
            return true;
        }
        if(BUTTON_IsPressed(BUTTON_S6)==true)
        {
            return false;
        }
    }
}

void mechanism(void){
    LCD_sendCommand(LCD_CLEAR);
    char buffer[16];
    bool p1_part = false;
    bool p1_first = false;
    bool p2_part = false;
    bool p2_first = false;
    
    while(1){
        LCD_setCursor(1, 0);
        LCD_print("Kto zaczyna?");
        LCD_setCursor(2, 0);
        LCD_print("G1 G2");
        if (BUTTON_IsPressed(BUTTON_S3)) {
            p1_part = true;
            p1_first = true;
        }
        else if((BUTTON_IsPressed(BUTTON_S6))){
            p2_part = true;
            p2_first = true;
        }

        if(p1_part == true || p2_part == true) break;
    }

    LCD_sendCommand(LCD_CLEAR);

    while(1){
        unsigned char minutes = player1 / 60;
        unsigned char secs = player1 % 60;
        LCD_setCursor(1,0);
        if (p1_part == true){
            LCD_sendData(2);
        }else{
            LCD_print(" ");
        }
        if(p1_first==true){
            LCD_sendData(1);
        }else{
            LCD_sendData(0);
        }
        sprintf(buffer, "G1: %02u:%02u", minutes, secs);
        LCD_print(buffer);

        unsigned char minutes = player2 / 60;
        unsigned char secs = player2 % 60;
        LCD_setCursor(2,0);
        sprintf(buffer, "%02u:%02u :G2", minutes, secs);
        LCD_print(buffer);
        if (p2_part == true){
            LCD_sendData(3);
        }else{
            LCD_print(" ");
        }
        if(p2_first==true){
            LCD_sendData(1);
        }else{
            LCD_sendData(0);
        }

        if(BUTTON_IsPressed(BUTTON_S3) && p1_part == true){
            p1_part = false;
            p2_part = true;
        }
        if(BUTTON_IsPressed(BUTTON_S6) && p2_part == true){
            p1_part = true;
            p2_part = false;
        }

        if(BUTTON_IsPressed(BUTTON_S4)==true){
            if(poddanie_G1()) break;
        }

        if(BUTTON_IsPressed(BUTTON_S5)==true){
            if(poddanie_G2()) break;
        }
        
        for(int i = 0; i < 100; i++) {
            if(BUTTON_IsPressed(BUTTON_S3) && p1_part) {
                p1_part = false; p2_part = true;
                __delay_ms(200);
                break; 
            }
            if(BUTTON_IsPressed(BUTTON_S6) && p2_part) {
                p1_part = true; p2_part = false;
                __delay_ms(200);
                break;
            }
            
            if(BUTTON_IsPressed(BUTTON_S4)) { if(poddanie_G1()) return; }
            if(BUTTON_IsPressed(BUTTON_S5)) { if(poddanie_G2()) return; }

            __delay_ms(10);
        }

        if(BUTTON_IsPressed(BUTTON_S3) && p1_part == true){
            p1_part = false;
            p2_part = true;
        }
        if(BUTTON_IsPressed(BUTTON_S6) && p2_part == true){
            p1_part = true;
            p2_part = false;
        }

        if(p1_part==true) player1--;
        if(p2_part==true) player2--;

        if(player1 == 0){
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(1,0);
            LCD_print("KONIEC CZASU");
            LCD_setCursor(1,0);
            LCD_print("WYGRANA G2");
            break;
        }else if(player2 == 0){
            LCD_sendCommand(LCD_CLEAR);
            LCD_setCursor(1,0);
            LCD_print("KONIEC CZASU");
            LCD_setCursor(1,0);
            LCD_print("WYGRANA G1");
            break;
        }

        if(BUTTON_IsPressed(BUTTON_S4)==true){
            if(poddanie_G1()) break;
        }

        if(BUTTON_IsPressed(BUTTON_S5)==true){
            if(poddanie_G2()) break;
        }
    }

}

void menu(void){
    LCD_sendCommand(LCD_CLEAR);
    int i = 0;
    while(1){
        LCD_setCursor(1, 0);
        LCD_print("WYBIERZ TRYB GRY");
        LCD_setCursor(2, 0);
        if (BUTTON_IsPressed(BUTTON_S6)) i++;
        if (BUTTON_IsPressed(BUTTON_S3)) i--;
        __delay_ms(200)
        if(i==0) {
            LCD_print("1. BULLET 1m");
            player1 = 60;
            player2 = 60;
        }
        else if (i==1) {
            LCD_print("2. BLITZ 5m");
            player1 = 300;
            player2 = 300;
        }
        else if (i==2) {
            LCD_print("3. RAPID 10m");
            player1 = 600;
            player2 = 600;
        }
        else if (i==3) {
            LCD_print("4.CLASSICAL 60m");
            player1 = 3600;
            player2 = 3600;
        }
        else {
            i=0;
        }

        if (BUTTON_IsPressed(BUTTON_S5)) {
            break;
        }
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

    LCD_saveCustChar(0, czarny);
    LCD_saveCustChar(1, bialy);
    LCD_saveCustChar(2, strzalkaLewo);
    LCD_saveCustChar(3, strzalkaPrawo);

    while(1) {
        menu();
        mechanism();
        while(1){
            if(BUTTON_IsPressed(BUTTON_S3)==true || BUTTON_IsPressed(BUTTON_S6)==true){
                break;
            }
        }
    }
    return 0;
}




