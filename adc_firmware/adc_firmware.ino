/*
 * Copyright 2013 Barnaby Shearer <b@Zi.iS>
 * License GPLv2
 */

#include <SPI.h>
#include <ADS124x.h>
#include <avr/power.h>
#include <EEPROM.h>

const unsigned char mycommand[] = "aTBTEMP-----";
const unsigned char myreply[] = "a--T";

//PINS
#define ADC_CS       9
//Other ADC pins are the 10,11,12,13 SPI pins
#define RADIO        8

#define SETTING_VERSION "FIJ3"

struct SETTINGS {
  unsigned long sample_interval;
  unsigned int samples_per_reading;
  unsigned long discard_min;
  unsigned long discard_max;
  unsigned long scale_min;
  unsigned long scale_range;
} settings = {
  300,
  5,
  6710886, //~0C 1000/((1/16/0x7fffff)/0.000050)
  8270496, //~60C 1232.4/((1/16/0x7fffff)/0.000050)
  6710886, //~0C
  1559610  //~60C - 0C
};

void setup() {
  Serial.begin(9600);  
    
  //Enable radio
  pinMode(RADIO, OUTPUT);
  digitalWrite(RADIO, HIGH);

  //Power down unused stuff (really sleeping dosn't help as we need the acurate clock running, and the support components make any other savings mute).
  power_adc_disable();
  power_timer2_disable();
  power_twi_disable();  

  ADS124X.begin(ADC_CS, SPI_CLOCK_DIV16); // 1Mhz
  
  //Configure ADC
  setupADC();

}

int inbyte=0;
unsigned char incommand[12] = {};
int incount=0;
int outbyte=0;

void loop() {
  if(Serial.available() >0) {
    inbyte = Serial.read();
    if(inbyte == 'a') {
      //new command
      incount=0;
    }
    incommand[incount++] = inbyte;
    
    if (incount==11) {
      //complete command received
      processcommand();
    }
  }

}

void processcommand() {
  if(memcmp(incommand,mycommand,12)==0) {
    //this command is for me
    long reading = ADS124X.read();
    Serial.write(myreply, 4);
    Serial.print(reading, HEX);
  }
}

void setupADC() {
  ADS124X.reset();
  ADS124X.stop_auto();
  ADS124X.writeRegister(REG_MUX0, SP_AIN1 | SN_AIN0);
  ADS124X.writeRegister(REG_MUX1, VREFCON_AUTO);
  ADS124X.writeRegister(REG_SYS0, PGA_16);
  ADS124X.writeRegister(REG_IDAC1, I1DIR_AIN1 | I2DIR_NONE);
  ADS124X.writeRegister(REG_IDAC0, IMAG_50);
  ADS124X.sleep();
}

