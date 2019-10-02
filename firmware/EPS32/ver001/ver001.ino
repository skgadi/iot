#include <WiFi.h>
#include <FirebaseESP32.h>
#define WIFI_SSID "IOTFIME2019"
#define WIFI_PASSWORD "EAFTUxgWt8aYw44M"
#define FIREBASE_HOST "skgadi-basic-iot.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "Ltergx9zqW7ZUDDpy7uBFyuqgthg2zq3585ZNlLf"
FirebaseData firebaseData;

void setup()
{
    //Please put your device ID
    String Device_ID = "FIME20190002";


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
    String path = "/Devices/"+Device_ID;

    String jsonData = "";
    

    FirebaseJson recData;
    FirebaseJson sendData;
}
void loop()
{
  if (Firebase.getJSON(firebaseData, path+'/led/r')) {
    if (firebaseData.dataType() == "int")
            Serial.println(firebaseData.intData());
  }
  delay(5000);
}
