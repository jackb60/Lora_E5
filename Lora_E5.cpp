#include "Arduino.h"
#include "Lora_E5.h"

Lora_E5::Lora_E5(Stream* ser, int rst) {
    _ser = ser;
    _rst = rst;
}

bool Lora_E5::begin(char* freq, int sf) {
    _sf = sf;
    _freq = freq;
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, 0);
    delay(1000);
    _ser->print("AT\n");
    delay(200);
    _ser->print("AT+MODE=TEST\n");
    delay(200);
    _ser->print("AT+TEST=RFCFG,");
    _ser->print(_freq);
    _ser->print(",SF");
    _ser->print(_sf);
    _ser->print(",125,8,8,22,ON,OFF,OFF\n");
    if(_ser->find("+TEST: RFCFG", 12)) {
        delay(1);
        while(_ser->available()) {
            _ser->read();
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
    _ser->print("AT+TEST=TXLRPKT, \"");
    for(int i = 0; i < len; i++) {
        _ser->print(*(data + i) < 0x10 ? "0" : "");
        _ser->print(*(data + i), HEX);
    }
    _ser->print("\"\n");
    if(_ser->find("+TEST: TX DONE", 14)) {
        delay(1);
        while(_ser->available()) {
            _ser->read();
            delay(1);
        }
        return true;
    }
    return false;
}

bool Lora_E5::listen() {
    _ser->write("AT+TEST=RXLRPKT\n");
    if(_ser->find("+TEST: RX", 9)) {
        delay(1);
        while(_ser->available()) {
            _ser->read();
            delay(1);
        }
        return true;
    }
    return false;
}

byte Lora_E5::avail() {
    return _ser->available();
}

byte Lora_E5::read() {
    byte bytes_avail = avail();
    delay(1);
    while(avail() > bytes_avail) {
        bytes_avail = avail();
        delay(1);
    }

    _ser->readStringUntil(':'); //+TEST:
    _ser->readStringUntil(':'); //LEN:
    String lenStr = _ser->readStringUntil(',');
    int len = lenStr.toInt();
    _ser->readStringUntil(':'); //, RSSI:
    String rssiStr = _ser->readStringUntil(',');
    _rssi = rssiStr.toInt();
    _ser->readStringUntil(':');
    String snrStr = _ser->readStringUntil('\n');
    _snr = snrStr.toInt();
    _ser->readStringUntil('"'); // +TEST: RX
    
    
    String dtaStr = _ser->readStringUntil('"');
    memset(buf, 0, 256);
    for (int i = 0; i < dtaStr.length(); i++) {
        byte data = dtaStr.charAt(i);
        data = (data < 0x41) ? data - 48 : data - 55;
        buf[i / 2] += (i % 2 == 1) ? data : data << 4;
    }
    bufLen = dtaStr.length() / 2;

    return bufLen;
    
}


int Lora_E5::snr() {
    return _snr;
}

int Lora_E5::rssi() {
    return _rssi;
}
