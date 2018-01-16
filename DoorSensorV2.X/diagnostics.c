#include "common.h"
#include "diagnostics.h"

typedef struct
{
    bool tempSensorHealth;
    bool compassHealth;
    
}SYSTEM;


//Initialise both Sensors as true
SYSTEM sensorHealth = {1,1};

void Diag_setTempSensorHealth (bool health)
{
    sensorHealth.tempSensorHealth=health;  
}

void Diag_setCompassHealth (bool health)
{
    sensorHealth.compassHealth=health;
}

bool Diag_getTempSensorHealth (void)
{
    return (sensorHealth.tempSensorHealth);  
}

bool Diag_getCompassHealth (void)
{
    return (sensorHealth.compassHealth);
}

char Diag_getSensorHealth (void)
{
    /*B1 - compass  B0 - temp sensor*/
    char var=0;
    
    var|=sensorHealth.compassHealth;
    var<<=1;
    var|=sensorHealth.tempSensorHealth;
    
    return var;
}