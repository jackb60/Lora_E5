#include "Lora_E5.h"
#include <SoftwareSerial.h>

#define RST 4

SoftwareSerial loraSer (2,3);

Lora_E5 lora(&loraSer, 4);

byte toSend[256];

void setup() {
  // put your setup code here, to run once:
  loraSer.begin(9600); //This line is required before lora.begin, and must be 9600
  Serial.begin(115200);
  
  Serial.println("LORA RX TEST");
  if(!lora.begin("920.000", 10)) { //920 MHZ, SF10
    Serial.println("BEGIN FAILED");
    while(1) {delay(1);}
  }
  Serial.println("BEGIN SUCCESS");

  if(!lora.reset()) {
    Serial.println("RESET FAILED");
    while(1) {delay(1);}
  }
  Serial.println("RESET SUCCESS");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("SENDING BYTE 0xCC");
  toSend[0] = 0xCC;
  if(!lora.write(toSend, 1)) {
    Serial.println("SEND BYTE FAILED");
    while(1) {delay(1);}
  }
  Serial.println("SEND BYTE SUCCESS");
  delay(1000);

  Serial.println("SENDING INT -3");
  int testInt = -3;
  if(!lora.write((byte*) &testInt, sizeof(testInt))) {
    Serial.println("SEND INT FAILED");
    while(1) {delay(1);}    
  }
  Serial.println("SEND INT SUCCESS");
  delay(1000);

  Serial.println("SENDING FLOAT 1.125");
  float testFloat = 1.125;
  if(!lora.write((byte*) &testInt, sizeof(testFloat))) {
    Serial.println("SEND FLOAT FAILED");
    while(1) {delay(1);}  
  }
  Serial.println("SEND FLOAT SUCCESS");
  delay(1000);

  Serial.println("SENDING ULONG 999999");
  unsigned long testUlong = 999999;
  if(!lora.write((byte*) &testUlong, sizeof(testUlong))) {
  Serial.println("SEND ULONG FAILED");
    while(1) {delay(1);}  
  }
  Serial.println("SEND ULONG SUCCESS");
  delay(1000);
}
