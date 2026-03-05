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
int main(void) {
unsigned char portValue;
// Port A access
//BUTTON_IsPressed ( BUTTON_S3 ) == true
AD1PCFG = 0xFFFF;
TRISA = 0x0000;
portValue = 0b11111111;
int selected_zadanie = 0;

void zadanie1(char portValue){
    int i=1;

    while(1){
        portValue += i;
        LATA = portValue;
        __Delay32(10000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie2(char portValue){
    int i=-1;

    while(1){
        portValue += i;
        LATA = portValue;
        __Delay32(10000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie3(char portValue){
    int i=1;

    while(1){
        portValue +=1;
        g_portValue = portValue ^ (portValue >> 1);
        LATA = g_portValue;
        __Delay32(10000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie4(char portValue){
    int i=-1;

    while(1){
        portValue +=1;
        g_portValue = portValue ^ (portValue >> 1);
        LATA = g_portValue;
        __Delay32(10000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}

void zadanie5(){
    wyswietlacz1 = 0b00000000;
    wyswietlacz2 = 0b00000000;

    while(1){
        wyswietlacz1+=1;

        if(wyswietlacz1==0b00001010){
            wyswietlacz1=0b00000000;
            wyswietlacz2+=16;
        }
        if(wyswietlacz2==0b10100000){
            wyswietlacz2=0b00000000;
        }
        
        g_portValue = wyswietlacz1 | wyswietlacz2;
        LATA = g_portValue;
        __Delay32(10000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }
}


void zadanie6(){
    wyswietlacz1 = 0b11111111;
    wyswietlacz2 = 0b11111111;

    while(1){
        wyswietlacz1+=-1;

        if(wyswietlacz1==0b00001010){
            wyswietlacz1=0b11111111;
            wyswietlacz2+=-16;
        }
        if(wyswietlacz2==0b10100000){
            wyswietlacz2=0b11111111;
        }
        
        g_portValue = wyswietlacz1 | wyswietlacz2;
        LATA = g_portValue;
        __Delay32(10000);
        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
        }
    }

}

void zadanie7(){
    waz = 0b11100000;

    while(1){
        while(1){
            waz = waz >> 1;
            LATA = waz;
            __Delay32(10000);
            if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
            }
            if (waz == 0b00000111) break;
        }

        while(1){
            waz = waz << 1;
            LATA = waz;
            __Delay32(10000);
            if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
            }
            if (waz == 0b11100000) break;
        }

    }
}

void zadanie8(){
    kolejka = 0b00000000;
    int i=7;
    int add=1;
    LATA = kolejka;
    __Delay32(10000);
    while(1){
        kolejka++;
        __Delay32(10000);
        for(int j=1; j<=i;j++)
        {
            kolejka+=add;
            add=add*2;
            LATA = kolejka;
            __Delay32(10000);
            if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
            break;
            }
        }
        i=i-1;
        add=1;
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
            if((doXOR >> i) & 1) nieparzysta^=1;
        }

        wyjscie=(wyjscie<<1) | nieparzysta;
        LATA=wyjscie;
        while(BUTTON_IsPressed(BUTTON_S4) == false) {
            if(BUTTON_IsPressed(BUTTON_S4) == true) break;
        }

        while(BUTTON_IsPressed(BUTTON_S4) == true);
        __Delay32(1000);

        if(BUTTON_IsPressed ( BUTTON_S3 ) == true){
        break;
        }
    } 
}

while(1) {
    if(BUTTON_IsPressed ( BUTTON_S6 ) == true){
        selected_zadanie++;
    }
    else if(BUTTON_IsPressed ( BUTTON_S5 ) == true){
        selected_zadanie--;
    }

    if(selected_zadanie<1) selected_zadanie=9;
    if(selected_zadanie>0) selected_zadanie=1;

    if(selected_zadanie==1){
        portValue=0b10000000;
    }
    else if(selected_zadanie==2){
        portValue=0b01000000;
    }
    else if(selected_zadanie==3){
        portValue=0b00100000;
    }
    else if(selected_zadanie==4){
        portValue=0b00010000;
    }
    else if(selected_zadanie==5){
        portValue=0b00001000;
    }
    else if(selected_zadanie==6){
        portValue=0b01000100;
    }
    else if(selected_zadanie==7){
        portValue=0b00000010;
    }
    else if(selected_zadanie==8){
        portValue=0b00000001;
    }
    else if(selected_zadanie==9){
        portValue=0b11000000;
    }

    if(selected_zadanie==1 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        zadanie1(0b00000000);
    }
    else if(selected_zadanie==2 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        zadanie2(0b11111111);
    }
    else if(selected_zadanie==3 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        zadanie3(0b00000000);
    }
    else if(selected_zadanie==4 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        zadanie4(0b11111111);
    }
    else if(selected_zadanie==5 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        zadanie5(0b00000000);
    }
    else if(selected_zadanie==6 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        portValue=0b01000100;
    }
    else if(selected_zadanie==7 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        portValue=0b00000010;
    }
    else if(selected_zadanie==8 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        portValue=0b00000001;
    }
    else if(selected_zadanie==9 && BUTTON_IsPressed ( BUTTON_S4 ) == true){
        portValue=0b11000000;
    }
    
    LATA = portValue;
}
return -1;
}
