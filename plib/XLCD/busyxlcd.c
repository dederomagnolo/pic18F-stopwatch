#include <p18cxxx.h>
#include "../xlcd.h"

/********************************************************************
*       Function Name:  BusyXLCD                                    *
*       Return Value:   char: busy status of LCD controller         *
*       Parameters:     void                                        *
*       Description:    This routine reads the busy status of the   *
*                       Hitachi HD44780 LCD controller.             *
********************************************************************/
#define _XTAL_FREQ 20000000
unsigned char BusyXLCD(void)
{
     
     __delay_us(10);
     return 0;  
        
}

