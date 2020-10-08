#include "M5Atom.h"
#include <BleMouse.h>  // https://github.com/T-vK/ESP32-BLE-Mouse

BleMouse bleMouse("M5Atom SSKiller");

int amount = 100;
int interval_sec = 5;
// Variables to be shared by the main loop and ISR
volatile boolean bleEnabled = false;
volatile boolean moveFlag = false;

// Timer Interrupt setting
hw_timer_t * timer = NULL;

volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(){
  // Set the pointer move flag when BLE is enable
  portENTER_CRITICAL_ISR(&timerMux);
  
  if(bleEnabled == true){
    moveFlag = true;
  }

  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here
  // if you want to toggle an output
}

void movePointer(){
  unsigned long startTime;
  M5.dis.drawpix(0, 0x0000f0);  // LED Blue
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
  M5.dis.drawpix(0, 0xf00000);  // LED Green
}

void setup(){
  M5.begin(true, false, true);

  // Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider(80MHz/80 = 1MHz) for prescaler
  // (see ESP32 Technical Reference Manual for more info).
  timer = timerBegin(0, 80, true);
  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);
  // Set alarm to call onTimer function(value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, interval_sec * 1000000, true);
  // Start an alarm
  timerAlarmEnable(timer);
}

void loop(){
  bleEnabled = bleMouse.isConnected();
  
  if(bleEnabled == true){
    M5.dis.drawpix(0, 0xf00000);  // LED Green
  }else{
    M5.dis.drawpix(0, 0x00f000);  // LED Red
  }

  if(moveFlag == true){
    movePointer();
    moveFlag = false;
  }

  delay(100);
}
