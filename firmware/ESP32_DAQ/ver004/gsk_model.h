#include <ArduinoJson.h>
#include <vector>
#include <math.h>
using namespace std;

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

struct PORT {
  String Name;
  int Pin;
};

struct VAR {
  String Name;
  double Value;
};

struct MODEL {
  vector <VAR> Params;
  vector <VAR> States;
  vector <PORT> Inputs;
  vector <PORT> Outputs;
  double T_S=0.001;
} System;

PORT newPORT(String Name, int Pin) {
  PORT Out;
  Out.Name = Name;
  Out.Pin = Pin;
  return Out;
}

VAR newVAR(String Name, double Value) {
  VAR Out;
  Out.Name = Name;
  Out.Value = Value;
  return Out;
}

StaticJsonDocument<300> JsonCodeIn;
StaticJsonDocument<300> JsonCodeOut;

void readParamsFromEEPROM() {
  int count = System.Params.size();
  int Address = 0;
  for (char i=0; i<count; i++) {
    System.Params[i].Value = EEPROM.readDouble(Address);
    Address += sizeof(double);
  }
}

void AppendAState(String Name, double Value) {
  System.States.push_back(newVAR(Name, Value));
}

void AppendAParam(String Name, double Value) {
  System.Params.push_back(newVAR(Name, Value));
}

void AppendAnInputPort(String Name, int Pin) {
  System.Inputs.push_back(newPORT(Name, Pin));
}

void AppendAnOutputPort(String Name, int Pin) {
  System.Outputs.push_back(newPORT(Name, Pin));
}

void rt_loop (MODEL &Sys);

void realtimeActivity(void *GSystem) {
  bool LEDState = true;
  // Initialise the xLastWakeTime variable with the current time.
  TickType_t xLastWakeTime = xTaskGetTickCount();
  //TickType_t xFrequency = pdMS_TO_TICKS(((MODEL*) GSystem)->States[0].Value);
  //MODEL Sys = (MODEL *)GSystem;
  for (;;) {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(((MODEL*) GSystem)->T_S*1000));
    
    rt_loop(*(MODEL *) GSystem);
  }
}


void gskInit() {
  Serial.begin(1500000);
  readParamsFromEEPROM();
  xTaskCreatePinnedToCore(realtimeActivity, "realtimeActivity", 32768, (void*)&System, 2,  NULL,  ARDUINO_RUNNING_CORE);
}

char cmdByte;
char tempDbl[10];
//String InItem;
void loop() {
  if (Serial.available()) {
    const String InItem = Serial.readString();
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(JsonCodeIn, InItem);
    if (!error) {
      int cmd = JsonCodeIn["A"];
      JsonCodeOut.clear();
      JsonCodeOut["B"] = cmd;
      switch (cmd) {
        case 0:  { //Count number of states
          JsonCodeOut["v"] = System.States.size();
        }
        break;
        case 1: { //Read state
          int i = JsonCodeIn["i"];
          if (i>=0 && i<System.States.size()) {
            JsonCodeOut["i"] = i;
            JsonCodeOut["v"] = System.States[i].Name;
          }
        }
        break;
        case 2: { //Read sValue
          int i = JsonCodeIn["i"];
          if (i>=0 && i<System.States.size()) {
            JsonCodeOut["i"] = i;
            JsonCodeOut["v"] = System.States[i].Value;
          }
        }
        break;
        case 3: { //Read pCount
          int i = JsonCodeIn["i"];
          if (i>=0 && i<System.States.size()) {
            JsonCodeOut["v"] = System.Params.size();
          }
        }
        break;
        case 4: { //Read param
          int i = JsonCodeIn["i"];
          if (i>=0 && i<System.Params.size()) {
            JsonCodeOut["i"] = i;
            JsonCodeOut["v"] = System.Params[i].Name;
          }
        }
        break;
        case 5: { //Read pValue
          int i = JsonCodeIn["i"];
          if (i>=0 && i<System.Params.size()) {
            JsonCodeOut["i"] = i;
            JsonCodeOut["v"] = System.Params[i].Value;
          }
        }
        break;
      }
      serializeJson(JsonCodeOut, Serial);
      JsonCodeIn.clear();
    } else {
      Serial.println("opss");
    }
  }
  /*
  
  int InCount = ;
  double recValue=0;
  if (InCount) {
    cmdByte = Serial.read();
    switch (cmdByte) {
      case 0x00: { //set sampling time
          char InBytes[100];
          Serial.readBytes(InBytes, InCount);
          double Temp_T_S = atof (InBytes);
          if (Temp_T_S>=0.001) {
            System.T_S = Temp_T_S;
            Serial.print("{t_s:"+ String(Temp_T_S) +"}");
          }
        }
      break;
      case 0x01: { //Count number of states
          Serial.print("{\"sCount\":" + String(System.States.size()) + "}");
      }
      break;
      case 0x02: { //Read state
        uint8_t idx = Serial.read();
        if (idx<System.States.size())
          Serial.print("{\"state\":\""+ System.States[idx].Name +"\",\"idx\":"+idx+"}");
      }
      break;
      case 0x03: { //Read State value
        uint8_t idx = Serial.read();
        if (idx<System.States.size()) {
          sprintf(tempDbl, "%e", System.States[idx].Value);
          Serial.print("{\"sValue\":\""+ String(tempDbl) +"\",\"idx\":"+idx+"}");
        }
      }
      break;
      case 0x04: { //Count number of Params
          Serial.print("{\"pCount\":" + String(System.Params.size()) + "}");
      }
      break;
      case 0x05: { //Read state
        uint8_t idx = Serial.read();
        if (idx<System.Params.size())
          Serial.print("{\"param\":\""+ System.Params[idx].Name +"\",\"idx\":"+idx+"}");
      }
      break;
      case 0x06: { //Write state
        uint8_t idx = Serial.read();
        if (idx<System.Params.size()) {
          sprintf(tempDbl, "%e", System.Params[idx].Value);
          Serial.print("{\"pValue\":\""+ String(tempDbl) +"\",\"idx\":"+idx+"}");
        }
      }
      break;
      case 0x07: { //Write state
        char InBytes[100];
        uint8_t idx = Serial.read();
        if (idx<System.Params.size()) {
          Serial.readBytes(InBytes, InCount-2);
          double TempPVal = atof (InBytes);
          System.Params[idx].Value = TempPVal;
          sprintf(tempDbl, "%e", System.Params[idx].Value);
          Serial.print("{\"pValue\":\""+ String(tempDbl) +"\",\"idx\":"+idx+"}");        
        }
      }
      break;
      default:
      break;
    }
    
  }*/
  delay(100);
}
