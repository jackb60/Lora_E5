#include "Arduino.h"

class Lora_E5 {
    public:
        Lora_E5(Serial* ser, int rst = -1);
        bool begin(unsigned long freq, int sf);
        bool reset();
        bool write(byte* data, byte len);
        bool listen();
        byte avail();
        byte read(byte* dest);
        int snr();
        int rssi();
    private:
        Serial* _ser;
        unsigned long _freq;
        int _sf;
        int _rst;
        int _rssi;
        int _snr;
        byte _buf[256];
        byte _bufLen;
}