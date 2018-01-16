#include "common.h"
#include "sensors.h"

#define ADC_MULTIPLIER 1.024 * 1024 //10 bit ADC conversation result 

float Sensor_getSupplyVoltage()
{
    u16 res;
    float voltage;
    
    Sensor_peripheralWake();
    while(!FVR_IsOutputReady());//wait for FVR to stabalise
    ADC_SelectChannel(channel_FVR);
    ADC_StartConversion();
    while(!ADC_IsConversionDone());//waiting for conversion
    Sensor_peripheralSleep();
    res=ADC_GetConversionResult();
    
    voltage= (float) ADC_MULTIPLIER/(float)res;   
    return(voltage);
    
    
    
}

void Sensor_peripheralSleep(void)
{
    ADCON0bits.ADON=0;
    FVRCONbits.FVREN=0;
}

void Sensor_peripheralWake(void)
{
    ADCON0bits.ADON=1;
    FVRCONbits.FVREN=1;
}
