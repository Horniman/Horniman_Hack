/*
 * Copyright 2013 Barnaby Shearer <b@Zi.iS>, Paul Lettington
 * License GPLv2
 */

#include <SPI.h>
#include <ADS124x.h>
#include <avr/power.h>
#include "LLAPSerial.h"
#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

//PINS
#define ADC_CS       9
//Other ADC pins are the 10,11,12,13 SPI pins
#define RADIO        8
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

#define BOOTLOADER_DELAY       10000 //10s

#define DEBUG
#define SEND

#define DEVICEID "HA"
#define AVERAGE 50
#define SAMPLERATE 200
#define VREF         2.0
#define PGA          16.0
#define I            0.000100
#define OFF          0
#define GAIN         1

long next_sample = 0;
unsigned long average = 0;
float reading = 0;
uint8_t average_count = 0;

void setup() {
  
  Serial.begin(115200);
    
  //Enable radeo for OTA programming.
  pinMode(RADIO, OUTPUT);
  digitalWrite(RADIO, HIGH);
  delay(BOOTLOADER_DELAY);  

  Serial.begin(9600);

  //Power down unused stuff (really sleeping dosn't help as we need the acurate clock running, and the support components make any other savings mute).
  power_adc_disable();
  power_timer2_disable();

  ADS124X.begin(ADC_CS, SPI_CLOCK_DIV16); // 1Mhz
  
  //Configure ADC
  setupADC();
  
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.print("Horniman HA");
  
  delay(1000);
  LLAP.init(DEVICEID);
  LLAP.sendMessage(F("STARTED"));
}

void loop() {
  if( (signed long)( millis() - next_sample ) >= 0) {
    takesample();
  } else {
    if (LLAP.bMsgReceived) {
       LLAP.bMsgReceived = false;
       #ifdef DEBUG
         Serial.print("DEBUG: ");
         Serial.println(LLAP.sMessage);
       #endif
       if(LLAP.sMessage.compareTo("TEMP-----") == 0) {
         LLAP.sendIntWithDP("TMP", reading * 100, 2);
       } else if(LLAP.sMessage.startsWith("TARG")) {
         lcd.setCursor(9, 1);
         lcd.print(LLAP.sMessage.substring(4));
         lcd.print((char)223);
         lcd.print("C ");
       }
    } else {
      delay(1);
    }
  }
}

void takesample() {
  next_sample = millis() + SAMPLERATE;
  if(++average_count == AVERAGE) {
    float voltage = ((average / AVERAGE)*VREF/PGA/(float)0x7fffff);
    float resistance = voltage / I;
    reading = (GetPt1000Temperature(resistance) + OFF) * GAIN;
    average = 0;
    average_count = 0;
    lcd.setCursor(0, 1);
    lcd.print(reading);
    lcd.print((char)223);
    lcd.print("C ");
    #ifdef DEBUG
      Serial.print("DEBUG: ");
      Serial.println(reading);
    #endif
  }
  digitalWrite(ADC_CS, LOW);
  SPI.transfer(CMD_READ);
  average += ((unsigned long)SPI.transfer(CMD_NOP) << 16) | ((unsigned long)SPI.transfer(CMD_NOP) << 8) | SPI.transfer(CMD_NOP); //Not intrested in negative anyway
  digitalWrite(ADC_CS, HIGH);
}

void setupADC() {
  #ifdef DEBUG
    Serial.println("INFO: Setup ADC");
  #endif
  ADS124X.reset();
  ADS124X.stop_auto();
  ADS124X.writeRegister(REG_MUX0, SP_AIN1 | SN_AIN0);
  ADS124X.writeRegister(REG_MUX1, VREFCON_AUTO);
  ADS124X.writeRegister(REG_SYS0, PGA_16);
  ADS124X.writeRegister(REG_IDAC1, I1DIR_AIN1 | I2DIR_NONE);
  ADS124X.writeRegister(REG_IDAC0, IMAG_100);
  ADS124X.sleep();
}

float GetPt1000Temperature(float r) {
  float const Pt1000[] = {
    803.1, 822.9, 842.7, 862.5, 882.2, 901.9, 921.6, 941.2, 960.9, 980.4,
    1000., 1019.5, 1039., 1058.5, 1077.9, 1097.3, 1116.7, 1136.1, 1155.4, 1174.7,
    1194., 1213.2, 1232.4, 1251.6, 1270.7, 1289.8, 1308.9, 1328., 1347., 1366.,
    1385., 1403.9, 1422.9, 1573.1, 1758.4, 1958.4
  };
 
  int t = -50, i = 0, dt = 0; 
 
  if (r > Pt1000[0]) {
    while (250 > t) {
      dt = (t < 110) ? 5 : (t > 110) ? 50 : 40;
      if (r < Pt1000 [++i]) {
        return t + ( r - Pt1000[i-1])*dt/(Pt1000[i]-Pt1000[i-1]);
      }
      t += dt;
    }
  }
  return t;
}
