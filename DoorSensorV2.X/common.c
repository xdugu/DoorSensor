#include "mcc_generated_files/mcc.h"
#include "hmc5883l.h"
#include "common.h"

extern volatile uint16_t timer1ReloadVal;

void setSleepTime(u16 timeSec)
{
    timer1ReloadVal=0x6000;//65536-(4096*timeSec);
}

int limit(int val, int min, int max){
    if(val<min)
        return min;
    if(val>max)
        return max;
      return val;
}