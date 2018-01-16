#include "common.h"
#include "stdlib.h"

char doorOpenAngle;
char doorClosedAngle;
DOOR doorState=DOOR_CLOSED;
bool initialised=false;
int referenceDirection;

void DOOR_init()
{
    //Storage_save(8,3);
    Storage_load(&doorOpenAngle, &doorClosedAngle);
}

void DOOR_update(char openAngle, char closedAngle)
{
    doorOpenAngle=openAngle;
    doorClosedAngle=closedAngle;
    Storage_save(openAngle,closedAngle);
}


DOOR DOOR_run()
{
#ifdef DOOR_SENSOR
    int dir;
    
    if(Diag_getCompassHealth()==false)//if faulty, try to re-initialise
    {
      HMC5883L_Initialize();
    }
    dir=HMC5883L_GetCompassDirection();
    
    if(dir<0)
    {
        Diag_setCompassHealth(false);//sensor is not working
        return DOOR_ERROR;
    }
    else
        Diag_setCompassHealth(true);//sensor is working
    
    if(initialised==false)
    {
          referenceDirection=dir;
          doorState=DOOR_CLOSED;
          initialised=true;
    }
    
    else if(doorState==DOOR_CLOSED && abs(referenceDirection-dir)>doorOpenAngle)
    {
        doorState=DOOR_OPEN;
    }
    
    else if(doorState==DOOR_OPEN && abs(referenceDirection-dir)<doorClosedAngle)
    {
        doorState=DOOR_CLOSED;
        return(doorState);
    }
    
#endif
    return(doorState);  
}

DOOR DOOR_resetPos()
{
    initialised=false;
    return(DOOR_CLOSED);
}

DOOR DOOR_getState()
{
    return(doorState);
}