#include "common.h"

volatile unsigned long globalCounter=0;
unsigned long curr;
u16 timeOut;

void Timeout_increment(void)
{
    globalCounter+=WAKE_UP_PERIOD_MS;
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