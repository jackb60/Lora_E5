#include "Arduino.h"
#include "SoftwareSerial.h"

class Lora_E5 {
    public:
        Lora_E5(SoftwareSerial* ser, int rst = -1);
        bool begin(unsigned long freq, int sf, bool mode);
        bool write(byte* data, byte len);
        bool avail();
        byte read_byte();
        int read_int();
        float read_float();
        unsigned long read_ulong();
        int read_block(byte* dest);
    private:
        unsigned long _freq;
        int _sf;
        SoftwareSerial* _ser;
        int _rst;

}