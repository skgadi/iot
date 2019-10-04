#include <WiFi.h>
#include <FirebaseESP32.h>
#define WIFI_SSID "ELSUPER"
#define WIFI_PASSWORD "8711843550"
/*#define WIFI_SSID "IOTFIME2019"
#define WIFI_PASSWORD "EAFTUxgWt8aYw44M"*/
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



uint8_t LEDs[3] = {0, 0, 0};
uint8_t POT = 23;
uint8_t LDR = 23;
uint8_t Touch = 23;
float Temp = 23;
float Humidity = 23;
uint32_t ite = 0;




void setup()
{

    path = "/Devices/"+Device_ID;

    Serial.begin(115200);
    Serial.println();
    Serial.println();

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
}

void writeSensorsData() {
  ite++;
  sendData.clear()
  .addInt("i",ite)
  .addInt("pot", POT+ite)
  .addInt("ldr", LDR+ite*2)
  .addInt("touch", Touch + 3*ite)
  .addDouble("temp", Temp*0.5)
  .addDouble("humidity", Humidity*1.5);
  Firebase.setJSON(firebaseData, path+"/sensors", sendData);  
}
