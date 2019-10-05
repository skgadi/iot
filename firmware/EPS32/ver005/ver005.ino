#include "DHTesp.h"

//Temp variables
int i=0;
uint8_t R, G, B;           // the Red Green and Blue color components



//Pin assignation
const uint8_t potPin = 15;
const uint8_t ldrPin = 2;
const uint8_t ledR = 23;
const uint8_t ledG = 22;
const uint8_t ledB = 21; 
const uint8_t dhtPin = 17;

//Variable initialization
uint8_t inByte[4];
uint8_t LEDs[3][1] = {&R, &G, &B};
int POT = 0;
int LDR = 0;
float Temp = 0.0;
float Humidity = 0.0;
DHTesp dht;


void setup() {
  //Setting up serial port
  Serial.begin(115200);
  delay(1000);


  //Setting up PWM for LEDs
  ledcAttachPin(ledR, 1);
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);

  //Setting up temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);

  
  Serial.println("Setup complete");
}

void loop() {
  i++;
  lookForLEDInstructions();
  if (i%500 == 0 ) {
    i = 0;
    getValues();
    setActuators();
    printStatus();
  }
  delay(10);
}

void printStatus() {
  Serial.print("LED Status => ");
  Serial.print("Red: ");
  Serial.print(LEDs[0]);
  Serial.print(", Green: ");
  Serial.print(LEDs[1]);
  Serial.print(", Blue: ");
  Serial.println(LEDs[2]);
  Serial.print("Temperature: ");
  Serial.println(Temp);
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  Serial.print("Touch status: ");
  Serial.println(touchRead(T0));
  Serial.print("Potentiometer: ");
  Serial.println(POT);
  Serial.print("Light detecting resistance: ");
  Serial.println(LDR);
  Serial.println("----------  -  ----------  -  ----------  -  ----------");
}

void getValues(){
  POT = analogRead(potPin);
  LDR = analogRead(ldrPin);
  getTempAndHum();
}

void setActuators() {
  for (int i=0; i<3; i++) {
    ledcWrite(i+1, LEDs[i]);
  }
}

void getTempAndHum() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return;
  }
  Temp = newValues.temperature;
  Humidity = newValues.humidity;  
}
void lookForLEDInstructions(){
  if (Serial.available() > 0) {
    uint8_t ActualLength = Serial.readBytes(inByte, 4);
    if (ActualLength<4) {
      Serial.println ("Invalid LED data");
      return;
    }
    for (int i=0; i<4; i++) {
      inByte[i] = inByte[i] - 0x30;
      if (inByte[0] > 2) {
        Serial.println("Invalid color selection");
        return;
      }
      if (inByte[0]>9) {
        Serial.println("Invalid brigtness");
        return;
      }
    }
    int brightness = 100*inByte[1] + 10*inByte[2] + inByte[3];
    if (brightness > 255) {
      Serial.println("Invalid brigtness");
      return;
    }
    LEDs[inByte[0]] = brightness;
    setActuators();
  }  
}


void hueToRGB(uint8_t hue, uint8_t brightness)
{
    uint16_t scaledHue = (hue * 6);
    uint8_t segment = scaledHue / 256; // segment 0 to 5 around the
                                            // color wheel
    uint16_t segmentOffset =
      scaledHue - (segment * 256); // position within the segment

    uint8_t complement = 0;
    uint16_t prev = (brightness * ( 255 -  segmentOffset)) / 256;
    uint16_t next = (brightness *  segmentOffset) / 256;

    if(invert)
    {
      brightness = 255 - brightness;
      complement = 255;
      prev = 255 - prev;
      next = 255 - next;
    }

    switch(segment ) {
    case 0:      // red
        R = brightness;
        G = next;
        B = complement;
    break;
    case 1:     // yellow
        R = prev;
        G = brightness;
        B = complement;
    break;
    case 2:     // green
        R = complement;
        G = brightness;
        B = next;
    break;
    case 3:    // cyan
        R = complement;
        G = prev;
        B = brightness;
    break;
    case 4:    // blue
        R = next;
        G = complement;
        B = brightness;
    break;
   case 5:      // magenta
    default:
        R = brightness;
        G = complement;
        B = prev;
    break;
    }
}
