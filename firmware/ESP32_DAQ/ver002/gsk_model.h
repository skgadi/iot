
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

char cmdByte, InBytes[100];
void loop() {
  int InCount = Serial.available();
  double recValue=0;
  if (InCount) {
    cmdByte = Serial.read();
    switch (cmdByte) {
      case 'A': { //set sampling time
          Serial.readBytes(InBytes, InCount);
          double Temp_T_S = atof (InBytes);
          if (Temp_T_S>=0.001) {
            System.T_S = Temp_T_S;
            Serial.print("{t_s:"+ String(Temp_T_S) +"}");
          }
        }
      break;
      case 'B': { //Count number of states
          Serial.print("{sCount:" + String(System.States.size()) + "}");
      }
      break;
      case 'C': { //Read state
        uint8_t idx = Serial.read();
        if (idx<System.States.size())
          Serial.print("{state:'"+ System.States[idx].Name +"',idx:'"+idx+"'}");
      }
      break;
      case 'D': { //Read state
        uint8_t idx = Serial.read();
        String tempItem = Serial.readString();
        if (idx<System.States.size())
          System.States[idx].Name = tempItem;
          Serial.print("{state:'"+ System.States[idx].Name +"',idx:'"+idx+"'}");
      }
      break;
      default:
      break;
    }
    
  }
  delay(100);
}
