#include "Arduino.h"
#include "Lora_E5.h"

Lora_E5::Lora_E5(Serial* ser, int rst = -1) {
    _ser = ser;
    _rst = rst;
}

bool Lora_E5::begin(unsigned long freq, int sf) {
    unsigned long _freq = freq;
    int _sf = sf
    _ser.begin(9600);
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, 0);
    delay(500);
    _ser.print("AT\n");
    delay(200);
    _ser.print("AT+MODE=TEST\n");
    delay(200);
    _ser.print("AT+TEST=RFCFG,");
    _ser.print(_freq);
    _ser.print(",SF");
    _ser.print(_sf);
    _ser.print(",125,8,8,22,ON,OFF,OFF\n");
    if(_ser.find("+TEST: RFCFG", 12)) {
        while(_ser.available()) {
            _ser.read();
            delay(1);
        }
        return true;
    }
    return false;
}

bool Lora_E5::reset() {
    digitalWrite(_rst, 1);
    delay(500);
    digitalWrite(_rst, 0);
    return begin(_freq, _sf);
}

bool Lora_E5::write(byte* data, byte len) {
    _ser.print("AT+TEST=TXLRPKT, \"");
    for(byte i = 0; i < len; i++) {
        _ser.print(*(data + i) < 0x10 ? "0" : "");
        _ser.print(*(data + i), HEX);
    }
    _ser.print("\"\n");
    if(_ser.find("+TEST: TX DONE", 14)) {
        while(_ser.available()) {
            _ser.read();
            delay(1);
        }
        return true;
    }
    return false;
}

bool Lora_E5::listen() {
    loraSer.write("AT+TEST=RXLRPKT\n");
    if(_ser.find("+TEST: RX", 9)) {
        while(_ser.available()) {
            _ser.read();
            delay(1);
        }
        return true;
    }
    return false;
}

byte Lora_E5::avail(byte* dest) {
    return _ser.available();
}

byte Lora_E5::read() {
    byte bytes_avail = avail();
    delay(1);
    while(avail() > bytes_avail) {
        bytes_avail = avail();
        delay(1);
    }

    _ser.readStringUntil(':'); //+TEST:
    _ser.readStringUntil(':'); //LEN:
    String lenStr = _ser.readStringUntil(',');
    int len = lenStr.toInt();
    _ser.readStringUntil(':'); //, RSSI:
    String rssiStr = _ser.readStringUntil(',');
    _rssi = rssiStr.toInt();
    loraSer.readStringUntil(':');
    String snrStr = _ser.readStringUntil('\n');
    _snr = snrStr.toInt();
    loraSer.readStringUntil('"'); // +TEST: RX
    
    
    String dtaStr = loraSer.readStringUntil('"');
    for (int i = 0; i < dtaStr.length(); i++) {
        byte data = dtaStr.charAt(i);
        data = (data < 0x41) ? data - 48 : data - 55;
        _buf[i / 2] = (i % 2 == 0) ? data : data << 4;
    }
    _bufLen = dtaStr.length / 2;

    dest = _buf;
    return _bufLen;
    
}


int Lora_E5::snr() {
    return _snr;
}

int Lora_E5::rssi() {
    return _rssi;
}
