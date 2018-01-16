// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// BMP280
// This code is designed to work with the BMP280_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Barometer?sku=BMP280_I2CSs#tabs-0-product_tabset-2

#include "common.h"
#include "bmp280.h"

  unsigned char b1[24];
  unsigned char data[8];
// BMP280 I2C address is 0x76(108)
#define Addr 0x76

unsigned int dig_T1; 
int dig_T2;
int dig_T3;
int dig_T4;

unsigned int dig_P1;
int dig_P2;
int dig_P3;
int dig_P4;
int dig_P5;
int dig_P6;
int dig_P7;
int dig_P8;
int dig_P9;

long adc_p; 
long adc_t;
double var1;
double var2;
double t_fine;
double cTemp;
double pressure ;


void BMP_init()
{
  char t;
  t=0x40;
  if(I2C_ByteWrite(Addr, &t, 0xF4)==false)//Setting the sensor is sleep mode
  {
      Diag_setTempSensorHealth(false);
      return;
  }
  t=0xE0;
  //HMC5883L_I2C_ByteWrite(Addr,&t,0xF5);//sensor will wake and sample every 4 seconds
  if(I2C_BufferRead(Addr, b1, 0x88, 24)==false) //reading calibration paramaters
  {
      Diag_setTempSensorHealth(false);
      return;
  }
  dig_T1 = ((u16)b1[0] & 0xFF) + (((u16)b1[1] & 0xFF) * 256);
  dig_T2 = (int)b1[2] + ((int)b1[3] * 256);
  dig_T3 = (int)b1[4] + ((int)b1[5] * 256);

  // pressure coefficients
//  dig_P1 = ((u16)b1[6] & 0xFF) + (((u16)b1[7] & 0xFF) * 256);
//  dig_P2 = (int)b1[8] + ((int)b1[9] * 256);
//  dig_P3 = (int)b1[10] + ((int)b1[11] * 256);
//  dig_P4 = (int)b1[12] + ((int)b1[13] * 256);
//  dig_P5 = (int)b1[14] + ((int)b1[15] * 256);
//  dig_P6 = (int)b1[16] + ((int)b1[17] * 256);
//  dig_P7 = (int)b1[18] + ((int)b1[19] * 256);
//  dig_P8 = (int)b1[20] + ((int)b1[21] * 256);
//  dig_P9 = (int)b1[22] + ((int)b1[23] * 256);
  Diag_setTempSensorHealth(true);
}

float BMP_readTemperature()
{
    char tmp=0;
    bool status;

    tmp=0x41;
    if(Diag_getTempSensorHealth()==false)
    {
        BMP_init();
    }
    I2C_ByteWrite(Addr, &tmp, 0xF4);//Setting the sensor to start temperature measurement
    do
    {
      status=HMC5883L_ReadBits(Addr, 0xF3 , 3, 1, &tmp);
    } while(tmp==0 && status==true);//wait for conversion
    if(status==false)
    {
        Diag_setTempSensorHealth(false);
        return -55;
    }
    
    if(I2C_BufferRead(Addr, data, 0xF7, 8)==false)
    {
       Diag_setTempSensorHealth(false);
       return -55; 
    }

  // Convert pressure and temperature data to 19-bits
  //adc_p = (((long)(data[0] & 0xFF) * 65536) + ((long)(data[1] & 0xFF) * 256) + (long)(data[2] & 0xF0)) / 16;
  adc_t = (((long)(data[3] & 0xFF) * 65536) + ((long)(data[4] & 0xFF) * 256) + (long)(data[5] & 0xF0)) / 16;

  // Temperature offset calculations
  var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
  var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
                 (((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
  t_fine = (long)(var1 + var2);
  cTemp = (var1 + var2) / 5120.0;
  // double fTemp = cTemp * 1.8 + 32;

  // // Pressure offset calculations
//   var1 = ((double)t_fine / 2.0) - 64000.0;
//   var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
//   var2 = var2 + var1 * ((double)dig_P5) * 2.0;
//   var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
//   var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
//   var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
//   p = 1048576.0 - (double)adc_p;
//   p = (p - (var2 / 4096.0)) * 6250.0 / var1;
//   var1 = ((double) dig_P9) * p * p / 2147483648.0;
//   var2 = p * ((double) dig_P8) / 32768.0;
//   pressure = (p + (var1 + var2 + ((double)dig_P7)) / 16.0) / 100;
  Diag_setTempSensorHealth(true);
  return(cTemp);
}
