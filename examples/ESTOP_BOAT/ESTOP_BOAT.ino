#include <Wire.h>
#include <Lora_E5.h>
#include <SoftwareSerial.h>

SoftwareSerial loraSer (2,3);
Lora_E5 lora(&loraSer, 4);

byte code;

bool estop = false;
bool drive = false;
bool connected = true;

byte pktLen = 0;

unsigned long last;

bool rst = false;

typedef union i2cfloat{
  float f;
  byte b[4];
};

typedef union i2cint{
  int i;
  byte b[2];
};

i2cfloat x_pos;
i2cfloat y_pos;

i2cint rssi;
i2cint snr;

void setup() {
  Wire.begin(0x12);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  pinMode(5, OUTPUT);

  digitalWrite(5, 0);
  Serial.begin(115200);
  loraSer.begin(9600);
  
  if(!lora.begin("920.000", 10)) {
    Serial.println("BEGIN FAILED");
    while(1) {delay(1);}
  }
  Serial.println("BEGIN SUCCESS");

  if(!lora.listen()) {
    Serial.println("LISTEN FAILED");
    while(1) {delay(1);}
  }
  Serial.println("LISTEN SUCCESS");

  last = millis();
}

void loop() {
  if(rst) {
    if(!lora.reset()) {
      Serial.println("RESET FAILED");
      while(1) {delay(1);}
    }
    Serial.println("RESET SUCCESS");
  
    if(!lora.listen()) {
      Serial.println("LISTEN FAILED");
      while(1) {delay(1);}
    }
    Serial.println("LISTEN SUCCESS");

    rst = false;
  }

  byte avail = loraSer.available();
  if(lora.avail()) {
    pktLen = lora.read();
    if(pktLen != 0) {
      parseRx();
    }
  }
  if(millis() - last > 1000) {
    estop = true;
    connected = false;
    digitalWrite(5, 0);
    drive = false;
  }
}

void parseRx() {
  estop = false;
  connected = true;
  last = millis();
  rssi.i = lora.rssi();
  snr.i = lora.snr();
  Serial.println("DATA RECIEVED");
  Serial.print("LENGTH: ");
  Serial.println(pktLen);
  Serial.print("RSSI: ");
  Serial.println(rssi.i);
  Serial.print("SNR: ");
  Serial.println(snr.i);

  if (pktLen == 1) {
    byte status = lora.buf[0];
    Serial.print("STATUS: ");
    Serial.println(status, HEX);
    if(status == 0xFF){
        estop = true;
        drive = false;
      }
      else if(status == 0xCC){
        estop = false;
        drive = true;
        x_pos.f = 0.0;
        y_pos.f = 0.0;
      }
      else{
        estop = false;
        drive = false;
      }
  }else if (pktLen == 2) {
    drive = true;
    float x = (float) (lora.buf[0]);
    float y = (float) (lora.buf[1]);
    Serial.print("X: ");
    Serial.print(lora.buf[0]);
    Serial.print(" Y: ");
    Serial.println(lora.buf[1]);
    x_pos.f = -1.0 * (x - 127.0) / 127.0;
    y_pos.f = (y - 127.0) / 127.0;
    Serial.print("NORM X: ");
    Serial.print(x_pos.f);
    Serial.print(" NORM Y: ");
    Serial.println(y_pos.f);
  }
  if(estop) {
    digitalWrite(5, 0);
  }
  else {
    digitalWrite(5, 1);
  }
  Serial.println();
}

void receiveEvent() {
  code = Wire.read();

  switch (code) {
    case 0x08:
      rst = true;
      break;
  }
}

void requestEvent() {
  switch (code) {
    case 0x01:
      Wire.write(estop);
      break;
    case 0x02:
      Wire.write(drive);
      break;
    case 0x03:
      Wire.write(x_pos.b[0]);
      Wire.write(x_pos.b[1]);
      Wire.write(x_pos.b[2]);
      Wire.write(x_pos.b[3]);
      break;
    case 0x04:
      Wire.write(y_pos.b[0]);
      Wire.write(y_pos.b[1]);
      Wire.write(y_pos.b[2]);
      Wire.write(y_pos.b[3]);
      break;
    case 0x05:
      Wire.write(connected);
      break;
    case 0x06:
      Wire.write(rssi.b[0]);
      Wire.write(rssi.b[1]);
      break;
    case 0x07:
      Wire.write(snr.b[0]);
      Wire.write(snr.b[1]);
      break;
  }
}