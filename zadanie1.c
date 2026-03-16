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

void zadanie1(unsigned char portValue){
    int i=1;

    while(1){
        portValue += i;
        LATA = portValue;
        __delay32(1000000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie2(unsigned char portValue){
    int i=-1;

    while(1){
        portValue += i;
        LATA = portValue;
        __delay32(1000000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie3(unsigned char portValue){
    while(1){
        portValue +=1;
        unsigned char g_portValue = portValue ^ (portValue >> 1);
        LATA = g_portValue;
        __delay32(1000000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie4(unsigned char portValue){
    while(1){
        portValue -=1;
        unsigned char g_portValue = portValue ^ (portValue >> 1);
        LATA = g_portValue;
        __delay32(1000000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie5(){
    unsigned char wyswietlacz1 = 0b00000000;
    unsigned char wyswietlacz2 = 0b00000000;

    while(1){
        wyswietlacz1+=1;

        if(wyswietlacz1==0b00001010){
            wyswietlacz1=0b00000000;
            wyswietlacz2+=16;
        }
        if(wyswietlacz2==0b10100000){
            wyswietlacz2=0b00000000;
        }
        
        unsigned char g_portValue = wyswietlacz1 | wyswietlacz2;
        LATA = g_portValue;
        __delay32(1000000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}


void zadanie6(){
    unsigned char wyswietlacz1 = 0b00001001;
    unsigned char wyswietlacz2 = 0b10010000;

    while(1){
        wyswietlacz1+=-1;

        if(wyswietlacz1==0b0000000){
            wyswietlacz1=0b00001001;
            wyswietlacz2+=-16;
        }
        if(wyswietlacz2==0b00000000){
            wyswietlacz2=0b10010000;
        }
        
        unsigned char g_portValue = wyswietlacz1 | wyswietlacz2;
        LATA = g_portValue;
        __delay32(1000000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }

}

void zadanie7(){
    unsigned char waz = 0b11100000;

    while(1){
        while(1){
            waz = waz >> 1;
            LATA = waz;
            __delay32(1000000);
            if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
            }
            if (waz == 0b00000111) break;
        }

        while(1){
            waz = waz << 1;
            LATA = waz;
            __delay32(1000000);
            if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
            }
            if (waz == 0b11100000) break;
        }
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
        break;
        }

    }
}

void zadanie8(){
    unsigned char kolejka = 0b00000000;
    int i=7;
    int add=1;
    LATA = kolejka;
    __delay32(1000000);
    while(1){
        kolejka++;
        LATA = kolejka;
        __delay32(1000000);
        for(int j=1; j<=i;j++)
        {
            kolejka+=add;
            add=add*2;
            LATA = kolejka;
            __delay32(1000000);
            if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
            }
        }
        i=i-1;
        add=1;
        if(kolejka == 0b11111111) break;
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
        break;
        }
    }
    while(1){
        LATA = kolejka;
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
        break;
        }
    }
}
   
void zadanie9(){
    unsigned char wyjscie = 0b000001;
    unsigned char konf = 0b110011;
    unsigned char doXOR, nieparzysta;
    while (1)
    {
        doXOR = wyjscie & konf;
        nieparzysta = 0;
        for(int i=0; i<6; i++)
        {
            if((doXOR >> i) & 1) nieparzysta ^= 1;
        }

        wyjscie = ((wyjscie << 1) | nieparzysta) & 0x3F; 

        LATA = wyjscie;
        
        __delay32(1000000);
        if(BUTTON_IsPressed(BUTTON_S3)){
            break;
        }
    } 
}

int main(void) {
unsigned char portValue;
char shift;
unsigned char start;
AD1PCFG = 0xFFFF; // set to digital I/O (not analog)
TRISA = 0x0000; // set all port bits to be output
start = 0b00000001;
shift = 0;
int i = 0;
while(1){
    
if (BUTTON_IsPressed ( BUTTON_S3 ) == true) {
            shift = shift + 1;
            i = i + 1;
            if (i > 8){
                shift = 0;
                i = 0;
            }
            __delay32(1000000);
        } else if ((BUTTON_IsPressed ( BUTTON_S4 ) == true)) {
            shift = shift - 1;
            i = i - 1;
            if (i < 0){
                shift = 8;
                i = 8;
            }
            __delay32(1000000);
        }

        if (i == 0) portValue = 0b10000000;
        else if (i == 1) portValue = 0b01000000;
        else if (i == 2) portValue = 0b00100000;
        else if (i == 3) portValue = 0b00010000;
        else if (i == 4) portValue = 0b00001000;
        else if (i == 5) portValue = 0b00000100;
        else if (i == 6) portValue = 0b00000010;
        else if (i == 7) portValue = 0b00000001;
        else if (i == 8) portValue = 0b11000000;

        LATA = portValue;
 
        if(i==0 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie1(0b00000000);
        }
        if(i==1 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie2(0b11111111);
        }
        if(i==2 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie3(0b00000000);
        }
        if(i==3 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie4(0b11111111);
        }
        if(i==4 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie5(0b00000000);
        }
        if(i==5 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie6();
        }
        if(i==6 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie7();
        }
        if(i==7 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie8();
        }
        if(i==8 && BUTTON_IsPressed ( BUTTON_S6 ) == true){
            zadanie9();
        }
}
return -1;
}