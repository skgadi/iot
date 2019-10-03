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




//Please put your device ID
String Device_ID = "FIME20190002";




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
  if (Firebase.getJSON(firebaseData, path+"/led/r")) {
    Serial.println(firebaseData.intData());
  } else {
    sendData.clear().addInt("r",0);
    Firebase.setJSON(firebaseData, path+"/led", sendData);
    Serial.println("Setting R to zero.");
  }
  delay(5000);
}
