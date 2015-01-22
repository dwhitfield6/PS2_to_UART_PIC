/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---         -------------------------------------------------------
 * 01/09/15     C           Written.
 *
/******************************************************************************/

/******************************************************************************/
/* Contains Interrupt routine
 *
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "PS_2.h"
#include "UART.h"
#include "FLASH.h"
#include "user.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Note that some PIC16's
 * are baseline devices.  Unfortunately the baseline detection macro is
 * _PIC12 */
#ifndef _PIC12

extern unsigned int PS_2_Read_Data_FirstTEMP;
extern unsigned int PS_2_Read_Data_SecondTEMP;
extern unsigned int PS_2_Read_Data_ThirdTEMP;
extern unsigned int PS_2_Read_Data_ForthTEMP;
extern unsigned char PS_2_bits;
extern unsigned int Read_Timer;
extern unsigned char Alarm;
extern unsigned int SinLEDtimer;

unsigned char blinkdelay =0;

void interrupt isr(void)
{
    unsigned char rx;
    unsigned char Rx_fault =0;   

    if(IOCAF & CLK)
    {
        //Change on PS_2 clk pin
        LATC |= KeyLED;
        if(PS_2_bits <PS_2_NUMBITS_4)
        {
            if(PS_2_bits <PS_2_NUMBITS )
            {
                PS_2_Read_Data_FirstTEMP <<= 1;
                if(READ_PS_2_PIN(DATA))
                {
                    PS_2_Read_Data_FirstTEMP++;
                }
                PS_2_Read_Data_SecondTEMP = 0;
                PS_2_Read_Data_ThirdTEMP = 0;
            }
            else if(PS_2_bits <PS_2_NUMBITS_2 )
            {
                PS_2_Read_Data_SecondTEMP <<= 1;
                if(READ_PS_2_PIN(DATA))
                {
                    PS_2_Read_Data_SecondTEMP++;
                }
            }
            else if(PS_2_bits <PS_2_NUMBITS_3 )
            {
                PS_2_Read_Data_ThirdTEMP <<= 1;
                if(READ_PS_2_PIN(DATA))
                {
                    PS_2_Read_Data_ThirdTEMP++;
                }
            }
            else
            {
                PS_2_Read_Data_ForthTEMP <<= 1;
                if(READ_PS_2_PIN(DATA))
                {
                    PS_2_Read_Data_ForthTEMP++;
                }
            }
            PS_2_bits++;
            Read_Timer = 0;
        }
        IOCAF &= ~CLK;
    }
    else if (IOCAF & DATA)
    {
        IOCAF &= ~DATA;
    }
    else if (PIR1bits.RCIF)
    {
        //rx interrupt
        LATC |= SinLED;
        SinLEDtimer = 0;
        Rx_fault = 0;
        if(RC1STAbits.FERR)
        {
            Rx_fault =1;
        }
        rx = ReadUSART(); //read the byte from rx register
        if(Rx_fault == 1)
        {
            if(rx == 0)
            {
                UART_send_break();
            }
        }
        else
        {
            UARTchar(rx, YES ,RC1STAbits.RX9D);
        }
        PIR1bits.RCIF = 0;
    }
    else if(PIR1bits.TMR2IF)
    {
        //Not used in this version
        PIE1bits.TMR2IE = 0;//disable timer 2 interupt
        blinkdelay++;LATC ^= pwrLED;
        if(blinkdelay > BlinkDelay)
        {
            if(Alarm)
            {
                
            }
            else
            {
                LATC |= pwrLED;
            }
            blinkdelay =0;
        }
        PIR1bits.TMR2IF =0;
        //PIE1bits.TMR2IE = 1;//enable timer 2 interupt
    }
    else
    {
        /* Unhandled interrupts */
    }
}
#endif

