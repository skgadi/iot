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


//Function declaration
void readDHT();
void readAIs();
void readServer();
void writeServer();

//Please put your device ID
String Device_ID = "FIME20190003";



//Timing variables
uint32_t count=0;
uint32_t DHTReadRate = 20;
uint32_t AIReadRate = 5;
uint32_t ReadServerRate = 1;
uint32_t WriteServerRate = AIReadRate;



//Pin assignation
const uint8_t potPin = 35;
const uint8_t ldrPin = 34;
const uint8_t ledR = 23;
const uint8_t ledG = 22;
const uint8_t ledB = 21; 
const uint8_t dhtPin = 17;

//Variable initialization
uint8_t LEDs[3] = {0, 0, 0};
String LEDLabels[3] = {"Red", "Green", "Blue"};
int POT = 0;
int LDR = 0;
double Temp = 0.0;
double Humidity = 0.0;
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
  if(count%DHTReadRate == 0) readDHT();
  if(count%AIReadRate == 0) readAIs();
  if(count%ReadServerRate == 0) readServer();
  if(count%WriteServerRate == 0) writeServer();
  count++;
  if (count>1000) count -= 1000;
  delay(100);
}

void createLEDsJsonOnServer() {
  sendData.clear();
  for (int i=0; i<3; i++) {
    sendData.addInt(String(i),0);
  }
  if (Firebase.setJSON(firebaseData, path+"/led", sendData))
    Serial.println("LED reset on server successful");
  else
    Serial.println("Server error: Unable to reset LED on server.");
}

void readServer() {
  for (int i=0; i<3; i++) {
    String PathString = path+"/led/"+String(i);
    if (Firebase.getInt(firebaseData, PathString)) {
      LEDs[i] = firebaseData.intData();
      Serial.println(String(LEDLabels[i]) + ": "+ String(LEDs[i]));
    } else {
      Serial.println("Error reading at: " + PathString + ". Resetting server LEDs.");
      createLEDsJsonOnServer();
      return;
    }
  }
  setActuators();
}

void writeServer() {
  ite++;
  sendData.clear()
  .addInt("i",ite)
  .addInt("pot", POT)
  .addInt("ldr", LDR)
  .addInt("touch", touchRead(T0))
  .addDouble("temp", Temp)
  .addDouble("humidity", Humidity);
  if (Firebase.setJSON(firebaseData, path+"/sensors", sendData))
    Serial.println("Write to sever successful");
  else
    Serial.println("Server error: Unable to write sensor data on server.");
  
}

void setActuators() {
  for (int i=0; i<3; i++) {
    ledcWrite(i+1, LEDs[i]);
  }
}

void readDHT() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return;
  }
  Temp = newValues.temperature;
  Humidity = newValues.humidity;
  Serial.println("Temp: "+ String(Temp) +"\nHumidity: "+ String(Humidity));
}

void readAIs() {
  POT = analogRead(potPin);
  LDR = analogRead(ldrPin);
  Serial.println("Pot: "+ String(POT) +"\nLDR: "+String(LDR));  
}
