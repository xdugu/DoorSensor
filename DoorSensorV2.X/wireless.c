#include "common.h"
#include <stdlib.h>

#define PAYLOAD_SIZE 7
#define CHANNEL_NO 2
#define MAX_TRANSMIT_POWER_LEVEL 3
#define MIN_TRANSMIT_POWER_LEVEL 0

extern char doorOpenAngle;
extern char doorClosedAngle;
extern long sensorUpdatePeriod;

char currentTransmitPower = 0;

    
void Wireless_changeTransmitPower(char power);
char Wireless_isDataTransmitSuccessful();


void Wireless_init(void)
{
#ifdef DOOR_SENSOR
    static const uint8_t TADDR[5] = "20001"; /* device address for door sensor */
#endif
#ifdef OUT_TEMP_SENSOR
    static const uint8_t TADDR[5] = "10001"; /* device address for outside sensor*/
#endif

#ifdef INDOOR_TEMP_SENSOR
    static const uint8_t TADDR[5] = "20001"; /* device address for inside sensor with no door logic*/
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

// called in main function. Return whether data should be retransmitted or not
//Adjust the data transmission power based on the level required to successfully send a message
char Wireless_determineTransmitPower(char initialise)
{
    char testPayload = 0xFF;
    char latestResult;
    static char numOfTransmits = 0;
    static char numOfSuccessfulTransmits = 0;
    
    if(initialise){
        currentTransmitPower = 0;
        //We will loop and send data at increasing power level to check the optimum
        //transmit power
        for(;currentTransmitPower <= MAX_TRANSMIT_POWER_LEVEL; currentTransmitPower++){
            Wireless_changeTransmitPower(currentTransmitPower);           
            Wireless_sendData(&testPayload, 1);
            while(NRF_IRQ_GetValue()); //wait to get results
            //if successfully sent, keep at appropriate power level
            if(Wireless_isDataTransmitSuccessful())
                break;
        }  
        currentTransmitPower = limit( currentTransmitPower, MIN_TRANSMIT_POWER_LEVEL, MAX_TRANSMIT_POWER_LEVEL);
    }else{
        // Get data from the previous data transmit
        latestResult = Wireless_isDataTransmitSuccessful();
        numOfTransmits++;
        numOfSuccessfulTransmits+= latestResult;
        //check if at current power level, we have a good successful transmission rate
        if(numOfTransmits>=5){
            // 80% sucessful rate or higher? reduce power level on next transmit
            if(currentTransmitPower > MIN_TRANSMIT_POWER_LEVEL && latestResult && numOfSuccessfulTransmits >=4)
                Wireless_changeTransmitPower(--currentTransmitPower);          
            //reset transmission quality parameters
             numOfTransmits = 0;
             numOfSuccessfulTransmits = 0;
             if(!latestResult && currentTransmitPower < MAX_TRANSMIT_POWER_LEVEL){
                 //if transmission was not successful increase power level and 
                 //try again
               Wireless_changeTransmitPower(++currentTransmitPower);               
               return true;
             } else return false;
            
        }else{
            //if count not reached
            if(!latestResult && currentTransmitPower < MAX_TRANSMIT_POWER_LEVEL){
                //if the data was not transmitted successfully but was not sent at max power level
                Wireless_changeTransmitPower(++currentTransmitPower);               
                return true;//it is required to resend data
            } else 
                return false;//we transmitted at max power level so its not
            //required to retransmit the data;            
        }
    }
    //latestResult contains state of whether data was sucessfully transmitted or not. If we get here in the code
    //we will return the opposite of this variable to tell the data transmission whether to/not to resend the data
    return !latestResult;
}
void Wireless_changeTransmitPower(char power){
    /*Wireless_changeTransmitPower changes the transmit power on the nrf device
     choose power level between 0 and 3 - 3 is the highest power level*/
    u8 val;
    
    //Ensure that rogue function does not send an invalid value
    power = limit(power, 0, 3);
    RF_ReadRegisterData(RF24_RF_SETUP, &val, 1);
    //Clearing bits we want to write to
    val &= 0b11111001;
    val |= (power <<1);
    RF_WriteRegister(RF24_RF_SETUP, val);
}
void Wireless_sendData(char* payload, char length)
{
    char status;
   /* clear interrupt flags */
    RF_ResetStatusIRQ(RF24_STATUS_RX_DR|RF24_STATUS_TX_DS|RF24_STATUS_MAX_RT);
    status =  RF_GetStatus();
    RF_TxPayload(payload, length); 
}


void Wireless_packageData(DOOR status,char openTime, float tempC)
{
    /* Packet contents
     * Byte 0 - B5-4 - door status B3-2 - Power level B1 - compass Health B0 - temp sensor health
     * Byte 1 - Door OpenTime
     * Byte 2 - Temperature above decimal
     * Byte 3 - Temperature below decimal (1 dp)
     * Byte 4 - Supply voltage * 10
     
     */
    tempC+=50;//Will attempt to convert to int to send
    tempC*=10;
    
    unsigned int t1 = tempC;
    char var;
    char data[PAYLOAD_SIZE];
    
    status<<=4;
    var=Diag_getSensorHealth();
    status|=var;
    status| = (currentTransmitPower<<2);
    
    
    data[0]=status;
    data[1]=openTime;
    data[2]=t1>>8;
    data[3]= t1 & 0xFF; 
    t1=(float)(Sensor_getSupplyVoltage()-1)*100;
    data[4]=t1;
    t1 = sensorUpdatePeriod/1000;
    //preparing to send next time data to be expected
    data[5] = t1 >>8;
    data[6] = t1 & 0xFF;
    do{
        Wireless_sendData(data, PAYLOAD_SIZE);
        while(NRF_IRQ_GetValue());//wait until data is sent - Active Low
    }
    while(Wireless_determineTransmitPower(false));
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

//This function returns true if the data transmitted was successfully acknowledged
char Wireless_isDataTransmitSuccessful(){
   char status =  RF_GetStatus();
   if(status & RF24_STATUS_TX_DS)
       return true;
   else
       return false;
}

void Wireless_checkDataReceived(void)
{
    /* Received Packet content
     Byte 1 -B3 - Reset door pos B2 - Change update rate B1-change open time B0 - change close time
     Byte 2 - B7 - 0 - close time
     Byte 3 - B7 - 0 - Open Time
     Byte 4 - B7 - 0 - update period*/
    char channel;
    char data[2];
    do
    {    
        if(RF_DataIsReady(&channel))
        {
          RF_RxPayload(data, 2);
          sensorUpdatePeriod = data[0];
          sensorUpdatePeriod << = 8;
          sensorUpdatePeriod |= data[1];
          sensorUpdatePeriod *=1000;//converting to milliseconds
       
          //RF_TxPayload(data,sizeof(data));//send an acknowledgement of change  
          //while(NRF_IRQ_GetValue());//wait until data is sent - Active Low
        }
    }while(RF_DataIsReady(&channel));
   
    
}
