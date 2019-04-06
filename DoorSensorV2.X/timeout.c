#include "common.h"

volatile unsigned long globalCounter=0;
unsigned long curr;
u16 timeOut;


void Timeout_increment(void)
{
    globalCounter+=WAKE_UP_PERIOD_MS;
}

void Timeout_updateCurrentTime(void)
{
    unsigned long var = 10;//WAKE_UP_PERIOD_MS/1000;
    var = TMR1_ReadTimer();
    var= var - 0x6000;
    globalCounter += ((var*1000)/4096);  
}

unsigned long Timeout_getGlobalCounter()
{
    return globalCounter;
}

void Timeout_setTimeout(u16 val)
{
    timeOut=val;
    curr=globalCounter;
}

bool Timeout_hasExpired()
{
    if(globalCounter-curr>timeOut)
        return true;
    else
        return false;
    
}