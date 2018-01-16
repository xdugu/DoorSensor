#include "common.h"

char I2C_ByteWrite(u8 slaveAddr, u8* pBuffer, u8 WriteAddr)
{
    u8 data[2];
    I2C2_MESSAGE_STATUS status=I2C2_MESSAGE_COMPLETE;
    
    data[0]= WriteAddr;
    data[1]= *pBuffer;
    
    
    I2C2_MasterWrite(data,2,slaveAddr, &status);
    Timeout_setTimeout(200);//set timeout for 200 milliseconds
         // wait for the message to be sent or status has changed.
    while(status == I2C2_MESSAGE_PENDING && Timeout_hasExpired()==false);
    
    if (status == I2C2_MESSAGE_COMPLETE)
    {
        return 1;
    }
    else
    {
        return 0;
    }
   
}


 
char I2C_BufferRead(u8 slaveAddr, u8* pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
    I2C2_MESSAGE_STATUS status=I2C2_MESSAGE_COMPLETE;

                    I2C2_MasterWrite(&ReadAddr, 1,slaveAddr, &status);
                    Timeout_setTimeout(200);//set timeout for 200 milliseconds
                    
                     // wait for the message to be sent or status has changed.
                    while(status == I2C2_MESSAGE_PENDING && Timeout_hasExpired()==false);

                     if (status != I2C2_MESSAGE_COMPLETE)
                     {
                           return 0;
                     }

                            // write one byte to EEPROM (2 is the count of bytes to write)
                     I2C2_MasterRead( pBuffer,NumByteToRead,slaveAddr,&status);
                     Timeout_setTimeout(200);//set timeout for 200 milliseconds
                            // wait for the message to be sent or status has changed.
                     while(status == I2C2_MESSAGE_PENDING && Timeout_hasExpired()==false);

                     if (status == I2C2_MESSAGE_COMPLETE)
                         return 1;
                     else
                         return 0;                   

}