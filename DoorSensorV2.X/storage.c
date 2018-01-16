#include "common.h"

#define STORAGE_STATUS_LOC      0
#define DOOR_OPEN_STORE_LOC     1
#define DOOR_CLOSED_STORE_LOC   2
#define UPDATE_PERIOD_STORE_LOC 3

void Storage_load(u8 *openAngle,u8 *closeAngle)
{
    char byte;
    
    byte=DATAEE_ReadByte(STORAGE_STATUS_LOC );
    if(byte==1)
    {
        *openAngle=DATAEE_ReadByte(DOOR_OPEN_STORE_LOC);
        *closeAngle=DATAEE_ReadByte(DOOR_CLOSED_STORE_LOC);
    }
    else
    {
        DATAEE_WriteByte(DOOR_OPEN_STORE_LOC, DOOR_OPEN_ANGLE);
        DATAEE_WriteByte(DOOR_CLOSED_STORE_LOC, DOOR_CLOSE_ANGLE);
        DATAEE_WriteByte(STORAGE_STATUS_LOC , 1);
        *openAngle=DOOR_OPEN_ANGLE;
        *closeAngle=DOOR_CLOSE_ANGLE;
    }
     
}


void Storage_save(u8 openAngle,u8 closeAngle)
{
    DATAEE_WriteByte(DOOR_OPEN_STORE_LOC, openAngle);
    DATAEE_WriteByte(DOOR_CLOSED_STORE_LOC, closeAngle);
    DATAEE_WriteByte(STORAGE_STATUS_LOC , 1);
}