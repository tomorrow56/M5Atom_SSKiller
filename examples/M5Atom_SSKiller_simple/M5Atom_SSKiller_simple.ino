#include "M5Atom.h"
#include <BleMouse.h>  // https://github.com/T-vK/ESP32-BLE-Mouse

BleMouse bleMouse("M5Atom SSKiller");

int amount = 100;
int interval_sec = 5;

void setup() {
  M5.begin(true, false, true);

  // Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

void loop() {
  if(bleMouse.isConnected()) {

    unsigned long startTime;

    M5.dis.drawpix(0, 0x0000f0);

    Serial.println("Move mouse pointer");
    startTime = millis();
    while(millis()<startTime + amount){
      bleMouse.move(0,-1);
      delay(20);
    }

    startTime = millis();
    while(millis()<startTime + amount) {
      bleMouse.move(0,1);
      delay(20);
    }

    M5.dis.drawpix(0, 0xf00000);
    delay(interval_sec * 1000);
  }else{
    M5.dis.drawpix(0, 0x00f000);
  }
}
