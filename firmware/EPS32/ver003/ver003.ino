#include "DHTesp.h"
//Pin assignation
const uint8_t potPin = 15;
const uint8_t ldrPin = 2;
const uint8_t ledR = 23;
const uint8_t ledG = 22;
const uint8_t ledB = 21; 
const uint8_t dhtPin = 17;

//Variable initialization
uint8_t inByte[2];
uint8_t LEDs[3] = {0, 0, 0};
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
  getValues();
  if (Serial.available() > 0) {
    Serial.readBytes(inByte, 2);
    inByte[0] = inByte[0] - 0x30;
    if (inByte[0] < 3) {
      LEDs[inByte[0]] = inByte[1];
    }
  }
  setActuators();
  printStatus();
  delay(5000);
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
