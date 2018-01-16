#include "common.h"
#include <stdlib.h>

#define PAYLOAD_SIZE 5
#define CHANNEL_NO 2

extern char doorOpenAngle;
extern char doorClosedAngle;
extern long sensorUpdatePeriod;

void Wireless_init(void)
{
#ifdef DOOR_SENSOR
    static const uint8_t TADDR[5] = "10001"; /* device address for door sensor */
#endif
#ifdef OUT_TEMP_SENSOR
    static const uint8_t TADDR[5] = "20001"; /* device address for outside sensor*/
#endif
   
    
    __delay_ms(100);
    
    RF_Init(); /* set CE and CSN to initialization value */
  
  RF_WriteRegister(RF24_RF_SETUP, RF24_RF_SETUP_RF_PWR_0|RF24_RF_SETUP_RF_DR_250);
  RF_WriteRegister(RF24_RX_PW_P0, PAYLOAD_SIZE); /* number of payload bytes we want to send and receive */
  RF_WriteRegister(RF24_RF_CH, CHANNEL_NO); /* set channel */
  
  /* Set RADDR and TADDR as the transmit address since we also enable auto acknowledgment */
  RF_WriteRegisterData(RF24_RX_ADDR_P0, (uint8_t*)TADDR, sizeof(TADDR));
  RF_WriteRegisterData(RF24_TX_ADDR, (uint8_t*)TADDR, sizeof(TADDR));

  /* Enable RX_ADDR_P0 address matching */
  RF_WriteRegister(RF24_EN_RXADDR, RF24_EN_RXADDR_ERX_P0); /* enable data pipe 0 */
  
  /* clear interrupt flags */
  RF_ResetStatusIRQ(RF24_STATUS_RX_DR|RF24_STATUS_TX_DS|RF24_STATUS_MAX_RT);

  RF_WriteRegister(RF24_EN_AA, RF24_EN_AA_ENAA_P0); /* enable auto acknowledge. RX_ADDR_P0 needs to be equal to TX_ADDR! */
  RF_WriteRegister(RF24_SETUP_RETR, RF24_SETUP_RETR_ARD_4000|RF24_SETUP_RETR_ARC_1); /* Important: need 750 us delay between every retry */
  RF_WriteRegister(RF24_CONFIG, RF24_EN_CRC|RF24_CRCO|RF24_PWR_UP|RF24_PRIM_TX); /* enable 2 byte CRC, power up and set as PTX */
  RF_WriteRegister(RF24_FEATURE, RF24_EN_DPL | RF24_EN_ACK_PAY); //enable dynamic payload length and payload in acknowledge packet
  RF_WriteRegister(RF24_DYNPD, RF24_DPL_P0);//enable DPL on pipe 0
  NRF_CE_SetLow();    /* Will pulse this later to send data */ 
}

void Wireless_sendData(char* payload)
{
   /* clear interrupt flags */
    RF_ResetStatusIRQ(RF24_STATUS_RX_DR|RF24_STATUS_TX_DS|RF24_STATUS_MAX_RT);
    RF_TxPayload(payload, PAYLOAD_SIZE); 
}


void Wireless_packageData(DOOR status,char openTime, float tempC)
{
    /* Packet contents
     * Byte 0 - B7-4 - door status B1 - compass Health B0 - temp sensor health
     * Byte 1 - Door OpenTime
     * Byte 2 - Temperature above decimal
     * Byte 3 - Temperature below decimal (1 dp)
     * Byte 4 - Supply voltage * 10
     
     */
    int t1=tempC;//Will attempt to convert to int to send
    int t2= (float)tempC * 10;
    char var;
    char data[PAYLOAD_SIZE];
    
    status<<=4;
    var=Diag_getSensorHealth();
    status|=var;
    
    data[0]=status;
    data[1]=openTime;
    data[2]=t1;
    t1=abs(t1);
    t2=abs(t2);
    data[3]=t2 - t1*10; 
    t1=(float)Sensor_getSupplyVoltage()*10;
    data[4]=t1;
    
    Wireless_sendData(data);
    while(NRF_IRQ_GetValue());//wait until data is sent - Active Low
    Wireless_checkDataReceived();
}


void Wireless_sleep()
{
    u8 reg;
    
     RF_ReadRegisterData(RF24_CONFIG , &reg, 1);
     reg&=0b11111101;
     RF_WriteRegister(RF24_CONFIG,reg);
    
}

void Wireless_wake()
{
     u8 reg;
    
     RF_ReadRegisterData(RF24_CONFIG , &reg, 1);
     reg|=0b00000010;
     RF_WriteRegister(RF24_CONFIG,reg);
     __delay_ms(2); //start up takes 1.5 ms
}

void Wireless_checkDataReceived(void)
{
    /* Received Packet content
     Byte 1 -B3 - Reset door pos B2 - Change update rate B1-change open time B0 - change close time
     Byte 2 - B7 - 0 - close time
     Byte 3 - B7 - 0 - Open Time
     Byte 4 - B7 - 0 - update period*/
    char channel;
    char data[4];
    do
    {    
        if(RF_DataIsReady(&channel))
        {
          RF_RxPayload(data, 4);
          if(data[0] & 0x01)
          {
           DOOR_update(doorOpenAngle,data[1]);
          }
          if(data[0] & 0x02)
          {
           DOOR_update(data[2],doorClosedAngle);
          }
          if(data[0] & 0x04)
          {
           sensorUpdatePeriod=data[3];
           sensorUpdatePeriod*=1000;//converting to milliseconds
          }
          if(data[0] & 0x08)
          {
            DOOR_resetPos();
          }
       
          //RF_TxPayload(data,sizeof(data));//send an acknowledgement of change  
          //while(NRF_IRQ_GetValue());//wait until data is sent - Active Low
        }
    }while(RF_DataIsReady(&channel));
   
    
}