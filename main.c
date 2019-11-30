#include <xc.h>
#include <stdio.h>
#include <pic18f4520.h>
#include <plib/delays.h>
#include "plib/xlcd.h" //ASPAS PEGA A PASTA QUE COLOCAMOS
//  Compiladores antigos
// __CONFIG(1,0x200);

#pragma config OSC = HS //Configuracao do FOSC3:FOSC0 oscilador do cristal
#pragma config PBADEN = OFF //PORTB Deixar digital quando está OFF - ON deixa analógico
#pragma config LVP = OFF //ICSP pino deixa em modo de não gravação, senão ela não ia fazer nada

#define _XTAL_FREQ 20000000 
#define _TEMPO_BOTAO 10000

int contTempo =0;
int seg = 0;
int min = 0;
int pressRB0 = 0;
int pressRB1 = 0;
int cronoProg = 0;
int cronoReg = 0;
int contBlink = 0;
int segReg = 59; //segundo do regressivo
int minReg = 59; //minuto do regressivo
char texto[5] = "00:00";

void configPic(void){
    
    TRISE = 0x00; //Como saída
    TRISB = 0xFF;
    
    INTCON2bits.RBPU = 0; //habilita pull up pra botão
    
    INTCONbits.GIE = 1; //Bit 7 do INTCONT, chave geral de interrupções
    INTCONbits.INT0IE = 1; //Habilita interrupção em RB0
    INTCON3bits.INT1IE = 1; //Habilita interrupção em RB1
    INTCONbits.TMR0IE = 1; //Ativar chave do timer 0
    T0CON = 0b11000111; 
    //Bit 7 liga o timer 0, bit 6 em 1 é 8 bits, bit 5 timer 0 como temporizador, bit 4 não utilizamos, bit 3 ativa o prescaler, bit 2 a 0 definem o fator do prescaler;
    TMR0L = 0; //começar o timer 0 (8 bits) em 0
}

void DelayFor18TCY(void){
    for (int i = 0; i < 18; i++)
    {
        __nop;
    }
}

//delay de 18
void DelayPORXLCD (void){
    __delay_ms(15);
}

void DelayXLCD (void){
    __delay_ms(5);
}

void inicializeLCD()
{
    DelayXLCD();
    OpenXLCD(FOUR_BIT & LINES_5X7);
    DelayXLCD();
    WriteCmdXLCD(DON & CURSOR_OFF & BLINK_OFF);
    DelayXLCD();
    while (BusyXLCD())
    {
        
    }
    WriteCmdXLCD(0x01);
}

void escreveLCD(char primeiraLinha[], char segundaLinha[])
{
    WriteCmdXLCD(0x01);
    DelayPORXLCD();
    WriteCmdXLCD(0x80);
    putrsXLCD(primeiraLinha);
    WriteCmdXLCD(0xC0);
    putrsXLCD(segundaLinha);
}

void cronoProgressivo()
{
    seg ++;
    if (seg == 60)
    {
        min = min++;
        seg = 0;
    }
    sprintf(texto, "%02d:%02d", min, seg);
    escreveLCD("Crono - Prog.", texto);
}

void cronoRegressivo()
{
    segReg --;
    if (segReg == 0)
    {
        minReg = minReg--;
        segReg = 60;
    }
    sprintf(texto, "%02d:%02d", minReg, segReg);
    escreveLCD("Crono - Reg.", texto);
}

void interrupt interrupcao(void)
{
    if (INTCONbits.INT0IF) //ocorre interrupçao
    {  
        INTCONbits.INT0IF = 0; //limpa flag para habilitar novamente interrupçao
   
        if (pressRB0 == 0) //botao RB0 habilita cronoProg
        {
            escreveLCD("Crono - Prog.", texto);
            PORTEbits.RE0 = 1; //acende o led em RE0
            cronoProg = 1; 
            pressRB0 = 1;
        }
        else
        {
            PORTEbits.RE0 = 0; 
            cronoProg = 0;
            pressRB0 = 0;
        }     
    }
    else if (INTCON3bits.INT1IF)
    {
        INTCON3bits.INT1IF = 0; //Precisa limpar a flag, quando ela é ativa é porque houve a interrupção devido ao RB0
        
        if (pressRB1 == 0)
        {
            escreveLCD("Crono - Reg.", texto);
            PORTEbits.RE1 = 1;
            cronoReg = 1;
            pressRB1 = 1;
        }
        else
        {
            PORTEbits.RE1 = 0;
            cronoReg = 0;
            pressRB1 = 0;
        }  
        
    } 
    
    else if (INTCONbits.TMR0IF)
    {
        INTCONbits.TMR0IF = 0; //zera flag
        TMR0L = 0;
        if (++contTempo == 77)
        {
            contTempo = 0;
                        
            if (cronoProg == 1)
            {
                cronoProgressivo();
            }
            
            if (cronoReg == 1)
            {
                cronoRegressivo();
            }
            
            if (PORTBbits.RB0 == 0)
            {
                contBlink ++; //segura botao
                if (contBlink == 2)
                {
                    contBlink = 0;
                    seg = 0;
                    min = 0;
                    cronoProg = 0;
                    sprintf(texto, "%02d:%02d", min, seg);
                    escreveLCD("Crono - Prog.", texto);
                }
            }
            
            if (PORTBbits.RB1 == 0)
            {
                contBlink ++; 
                if (contBlink == 2) 
                {
                    contBlink = 0;
                    segReg = 59;
                    minReg = 59;
                    cronoProg = 0;
                    sprintf(texto, "%02d:%02d", minReg, segReg);
                    escreveLCD("Crono - Reg.", texto);
                }
            }
            
        }
    }
}

void main(void) {
    configPic();
    inicializeLCD();
    
    PORTE = 0x00;
    PORTEbits.RE0 = 1;
         
    while (1)
    {        
    }
    
    return;
}
