#include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

/*
 * Copyright 2013 Barnaby Shearer <b@Zi.iS>, Gary Fletcher <garygfletcher@hotmail.com>
 * License GPLv2
 */

#include <SPI.h>
#include <LiquidCrystal_I2C.h>

//PINS
#define ADC_CS       9
//Other ADC pins are the 10,11,12,13 SPI pins
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

//Register addresses
#define REG_MUX0     0x00
#define REG_MUX1     0x02
#define REG_SYS0     0x03
#define REG_IDAC0    0x0A
#define REG_IDAC1    0x0B

//Register Values
#define VREFCON_AUTO 0b01000000 //Turn int ref on and off out of sleep mode
#define PGA_16       0b01000000 
#define IMAG_50      0b00000001 //50 micro amps
#define I1DIR_AIN1   0b00010000
#define I2DIR_NONE   0b00001111
#define SP_AIN1      0b00001000
#define SN_AIN0      0b00000000

//Commands
#define CMD_WAKE     0b00000000
#define CMD_SLEEP    0b00000010
#define CMD_RESET    0b00000110
#define CMD_NOP      0b11111111
#define CMD_READ     0b00010010
#define CMD_SDATAC   0b00010110
#define CMD_RREG     0b00100000
#define CMD_WREG     0b01000000

#define CAL          0.4
#define VREF         1.0
  #define PGA          16.0
  #define I            0.000050
  
  int lcd_scroll = 0;
  boolean lcd_scroll_right = false;
  
  void setup() {
    pinMode(8, OUTPUT);
    digitalWrite(8, HIGH);
       
    pinMode(ADC_CS, OUTPUT);
    digitalWrite(ADC_CS, HIGH);
    
    lcd.begin(16, 2);
    
    Serial.begin(115200);
    
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1);
    SPI.setClockDivider(SPI_CLOCK_DIV16); // 1Mhz
    
    delay(16); //ADC power-up
  
    digitalWrite(ADC_CS, LOW);
    SPI.transfer(CMD_RESET);
    delay(1);
  SPI.transfer(CMD_SDATAC);
  SPI.transfer(CMD_NOP);
  digitalWrite(ADC_CS, HIGH); 
  
  dumpRegisters();
  
  //Set to defaults can't seem to rely on them being right
  writeRegister(0, 0b00000001);
  writeRegister(1, 0b00000000);
  writeRegister(2, 0b00000000);
  writeRegister(3, 0b00000000);
  writeRegister(10, 0b00000000);
  writeRegister(11, 0b11111111);
  dumpRegisters();

  //Confiure DAC
  writeRegister(REG_MUX0, SP_AIN1 | SN_AIN0);
  writeRegister(REG_MUX1, VREFCON_AUTO);
  writeRegister(REG_SYS0, PGA_16);
  writeRegister(REG_IDAC1, I1DIR_AIN1 | I2DIR_NONE);
  writeRegister(REG_IDAC0, IMAG_50);
  dumpRegisters();
  
  lcd.noCursor();
  lcd.print("_-= Horniman Aquarium Coral Spawning =-_ ");
  
  delay(210*2); //ensure we have settled
}

void loop() {
  digitalWrite(ADC_CS, LOW);
  SPI.transfer(CMD_READ);
  unsigned long reading = ((unsigned long)SPI.transfer(CMD_NOP) << 16) | ((unsigned long)SPI.transfer(CMD_NOP) << 8) | SPI.transfer(CMD_NOP); //Not intrested in negative anyway
  digitalWrite(ADC_CS, HIGH);
  float voltage = (reading*VREF/PGA/(float)0x7fffff);
  float resistance = voltage / I;
  float temp = GetPt1000Temperature(resistance) + CAL;
  Serial.print(reading);
  Serial.print(":");
  Serial.print(voltage, 6);
  Serial.print("v = ");
  Serial.print(resistance,6);
  Serial.print("Ohm = ");
  Serial.print(temp);
  Serial.println("C");
  
  lcd_scroll++;
  if(lcd_scroll>24) {
    lcd_scroll = 0;
    lcd_scroll_right = !lcd_scroll_right;
  }
  if(lcd_scroll_right) {
    lcd.scrollDisplayRight();
  } else {
    lcd.scrollDisplayLeft();
  }
  lcd.setCursor(lcd_scroll_right ? 26 - lcd_scroll : lcd_scroll + 3, 1);
  lcd.print(" ");
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C ");
    
  delay(500);
}

void writeRegister(byte reg, byte value) {
  digitalWrite(ADC_CS, LOW);
  Serial.print("WREG ");
  SPI.transfer(CMD_WREG | (reg & 0b00001111));
  Serial.print(CMD_WREG | (reg & 0b00001111), BIN);
  Serial.print(" ");
  SPI.transfer(0); //Write to 1 register
  Serial.print(value, BIN);
  SPI.transfer(value);
  Serial.println("");
  digitalWrite(ADC_CS, HIGH);
}

void dumpRegisters() {
  digitalWrite(ADC_CS, LOW);
  SPI.transfer(CMD_RREG | 0x0); //From first
  delay(10);
  SPI.transfer(14); //All 14+1
  delay(10);
  Serial.println("Registers ====");
  for(int x=0; x<15; x++) {
    Serial.println(SPI.transfer(CMD_NOP), BIN);
    delay(10);
  }
  Serial.println("==============");
  digitalWrite(ADC_CS, HIGH);
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
