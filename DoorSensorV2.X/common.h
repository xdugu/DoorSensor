/* 
 * File:   common.h
 * Author: Yayra
 *
 * Created on 25 July 2015, 15:44
 */

#ifndef COMMON_H
#define	COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
#include "MPU6050.h"
#include "hmc5883l.h"
#include "nrf24l01.h"
#include "bmp280.h"
#include "diagnostics.h"
#include "sensors.h"
    
    
#define true 1
#define false 0

#define ON  1
#define OFF 0

#define ENABLED  1
#define DISABLED 0
    
//#define OUT_TEMP_SENSOR   
//#define DOOR_SENSOR
#define INDOOR_TEMP_SENSOR
    
#ifdef DOOR_SENSOR
#define WAKE_UP_PERIOD_MS 10000
#define TEMPERATURE_UPDATE_RATE 600000

#define DOOR_OPEN_DEBOUNCE 0
#define DOOR_CLOSED_DEBOUNCE 20000
#endif
    
#ifdef OUT_TEMP_SENSOR  
#define WAKE_UP_PERIOD_MS       10000 //max is 16000
#define TEMPERATURE_UPDATE_RATE 600000
#endif   
    
#ifdef INDOOR_TEMP_SENSOR
#define WAKE_UP_PERIOD_MS 10000
#define TEMPERATURE_UPDATE_RATE 600000
#endif
    
#define DOOR_OPEN_ANGLE  15
#define DOOR_CLOSE_ANGLE 5

#define PI 3.14

 
typedef enum 
{
   DOOR_OPEN=1,
   DOOR_CLOSED=0,
   DOOR_ERROR=2
}DOOR;

//#define _XTAL_FREQ 8000000
   
  typedef unsigned char uint8_t;
  typedef unsigned char bool;
  typedef unsigned char u8;
  typedef signed char s8;
  typedef unsigned int u16;
  typedef signed int s16;
  typedef signed long s32;
  typedef unsigned long u32;



  //Functions protoypes
  void DOOR_init();
  void DOOR_update(char openAngle, char closedAngle);
  DOOR DOOR_run();
  DOOR DOOR_resetPos();
  DOOR DOOR_getState();

void Wireless_init(void);
void Wireless_sendData(char* payload, char);
void Wireless_sleep();
void Wireless_wake();
void Wireless_packageData(DOOR status,char openTime, float tempC);
void Wireless_checkDataReceived(void);
char Wireless_determineTransmitPower(char initialise);

void Timeout_increment();
void Timeout_setTimeout(u16 val);
bool Timeout_hasExpired();
void Timeout_updateCurrentTime(void);
unsigned long Timeout_getGlobalCounter();

char I2C_ByteWrite(u8 slaveAddr, u8* pBuffer, u8 WriteAddr);
char I2C_BufferRead(u8 slaveAddr, u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);

void Storage_load(u8 *openAngle,u8 *closeAngle);
void Storage_save(u8 openAngle,u8 closeAngle);

void setSleepTime(u16 timeSec);

int limit(int val, int min, int max);

#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

