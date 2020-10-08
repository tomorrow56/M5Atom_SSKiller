#include "M5Atom.h"
#include <BleMouse.h>  // https://github.com/T-vK/ESP32-BLE-Mouse

BleMouse bleMouse("M5Atom SSKiller");

int amount = 100;
int interval_sec = 5;
volatile boolean bleEnabled = false;

void task0(void* arg) {
  while (1) {
    bleEnabled = bleMouse.isConnected();
    if(bleEnabled == true){
      M5.dis.drawpix(0, 0xf00000);  // LED Green
    }else{
      M5.dis.drawpix(0, 0x00f000);  // LED Red
    }
    vTaskDelay(100);
  }
}

void task1(void* arg) {
  while (1) {
    if(bleEnabled == true){
      movePointer();
    }
    vTaskDelay(interval_sec * 1000);
  }
}

void movePointer(){
  unsigned long startTime;
  M5.dis.drawpix(0, 0x0000f0);  // LED Blue
  Serial.println("Move mouse pointer");

  startTime = millis();
  while(millis()<startTime + amount){
    bleMouse.move(0,-1);
    vTaskDelay(20);
  }
  startTime = millis();
  while(millis()<startTime + amount) {
    bleMouse.move(0,1);
    vTaskDelay(20);
  }
  M5.dis.drawpix(0, 0xf00000);  // LED Green
}

void setup(){
  // M5.begin(SerialEnable, I2CEnable, DisplayEnable);
  M5.begin(true, false, true);

  // Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();

  // xTaskCreatePinnedToCore(func,"name",Stuck size,NULL,priority,Task pointer,Core ID)
  // Core ID: 0 or 1 or tskNO_AFFINITY
  xTaskCreatePinnedToCore(task0, "Task0", 4096, NULL, 1, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(task1, "Task1", 4096, NULL, 1, NULL, tskNO_AFFINITY);
}

void loop(){
}
