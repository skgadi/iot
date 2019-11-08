#include <ESP32Encoder.h>

ESP32Encoder encoder;


void setup(){
  
  Serial.begin(115200);
  // Enable the weak pull down resistors
  ESP32Encoder::useInternalWeakPullResistors=true;
  
  // set starting count value
  encoder.clearCount();


  Serial.println("Encoder Start = "+String((int32_t)encoder.getCount()));

  // Attache pins for use as encoder pins
  //encoder.attachHalfQuad(22, 23);
  encoder.attachHalfQuad(22, 23);

}

void loop(){
  // Loop and read the count
  Serial.println(encoder.getCount());
  delay(10);
}
