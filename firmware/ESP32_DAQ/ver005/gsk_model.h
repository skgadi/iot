#include <ArduinoJson.h>
#include <vector>
#include <math.h>
using namespace std;

#include "EEPROM.h"
#define EEPROM_SIZE 1000

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

enum VARTYPES {
  VT_STATE, VT_PARAM, VT_IN, VT_OUT, VT_ST, VT_PWM
  };

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

void writeToEEPROM() {
  char isFactoryReset = EEPROM.readByte(0);
  EEPROM.writeByte(0, 0);
  if (isFactoryReset == 1) {
    int count = System.Params.size();
    int Address = 1;
    for (char i=0; i<count; i++) {
      EEPROM.writeDouble(Address, System.Params[i].Value);
      Address += sizeof(double);
    }
    EEPROM.commit();
  }
}

void updateEEPROM(VARTYPES vt, double value, int idx=0) {
  int Address;
  switch (vt) {
    case VT_PARAM:
      Address = 1 + sizeof(double)*idx;
  }
  EEPROM.writeDouble(Address, value);
  EEPROM.commit();
  //Serial.println(EEPROM.readDouble(Address));  
}

void readFromEEPROM() {
  int count = System.Params.size();
  int Address = 1;
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
void handleInputMessage();
void resetToFactorySettings();

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

  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("EEPROM failed");
    delay(1000);
    ESP.restart();
  }

  writeToEEPROM();
  readFromEEPROM();
  xTaskCreatePinnedToCore(realtimeActivity, "realtimeActivity", 32768, (void*)&System, 2,  NULL,  ARDUINO_RUNNING_CORE);
}


boolean isRestart=false;
void loop() {
  if (isRestart) {delay(1000); ESP.restart();}
  if (Serial.available()) {
    DeserializationError error = deserializeJson(JsonCodeIn, Serial);
    if (!error) {
      handleInputMessage();
      serializeJson(JsonCodeOut, Serial);
    } else {
      JsonCodeOut.clear();
      JsonCodeOut["B"] = -1;
      serializeJson(JsonCodeOut, Serial);
    }
    JsonCodeIn.clear();
  }
}

void handleInputMessage() {
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
    case 6: { //write pValue
      int i = JsonCodeIn["i"];
      double val = JsonCodeIn["v"];
      if (i>=0 && i<System.Params.size()) {
        System.Params[i].Value = val;
        updateEEPROM(VT_PARAM, val, i);
        JsonCodeOut["i"] = i;
        JsonCodeOut["v"] = val;
      }
    }
    break;
    case 7: { //Read ts
      JsonCodeOut["v"] = System.T_S;
    }
    break;
    case 8: { //write ts
      double val = JsonCodeIn["v"];
      System.T_S = val;
      JsonCodeOut["v"] = System.T_S;
    }
    break;
    case 9: { //ResetToFactorySettings
      resetToFactorySettings();
    }
    break;
    case 10: { //Reset device
      isRestart = true;
    }
    break;
  }
}

void resetToFactorySettings() {
  EEPROM.writeByte(0, 1);
  EEPROM.commit();
  isRestart = true;
}
