// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6) 
const int potPin = 15;
const int ldrPin = 2;

// variable for storing the potentiometer value
int potValue = 0;
int ldrValue = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  potValue = analogRead(potPin);
  Serial.print("POT value: ");
  Serial.println(potValue);
  ldrValue = analogRead(ldrPin);
  Serial.print("LDR value: ");
  Serial.println(ldrValue);
  delay(5000);
}
