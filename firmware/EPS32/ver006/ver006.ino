#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHTesp.h"
/*#define WIFI_SSID "Souji"
#define WIFI_PASSWORD "sur12345"/**/
/*#define WIFI_SSID "ELSUPER"
#define WIFI_PASSWORD "8711843550"/**/
#define WIFI_SSID "IOTFIME2019"
#define WIFI_PASSWORD "EAFTUxgWt8aYw44M"/**/
#define FIREBASE_HOST "skgadi-basic-iot.firebaseio.com"
#define FIREBASE_AUTH "Ltergx9zqW7ZUDDpy7uBFyuqgthg2zq3585ZNlLf"
FirebaseData firebaseData;
FirebaseJson recData;
FirebaseJson sendData;
String path;


void readNSetLEDValues();
void createLEDsJsonOnServer();
void writeSensorsData();

//Please put your device ID
String Device_ID = "FIME20190002";



//Temp variables
int count=0;


//Pin assignation
const uint8_t potPin = 35;
const uint8_t ldrPin = 34;
const uint8_t ledR = 23;
const uint8_t ledG = 22;
const uint8_t ledB = 21; 
const uint8_t dhtPin = 17;

//Variable initialization
uint8_t LEDs[3] = {0, 0, 0};
int POT = 0;
int LDR = 0;
float Temp = 0.0;
float Humidity = 0.0;
DHTesp dht;
uint32_t ite = 0;




void setup()
{

    path = "/Devices/"+Device_ID;

    Serial.begin(115200);
    delay(1000);
    Serial.println();
    Serial.println();

      //Setting up PWM for LEDs
  ledcAttachPin(ledR, 1);
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);

  //Setting up temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);


    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);    

    Serial.println("Setup complete");


}
void loop()
{

  readNSetLEDValues();
  writeSensorsData();
  delay(5000);
}

void createLEDsJsonOnServer() {
  sendData.clear();
  for (int i=0; i<3; i++) {
    sendData.addInt(String(i),0);
  }
  Firebase.setJSON(firebaseData, path+"/led", sendData);
}

void readNSetLEDValues() {
  for (int i=0; i<3; i++) {
    String PathString = path+"/led/"+String(i);
    if (Firebase.getInt(firebaseData, PathString)) {
      LEDs[i] = firebaseData.intData();
      Serial.println(LEDs[i]);
    } else {
      Serial.println("Error reading at: " + PathString + ". Resetting server LEDs.");
      createLEDsJsonOnServer();
      return;
    }
  }
  setActuators();
}

void writeSensorsData() {
  getValues();
  ite++;
  sendData.clear()
  .addInt("i",ite)
  .addInt("pot", POT)
  .addInt("ldr", LDR)
  .addInt("touch", touchRead(T0))
  .addDouble("temp", Temp)
  .addDouble("humidity", Humidity);
  Firebase.setJSON(firebaseData, path+"/sensors", sendData);  
}

void setActuators() {
  for (int i=0; i<3; i++) {
    ledcWrite(i+1, LEDs[i]);
  }
}

void getValues(){
  POT = analogRead(potPin);
  LDR = analogRead(ldrPin);
  Serial.println(POT);
  Serial.println(LDR);
  getTempAndHum();
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
