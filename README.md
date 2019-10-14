# iot

A simple Internet of Things (IOT) example which implements the following functionality:
1. Temperature read
1. Humidity read
1. Potentiometer read
1. Light dependent resistor read
1. Touch sensor read
1. LED color and brighgness write.


## Device

DOIT ESP32 DevKit v1 is used as the IOT node device. The code firmare is available at https://github.com/skgadi/iot/tree/master/firmware/EPS32

### IDE
We use Arduino IDE which is available at https://www.arduino.cc/en/Main/software

### Libraries

Arduino IDE should be configured to include the EPS32 board by including the following link into Preferences > Additional Boards Manager URLs

https://dl.espressif.com/dl/package_esp32_index.json


We also need to include the following arduino libraries which can be added by Tools > Manage Libraries.

1. Firebase ESP32 Client by Mobizt
1. DHT sensor library for ESPx


### Pin layout
PIN layout and documentation is available at https://docs.zerynth.com/latest/official/board.zerynth.doit_esp32/docs/index.html



### Pin assignation
```C
const uint8_t potPin = 35;
const uint8_t ldrPin = 34;
const uint8_t ledR = 23;
const uint8_t ledG = 22;
const uint8_t ledB = 21;
const uint8_t dhtPin = 17;
```

