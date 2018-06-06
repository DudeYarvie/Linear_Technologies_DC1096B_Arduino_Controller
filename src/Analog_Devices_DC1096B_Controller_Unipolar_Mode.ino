/*
          Analog_Devices_DC1096B_Controller_Unipolar_Mode.ino 
Author: Jarvis Hill (hilljarvis@gmail.com)
Date: 06-JUNE-2018
Purpose: Use an Arduino UNO (Atmega328) to control and drive the oout of the Analog Devices DC1096B demo board set for unipolar output configuration (0V to +5V).

Usage:
         1. Connect DC1096B demo board to Arduino UNO per the connection diagram in sketch folder.
                          
References: DC1096B user manaul, LTC2642ACDD-16 16-bit DAC datasheet
*/


/*INCLUDES*/
#include <SPI.h>                //Required for SPI 


/*GLOBALS*/

/*LTC2642ACDD-16 DAC control signals*/
#define nCS_DAC    PIN4       
#define nCS_DAC_ddr DDD4   

/*GPIO states*/
#define ON  true
#define OFF false

/*SPI Settings*/
SPISettings settingsA(16000000, MSBFIRST, SPI_MODE0); 

/*LTC2642ACDD-16 DAC*/
uint16_t dac_code = 0;
uint8_t lsdac_byte0, lsdac_byte1;


/*FUNCTIONS*/

/**********************************************************************
 * init_DAC_io
 * Purpose: Configure I/O for ADC AD4020 and DAC AD5791 communications
 **********************************************************************/
void init_DAC_io(){
  //intialize DC1096B control signals (all SPI signals are handled by SPI.begin() function in setup)
  DDRD = (1<<nCS_DAC);            //Set signals to output 

  PORTD = (ON << nCS_DAC_ddr);    //Init pin logic levels, OFF = LOW, ON = HIGH 
}



/************************************************************************************************************************************
 * LTC_DAC_Load
 * Purpose: Wite data to DC1096B DAC. DAC out = Vref x (DAC code/2^16)
 * Note: 2 bytes (16-bits) is required to write DAC. Data is shifted into DAC on CLK rising edge.  
*************************************************************************************************************************************/
void LTC_DAC_Load(uint8_t b0, uint8_t b1){

    //Write data to DAC
    PORTD &= ~(ON << nCS_DAC);        //DAC nCS_DAC signal LOW to enable loading 
    
    SPI.beginTransaction(settingsA);  //Start SPI transaction using desired SPI settings and driving Arduino SS pin LOW but this code is not using it  
    SPI.transfer(b0);                 //8 clk cycles, 8 bits
    SPI.transfer(b1);                 //8 clk cycles, 8 bits
    
    //End data load
    PORTD |= (ON << nCS_DAC);         //Idle DAC nCS_DAC signal HIGH to disable loading

    SPI.endTransaction();             //Stops SPI transaction and brings Arduino SS pin HIGH but this code is not using it

}



/*SETUP*/
void setup() {
 
  /*Init SPI*/
  SPI.begin();
  
  /*Open Serial connection for debbuging*/
  Serial.begin(2000000);

  /*Init DC1096B I/O*/
  init_DAC_io();

  delay(1000);              //delay
  
}



/*MAIN PROGRAM*/
void loop() {

  /*Stream data to DAC*/
  dac_code = uint16_t((4.36/5.0)*65536.0);   //Calculate DAC code base on desired DAC output voltage, DAC code = (dac_output_voltage/VREF)*2^16 
  lsdac_byte0 = (dac_code & 0xFF00) >> 8;     //Parse DAC code into a HIGH byte and LOW byte     
  lsdac_byte1 = (dac_code & 0xFF);            
  LTC_DAC_Load(lsdac_byte0,lsdac_byte1);      //Write DAC MSB first
  Serial.println(dac_code);                   //Print dac_code to serial terminal for debugging
    
}



