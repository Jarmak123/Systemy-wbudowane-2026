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

int player1 = 0;
int player2 = 0;
unsigned int chess_mode = 0;

bool remis(void){
    char buffer[16];
    LCD_ClearScreen();
    
    bool p1_agree = 0;
    bool p2_agree = 0;

    while(1){
        LCD_ClearScreen();
        LCD_PutString("REMIS?          ", 16);
        sprintf(buffer, "G1:%01u G2:%01u", p1_agree, p2_agree);
        LCD_PutString(buffer, strlen(buffer));

        if(BUTTON_IsPressed(BUTTON_S3)){ p1_agree = 1; __delay32(FCY / 5); }
        if(BUTTON_IsPressed(BUTTON_S4)){ p2_agree = 1; __delay32(FCY / 5); }
        if(BUTTON_IsPressed(BUTTON_S6) || BUTTON_IsPressed(BUTTON_S5)) return false;

        if(p1_agree && p2_agree){
            LCD_ClearScreen();
            LCD_PutString("MECZ ZAKONCZONY REMISEM", 25);
            __delay32(FCY * 3);
            return true;
        }
        __delay32(FCY / 5);
    }
}

bool poddanie_G1(void){
    LCD_ClearScreen();
    char buffer[16];
    while (1)
    {
        LCD_ClearScreen();
        LCD_PutString("G1 - PODDANIE   S5=TAK  S3=NIE", 31);
        if(BUTTON_IsPressed(BUTTON_S5)==true)
        {
            LCD_ClearScreen();
            LCD_PutString("G1 - PODDANIE   WYGRANA G2", 27);
            __delay32(FCY * 3);
            return true;
        }
        if(BUTTON_IsPressed(BUTTON_S3)==true)
        {
            return false;
        }
        __delay32(FCY / 5);
    }
}

bool poddanie_G2(void){
    LCD_ClearScreen();
    char buffer[16];
    while(1){
        LCD_ClearScreen();
        LCD_PutString("G2 - PODDANIE   S6=TAK  S4=NIE", 31);

        if(BUTTON_IsPressed(BUTTON_S6)){
            LCD_ClearScreen();
            LCD_PutString("G2 - PODDANIE   WYGRANA G1", 27);
            __delay32(FCY * 3);
            return true;
        }
        if(BUTTON_IsPressed(BUTTON_S4)) return false;
        __delay32(FCY / 5);
    }
}

void mechanism(void){
    LCD_ClearScreen();
    char buffer1[15];
    char buffer2[15];
    bool p1_part = false;
    bool p1_first = false;
    bool p2_part = false;
    bool p2_first = false;

    while(1){
        LCD_ClearScreen();
        LCD_PutString("Kto zaczyna?     S3=G1 S4=G2", 32);
        if(BUTTON_IsPressed(BUTTON_S3)){ p1_part = true; p1_first = true; break; }
        if(BUTTON_IsPressed(BUTTON_S4)){ p2_part = true; p2_first = true; break; }
        __delay32(FCY / 5);
    }
    __delay32(FCY / 5);
   LCD_ClearScreen();

    while(1){
        LCD_ClearScreen();

        unsigned char min1 = (unsigned char)(player1 / 60);
        unsigned char sec1 = (unsigned char)(player1 % 60);
        if(p1_part) LCD_PutString("<",1); else LCD_PutString(" ",1);
        sprintf(buffer1, "G1:%02u:%02u       ", min1, sec1);
        LCD_PutString(buffer1, 15);

        unsigned char min2 = (unsigned char)(player2 / 60);
        unsigned char sec2 = (unsigned char)(player2 % 60);
        sprintf(buffer2, "       %02u:%02u:G2", min2, sec2);
        LCD_PutString(buffer2, 15);
        if(p2_part) LCD_PutString(">",1); else LCD_PutString(" ", 1);


        int i;
        for(i = 0; i < 100; i++){
            if(BUTTON_IsPressed(BUTTON_S3) && p1_part){
                p1_part = false; p2_part = true;
                __delay32(FCY / 100);
                break;
            }
            if(BUTTON_IsPressed(BUTTON_S4) && p2_part){
                p1_part = true; p2_part = false;
                __delay32(FCY / 100);
                break;
            }
            if(BUTTON_IsPressed(BUTTON_S6)){ if(poddanie_G1()) return; LCD_ClearScreen(); break; }
            if(BUTTON_IsPressed(BUTTON_S5)){ if(poddanie_G2()) return; LCD_ClearScreen(); break; }
            if(p1_part == true && BUTTON_IsPressed(BUTTON_S4)){ if(remis()) return; LCD_ClearScreen(); break; }
            if(p2_part == true && BUTTON_IsPressed(BUTTON_S3)){ if(remis()) return; LCD_ClearScreen(); break; }
            
            __delay32(FCY / 100);
        }

        if(p1_part && player1 > 0) player1--;
        if(p2_part && player2 > 0) player2--;

        if(player1 <= 0){
            LCD_ClearScreen();
            LCD_PutString("KONIEC CZASU G1 - WYGRANA G2",29);
            __delay32(FCY * 3);
            return;
        }
        if(player2 <= 0){
            LCD_ClearScreen();
            LCD_PutString("KONIEC CZASU G2 - WYGRANA G1",29);
            __delay32(FCY * 3);
            return;
        }
        __delay32(FCY / 5);
    }
}

void menu(void){
    LCD_ClearScreen();
    int i = 0;
    while(1){
        LCD_ClearScreen();
        LCD_PutString("WYBIERZ TRYB GRY", 16);
        if(BUTTON_IsPressed(BUTTON_S6)){ i++; __delay32(FCY / 5); }
        if(BUTTON_IsPressed(BUTTON_S3)){ i--; __delay32(FCY / 5); }
        if(i < 0) i = 3;
        if(i > 3) i = 0;

        if(i == 0){
            LCD_PutString("1. BULLET 1m", 13);
            player1 = 60; player2 = 60;
        } else if(i == 1){
            LCD_PutString("2. BLITZ 5m", 12);
            player1 = 300; player2 = 300;
        } else if(i == 2){
            LCD_PutString("3. RAPID 10m", 13);
            player1 = 600; player2 = 600;
        } else {
            LCD_PutString("4.CLASSICAL 60m", 16);
            player1 = 3600; player2 = 3600;
        }

        if(BUTTON_IsPressed(BUTTON_S5)){ __delay32(FCY / 5); break; }
        __delay32(FCY / 5);
    }
}

int main(void) {
    LCD_Initialize();
    LCD_ClearScreen();

    while(1){
        menu();
        mechanism();
        while (1)
        {
            if(BUTTON_IsPressed(BUTTON_S3) || BUTTON_IsPressed(BUTTON_S6)) break;
        }
        __delay32(FCY / 5);
    }
return 0;
}
