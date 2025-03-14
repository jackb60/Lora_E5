#include "Arduino.h"
#include "Lora_E5.h"
#include "SoftwareSerial.h"

Lora_E5::Lora_E5(SoftwareSerial* ser, int rst = -1) {
    _ser = ser;
    _rst = rst;
}

bool Lora_E5::begin() {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, 0);
    delay(500);
    _ser.write("AT\n");
    delay(200);
    _ser.write("AT+MODE=TEST\n");
    delay(200);
    _ser.write("AT+TEST=RFCFG,");
    _ser.print(_freq);

    
}