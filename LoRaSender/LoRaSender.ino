#include "Arduino.h"
#include <analogWrite.h>
#include <SPI.h>
#include <LoRa.h>




// LoRa Pins
#define ss 5
#define rst 14
#define dio0 2


// LoRa Setups Variables
long frequency = 866E6;//  [433E6 for Asia], [866E6 for Europe], [915E6 for North America]
long bandwidth = 125E3;
int  spreading_fuctor = 7;
int tx_power = 10;
int sync_word = 0;
int coding_rate = 5;
long preamble_length = 0;



// Setup LoRa transceiver module
void setup_lora( long &frequency, long &bandwidth, 
                  int &spreading_fuctor, int &tx_power, 
                  int &sync_word, int &coding_rate, long &preamble_length ){
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(frequency)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  LoRa.setSignalBandwidth(bandwidth);
  LoRa.setSpreadingFactor(spreading_fuctor); // 6 - 12
  LoRa.setTxPower(tx_power);  // Max 20
  LoRa.setCodingRate4(coding_rate); // 5,6,7,8
  LoRa.setPreambleLength(preamble_length);
  Serial.println("LoRa Initializing OK!");
}








// MIC / SPEACKER  Pins
#define speacker 32
#define mic_pin  33

const int len = 83900;
byte buff[len];
int i = 0;

void read_mic(){
  // Loop per hulf of second
  long time_loop = 1000;
  long startTime = millis();
  int counter = 0;
  
  while ( (millis()-startTime) <= time_loop ) {
    // Fill buffer and then send it without LoRa
    byte val = analogRead(mic_pin);
    //Serial.println(val);
    //analogWrite(speacker, val);
    // Create buffer
    buff[i] = val;
    i++;
    if (i == len-1) {
      Serial.println("LoRa sends OK   i= " + String(i));
      
      LoRa.beginPacket();
      LoRa.write((uint8_t*)&buff, sizeof(buff));
      LoRa.endPacket(true);
      
      i = 0;
    }
    counter++;
    //delayMicroseconds(5.3);
  }

    Serial.println( String(counter) + " Hz");
}




void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  // Setup Pins
  pinMode(speacker, OUTPUT);
  pinMode(mic_pin, INPUT);
  // Setup LoRa transceiver module
  setup_lora(frequency,bandwidth,spreading_fuctor,
              tx_power,sync_word,coding_rate,preamble_length );
}





void loop() {

  read_mic();
  
}
