#include "Arduino.h"

class Lora_E5 {
    public:
        Lora_E5(Stream* ser, int rst = -1);
        bool begin(unsigned long freq, int sf);
        bool reset();
        bool write(byte* data, byte len);
        bool listen();
        byte avail();
        byte read();
        int snr();
        int rssi();
        byte buf[256];
        byte bufLen;
    private:
        Stream* _ser;
        unsigned long _freq;
        int _sf;
        int _rst;
        int _rssi;
        int _snr;
};