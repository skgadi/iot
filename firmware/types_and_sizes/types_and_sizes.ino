#include <Arduino.h>

void show(const char * tag, int l) {
    Serial.print(tag); Serial.print("\t"); Serial.println(l);
}

void setup() {
    
    Serial.begin(115200); delay(200); Serial.println();

    show("              bool",sizeof(bool));
    show("           boolean",sizeof(boolean));
    show("              byte",sizeof(byte));
    show("              char",sizeof(char));
    show("     unsigned char",sizeof(unsigned char));
    show("           uint8_t",sizeof(uint8_t));
    
    show("             short",sizeof(short));
    show("          uint16_t",sizeof(uint16_t));
    show("              word",sizeof(word));

    show("               int",sizeof(int));
    show("      unsigned int",sizeof(unsigned int));
    show("            size_t",sizeof(size_t));
    
    show("             float",sizeof(float));
    show("              long",sizeof(long));
    show("     unsigned long",sizeof(unsigned long));
    show("          uint32_t",sizeof(uint32_t));

    show("            double",sizeof(double));
    show("       long double",sizeof(long double));
    
    show("         long long",sizeof(long long));
    show("unsigned long long",sizeof(unsigned long long));
    show("          uint64_t",sizeof(uint64_t));

}

void loop() {}
