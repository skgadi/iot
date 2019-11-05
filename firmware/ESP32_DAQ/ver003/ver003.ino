#include "EEPROM.h"
#include "gsk_model.h"

void setup() {
  AppendAState("System out ($y$)", 1);
  AppendAState("Control out ($u$)", 0.0000000000001);
  AppendAState("Error ($e$)", 0);

  AppendAParam("$K_P$", 0);
  AppendAParam("$K_I$", 0);
  AppendAParam("$K_D$", 0);
  AppendAParam("Sampling time ($T_S$)", 0);
  AppendAParam("PWM Frequency", 0);

  AppendAnInputPort("Sensor 1 ($y_1(t)$)", 33);
  AppendAnInputPort("Sensor 2 ($y_2(t)$)", 34);

  AppendAnOutputPort("Control out 1 ($u_1(t)$)", 2);
  AppendAnOutputPort("Control out 2 ($u_2(t)$)", 3);

  gskInit();
  /*ledcSetup(0, 1, 2);
  ledcAttachPin(2, 0);*/
  pinMode(2, OUTPUT);
}

void rt_loop (MODEL &Sys) {
  digitalWrite(2, ((int)(Sys.States[0].Value++)%2==0));
}
