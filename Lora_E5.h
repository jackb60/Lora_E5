#include "Arduino.h"

class Lora_E5 {
    public:
        LoRa_E5(int rst = -1);
        bool begin();
    private:
        int _freq;
        int _sf;
        int
}