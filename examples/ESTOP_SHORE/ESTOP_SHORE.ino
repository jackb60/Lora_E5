#include <SoftwareSerial.h>
#include <Lora_E5.h>
#define BATT    A2
#define Y       A3
#define X       A4
#define ESTOP   6
#define DRIVE   7

SoftwareSerial loraSer (2,3);

Lora_E5 lora(&loraSer, 4);

int x0;
int y0;

unsigned long prev = millis();

byte toSend[256];

void writeWithRetry(byte* data, byte len) {
    if(!lora.write(data, len)) {
      Serial.println("SEND FAILED, RESETTING");
      if(!lora.reset()) {
        Serial.println("RESET FAILED");
        while(1) {delay(1);}
      }
      Serial.println("RESET SUCCESS");
    } else {
      Serial.println("SEND SUCCESS");
      prev = millis();
    }
}

void setup() {
  pinMode(5, OUTPUT); //batt led
  digitalWrite(5, 1);
  x0 = analogRead(X); //joystick calibration
  y0 = analogRead(Y);
  loraSer.begin(9600);
  Serial.begin(115200);

  if(!lora.begin("920.000", 10)) {
    Serial.println("BEGIN FAILED");
    while(1) {delay(1);}
  }
  Serial.println("BEGIN SUCCESS");
}
void loop() {
  if(millis() - prev > 250 && digitalRead(ESTOP) && !digitalRead(DRIVE)) { //Heartbeat
    toSend[0] = 0xAA;
    writeWithRetry(toSend, 1);
  }
  if(!digitalRead(ESTOP)) { //ESTOP
    if(millis() - prev > 250){
      toSend[0] = 0xFF;
      writeWithRetry(toSend, 1);
    }
  }
  if(digitalRead(DRIVE) && digitalRead(ESTOP)) { //Manual Drive
    if(millis() - prev > 250){
      if(abs(analogRead(X) - x0) > 15 || abs(analogRead(Y) - y0) > 15) {
        byte x = map(analogRead(X) - x0,-x0,1023-x0,0,254);
        byte y = map(analogRead(Y) - y0,-y0,1023-y0,0,254);
        toSend[0] = x;
        toSend[1] = y;
        writeWithRetry(toSend, 2);
      }
      else {
        toSend[0] = 0xCC;
        writeWithRetry(toSend, 1);
      }
    }
  }
  if(analogRead(BATT) < 675) {
    digitalWrite(5, 0);
  }
}
