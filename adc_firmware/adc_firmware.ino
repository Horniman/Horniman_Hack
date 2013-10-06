/*
 * Copyright 2013 Barnaby Shearer <b@Zi.iS>, Paul Lettington
 * License GPLv2
 */

#include <SPI.h>
#include <ADS124x.h>
#include <avr/power.h>
#include <SoftwareSerial.h>
#include "LLAPSerial.h"

//PINS
#define ADC_CS       9
//Other ADC pins are the 10,11,12,13 SPI pins
#define RADIO        8

#define BOOTLOADER_DELAY       10000 //10s

#define DEBUG
#define SEND

#define DEVICEID "H1"

long next_sample = 0;
long average = 0;
uint8_t average_count = 0;
uint8_t count = 0;

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
  power_twi_disable();

  ADS124X.begin(ADC_CS, SPI_CLOCK_DIV16); // 1Mhz

  //Configure ADC
  setupADC();

  delay(1000);
  LLAP.init();
  LLAP.sendMessage(F("STARTED"));
}

void loop() {
  if( (signed long)( millis() - next_sample ) >= 0) {
    takesample();
  } else {
    if (LLAP.bMsgReceived) {
       LLAP.bMsgReceived = false;
       LLAP.sendIntWithDP("HUM", reading * 10000, 4);
    } else {
      delay(1);
    }
  }
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
  ADS124X.writeRegister(REG_IDAC0, IMAG_50);
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
