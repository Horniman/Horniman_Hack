/*
 * Copyright 2013 Barnaby Shearer <b@Zi.iS>
 * License GPLv2
 */

#include <SPI.h>
#include <ADS124x.h>
#include <SoftwareSerial.h>
#include <IridiumSBD.h>
#include <QueueList.h>
#include <avr/power.h>
#include <EEPROM.h>

//PINS
#define ADC_CS       9
//Other ADC pins are the 10,11,12,13 SPI pins
#define RADIO        8
SoftwareSerial ssIridium(2, 3);
IridiumSBD isbd(ssIridium, 4);

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

#define BOOTLOADER_DELAY       10000 //10s
#define MSG_LENGTH             50

#define DEBUG
#define SEND

//Wrap the array so we can pass it around 
class Msg {
  public:
    uint8_t data[MSG_LENGTH];  
    Msg() {};
    Msg(uint8_t d[MSG_LENGTH]) {memcpy(data, d, sizeof (data));};
};

long next_sample = 0;
long average = 0;
uint8_t average_count = 0;
uint8_t count = 0;
uint8_t msg[MSG_LENGTH];
uint8_t msg_p = 0;

QueueList <Msg> send_queue;

void setup() {
  
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
    
  //Enable radeo for OTA programming.
  pinMode(RADIO, OUTPUT);
  digitalWrite(RADIO, HIGH);
  #ifndef DEBUG
    delay(BOOTLOADER_DELAY);  
    digitalWrite(RADIO, LOW);
  #endif

  //Power down unused stuff (really sleeping dosn't help as we need the acurate clock running, and the support components make any other savings mute).
  power_adc_disable();
  power_timer2_disable();
  power_twi_disable();  

  loadSettings();

  ADS124X.begin(ADC_CS, SPI_CLOCK_DIV16); // 1Mhz
  
  //Configure ADC
  setupADC();

  //Configure ISBD
  isbd.setPowerProfile(0); //High Current mode/fast retries
  #ifdef DEBUG
    isbd.attachConsole(Serial);
    isbd.attachDiags(Serial);
  #endif

  isbd.begin();
  syncTime();
  isbd.sleep();
}

void loop() {
  if( (signed long)( millis() - next_sample ) >= 0) {
    takesample();
    sendmsg();
  } else {
    delay(1);
  }
}

bool ISBDCallback() {
  if( (signed long)( millis() - next_sample ) >= 0) {
      takesample();
  }
  return true;
}


void loadSettings() {
  if(EEPROM.read(0) == SETTING_VERSION[0] &&
    EEPROM.read(1) == SETTING_VERSION[1] &&
    EEPROM.read(2) == SETTING_VERSION[2] &&
    EEPROM.read(3) == SETTING_VERSION[3]
  ) {
    #ifdef DEBUG
      Serial.println("INFO: loading settings");
    #endif
    for (unsigned int i=0; i<sizeof(settings); i++) {
      *((char*)&settings + i) = EEPROM.read(4 + i);
    }
    #ifdef DEBUG
      dumpSettings();
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("WARNING: settings not found");
    #endif  
    EEPROM.write(0, SETTING_VERSION[0]);
    EEPROM.write(1, SETTING_VERSION[1]);
    EEPROM.write(2, SETTING_VERSION[2]);
    EEPROM.write(3, SETTING_VERSION[3]);
    saveSettings();
  }
}

void saveSettings() {
  #ifdef DEBUG
    Serial.println("INFO: Saving settings:");
    dumpSettings();
  #endif
  for (unsigned int i=0; i<sizeof(settings); i++) {
    EEPROM.write(4 + i, *((char*)&settings + i));
  }
}

#ifdef DEBUG
void dumpSettings() {
  Serial.print("DEBUG: sample_interval = ");
  Serial.println(settings.sample_interval);
  Serial.print("DEBUG: samples_per_reading = ");
  Serial.println(settings.samples_per_reading);
  Serial.print("DEBUG: discard_min = ");
  Serial.println(settings.discard_min);
  Serial.print("DEBUG: discard_max = ");
  Serial.println(settings.discard_max);
  Serial.print("DEBUG: scale_min = ");
  Serial.println(settings.scale_min);
  Serial.print("DEBUG: scale_range = ");
  Serial.println(settings.scale_range);
}
#endif

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

void syncTime() {
  #ifdef DEBUG
    Serial.println("INFO: Waitinging for network time");
  #endif
  
  time_t time;
  if(isbd.getSystemTime(time) == ISBD_SUCCESS) {
    next_sample = (((settings.sample_interval/1000) - time % (settings.sample_interval/1000))) * 1000;
    #ifdef DEBUG
      Serial.print("DEBUG: Network time:");
      Serial.print(time);
      Serial.print(", next sample:");
      Serial.print(next_sample);
      Serial.print(", at:");
      Serial.println(time + next_sample/1000);
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("ERROR: no network time");
    #endif
  }
}

void takesample() {
  next_sample += settings.sample_interval;
  #ifdef DEBUG
    Serial.print("DEBUG: Sample: ");
  #endif
  long reading = ADS124X.read();
  #ifdef DEBUG
    Serial.println(reading);
  #endif
  if(reading < settings.discard_min || reading > settings.discard_max) {
    #ifdef DEBUG
        Serial.println("WARNING: Discarding out-of-range!");
    #endif
    setupADC();
  } else {
    average_count++;
    average += reading;
  }
  if(++count==settings.samples_per_reading) {
    if(average_count>0) {
      msg[msg_p++] = scalereading(average/average_count);
    } else {
      #ifdef DEBUG
        Serial.print("ERROR: no samples");
      #endif
      msg[msg_p++] = 0;
    }
    #ifdef DEBUG
      Serial.print("INFO: Reading:");
      Serial.println(msg[msg_p-1]);
    #endif
    average = 0;
    average_count = 0;
    count = 0;
    if(msg_p==MSG_LENGTH) {
      #ifdef DEBUG
        Serial.print("INFO: Queued Message");
      #endif
      send_queue.push(Msg(msg));
      msg_p == 0;
    }
  }  
}

uint8_t scalereading(long reading) {
  return (unsigned long)(reading - settings.scale_min) * (unsigned long)256 / settings.scale_range;
}

void sendmsg() {
  if(send_queue.isEmpty()) {
    return;
  }
  uint8_t rxBuffer[270];
  size_t bufferSize = 270;
  isbd.begin();
  if(isbd.isAsleep()) {
    #ifdef DEBUG
      Serial.println("ERROR: ISBD wake failed");
    #endif
  } else {
    while(!send_queue.isEmpty()) {
      #ifdef DEBUG
        Serial.print("DEBUG: MSG:");
        Serial.write(send_queue.peek().data, 50);
        Serial.println();
      #endif
      #ifdef SEND
        int status =  isbd.sendReceiveSBDBinary(send_queue.peek().data, MSG_LENGTH, rxBuffer, bufferSize);
        if (status == ISBD_SUCCESS) {
          send_queue.pop();
        } else {
          Serial.println("ERROR: ISBD send failed");
          break;
        }
      #else
        send_queue.pop();
      #endif
      if(bufferSize>0) {
        memcpy(&settings, rxBuffer, sizeof (settings));
        saveSettings();
      }
    }
    syncTime();
    isbd.sleep();
  }
}

