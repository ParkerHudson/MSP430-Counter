#include <msp430.h> 



//Digit configuration, make sure segments h-a are connected to PORT1 pins 7-0

//also besides disigts 0-9, you have single segments abcdefg.

int LEDS[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67,0x01,0x02,04,0x08,0x10,0x20,0x40,0x80};


int switches=0;

int leftdigit=0, rightdigit=0;

int pleftdigit=0, prightdigit=0;    //preset values

int flag=0;

int countflag = 0; //flag for the two up/down counters that start from 99/00


int main(void)

{

//  WDTCTL = WDTPW | WDTHOLD;       // Stop watchdog timer

    BCSCTL2 |= DIVS_2;              // DIVS_0; DIVS_1; DIVS_2; DIVS_3;

    WDTCTL = WDT_MDLY_0_5;          // WDT_MDLY_32; WDT_MDLY_8; WDT_MDLY_0_5;

    IE1 |= WDTIE;


    P1OUT = 0x00;                   // port 1 out default 00

    P1DIR = 0xff;                   // port 1 all output

    P2DIR = 0x03;                   // port 2 all inputs, except BIT0 and BIT1


    __enable_interrupt();


for (;;)

{

//  switches =  P2IN; //if wired as active low

    switches = ~P2IN; //if wired as active high


    //the displayed numbers while we keep multiplexing of the display relatively faster


    //check switches for 000 --> Counter resets to 00

    if (((switches & BIT5) != BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) != BIT3))

    {leftdigit=0; rightdigit=0; }


    //check switches for 001 --> right digit count up

    if (((switches & BIT5) != BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) == BIT3))

    {rightdigit++; if (rightdigit >=10) {rightdigit=0;} }


    //check switches for 010 --> left digit count up

    if (((switches & BIT5) != BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) != BIT3))

    {leftdigit++ ; if (leftdigit >=10) {leftdigit=0;} }


    //check switches for 011 --> Right and left digits both hold values (preset value)

    if (((switches & BIT5) != BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) == BIT3))

    {pleftdigit=leftdigit; prightdigit=rightdigit; }


    // SW-321 = 111: Counter cycles up from the preset value to 99

    if (((switches & BIT5) == BIT5) && ((switches & BIT4) == BIT4) && ((switches & BIT3) == BIT3))

    // modify this section, for now you have a rotating pattern

    {

        rightdigit++;

        if(rightdigit >= 10)

        {

            rightdigit = 0;

            leftdigit++;

            if(leftdigit >= 10)

               {

               leftdigit = 0;

            }

        }


    }


    // SW-321 = 100: Counter cycles down from the preset value to 00

    if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) != BIT3))

    // modify this section,

    {

        rightdigit--;

        if(rightdigit < 0) {

            rightdigit = 9;

            leftdigit--;

            if(leftdigit < 0) {

                leftdigit = 9;

            }

        }


    }

    // SW-321 = 101: Pause Counting

    if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4) && ((switches & BIT3) == BIT3))

    {pleftdigit = leftdigit; prightdigit = rightdigit;}


    //check switches for 110: Pause Counting

    if (((switches & BIT5) == BIT5) && ((switches & BIT4) != BIT4)&& ((switches & BIT3) != BIT3))

    {pleftdigit = leftdigit; prightdigit = rightdigit;}


    else {

        countflag = 0;  //Reset the count flag if this command isn't being used

    }


    // this delay determins the speed of chaning the number bing displayd

    __delay_cycles (300000);


} // end of for loop

} // end of main



// WDT interrupt service routine

#pragma vector=WDT_VECTOR

__interrupt void WDT(void)

{

    //This executes everytime there is a timer interrupt from WDT

    //The fequency of this interrupt controls the flickering of display

    //The right and left digits are displayed alternatively

    //Note that both digits must be turned off to avoid aliasing



    //Display code for Common-Anode display

    P1OUT= 0; P2OUT=0;

    __delay_cycles (100);

    if (flag == 0) {P2OUT= BIT0; P1OUT= LEDS[leftdigit];  flag=1;}

    else           {P2OUT= BIT1; P1OUT= LEDS[rightdigit]; flag=0;}

    __delay_cycles (100);


    //Display code for Common-Cathod display

//  P1OUT= 0xFF; P2OUT=0xFF;

//    __delay_cycles (1000);

//    if (flag == 0) {P2OUT &= ~BIT0; P1OUT= ~LEDS[leftdigit];  flag=1;}

//    else           {P2OUT &= ~BIT1; P1OUT= ~LEDS[rightdigit]; flag=0;}

//    __delay_cycles (100);



}
