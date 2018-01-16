/* 
 * File:   sensors.h
 * Author: Yayra Adugu
 *
 * Created on December 21, 2017, 7:32 PM
 */

#ifndef SENSORS_H
#define	SENSORS_H

#ifdef	__cplusplus
extern "C" {
#endif

    float Sensor_getSupplyVoltage();
    void Sensor_peripheralSleep(void);
    void Sensor_peripheralWake(void);



#ifdef	__cplusplus
}
#endif

#endif	/* SENSORS_H */

