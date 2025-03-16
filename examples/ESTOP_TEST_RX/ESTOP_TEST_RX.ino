#include "Lora_E5.h"
#include <SoftwareSerial.h>

#define RST 4

SoftwareSerial loraSer (2,3);

Lora_E5 lora(&loraSer, 4);

void setup() {
  // put your setup code here, to run once:
  loraSer.begin(9600); //This line is required before lora.begin, and must be 9600
  Serial.begin(115200);
  
  Serial.println("LORA RX TEST");
  if(!lora.begin(920000000, 10)) { //920 MHZ, SF10
    Serial.println("BEGIN FAILED");
    while(1) {delay(1);}
  }
  Serial.println("BEGIN SUCCESS");

  if(!lora.reset()) {
    Serial.println("RESET FAILED");
  }
  Serial.println("RESET SUCCESS");


  if(!lora.listen()) {
    Serial.println("LISTEN FAILED");
    while(1) {delay(1);}
  }
  Serial.println("LISTEN SUCCESS");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(lora.avail()) {
    Serial.println("DATA AVIAILABLE");
    byte numBytes = lora.read();
    Serial.print(numBytes);
    Serial.println(" BYTES RECIEVED");
    Serial.print("DATA: 0x ");
    for(int i = 0; i < numBytes; i++) {
        Serial.print(lora.buf[i] < 0x10 ? "0" : "");
        Serial.print(lora.buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
  }

}
