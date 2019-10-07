/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 3.15.0
        Device            :  PIC16LF1829
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.20
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"
#include "common.h"
/*
                         Main application
 */


//void packageData(char* data,DOOR status,char,float tmp);
void setSleepTime(u16 timeSec);

long sensorUpdatePeriod = TEMPERATURE_UPDATE_RATE;

void main(void)
{
    DOOR doorStatus=DOOR_CLOSED,prevDoorStatus=DOOR_CLOSED;
    unsigned long doorClosedUpdateCounter=0;
    unsigned long doorOpenTime=0;
    unsigned int dur=0;
   
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    BUTTON_SetPullup();
    __delay_ms(1000);
   Sensor_peripheralSleep();//all mcu sensing peripherals to be set off
   Wireless_init();
   BMP_init();
#ifdef DOOR_SENSOR
   //HMC5883L_Initialize
#endif
#ifdef OUT_TEMP_SENSOR 
   setSleepTime(WAKE_UP_PERIOD_MS/1000);   
#endif
   setSleepTime(WAKE_UP_PERIOD_MS/1000); 
   DOOR_init();
   LED_SetHigh();
   __delay_ms(500);
   LED_SetLow();
    Wireless_wake();
    Wireless_determineTransmitPower(true);
    Wireless_packageData(doorStatus,doorOpenTime/1000,BMP_readTemperature());           
    Wireless_sleep();
   
    while (1)
    {
        Timeout_updateCurrentTime();
        if(BUTTON_GetValue()==0)//i.e. reset button pressed
        {
            //doorStatus=DOOR_resetPos();
            LED_SetHigh();
            __delay_ms(500);
            LED_SetLow();
            Wireless_wake();
            dur = Timeout_getGlobalCounter() - doorOpenTime;
            Wireless_packageData(doorStatus,dur/1000,BMP_readTemperature());           
            Wireless_sleep();
        }
        else
           doorStatus=DOOR_run();
        
        
        if((doorStatus!=prevDoorStatus && doorStatus!=DOOR_ERROR))
        {
            Wireless_wake();
            if(doorStatus==DOOR_OPEN)
            {
               Wireless_packageData(doorStatus,0,BMP_readTemperature()); 
               doorOpenTime= Timeout_getGlobalCounter();
              // LED_SetHigh();
              // __delay_ms(50);
             //  LED_SetLow(); 
            }
            else
            {
                dur = Timeout_getGlobalCounter() - doorOpenTime;
                Wireless_packageData(doorStatus,dur/1000,BMP_readTemperature()); 
                doorOpenTime = Timeout_getGlobalCounter();
            }
            Wireless_sleep();
            doorClosedUpdateCounter=0;
        }
        else if(doorStatus==DOOR_OPEN)
        {
            dur = Timeout_getGlobalCounter()-doorOpenTime;
            if(dur>=60000)//in case the open time is more than 60 seconds, send update
            {
                 Wireless_wake();
                 Wireless_packageData(doorStatus,dur/1000,BMP_readTemperature());           
                 Wireless_sleep();///Reset open time
                 doorClosedUpdateCounter=0;
                 doorOpenTime=Timeout_getGlobalCounter();
            }
            //LED_SetHigh();
        }
         else if(doorClosedUpdateCounter>=sensorUpdatePeriod)
        {
            Wireless_wake();
            Wireless_packageData(doorStatus,0,BMP_readTemperature()); 
            Wireless_sleep();
            doorClosedUpdateCounter=0;
            doorOpenTime = Timeout_getGlobalCounter();
        }
        else
        {
           // LED_SetLow();
             doorOpenTime=Timeout_getGlobalCounter();
        }
              
        prevDoorStatus=doorStatus;
        doorClosedUpdateCounter+=WAKE_UP_PERIOD_MS;
        SLEEP();       
    }
}
/**
 End of File
*/




void doSleep(void)
{
     
     SLEEP();
     
}
