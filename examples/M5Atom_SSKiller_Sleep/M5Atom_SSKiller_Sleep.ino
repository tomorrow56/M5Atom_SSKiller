
#include "M5Atom.h"
#include <BleMouse.h>  // https://github.com/T-vK/ESP32-BLE-Mouse

BleMouse bleMouse("M5Atom SSKiller");

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds

int amount = 100;
int interval_sec = 5;
volatile boolean bleEnabled = false;
volatile boolean blePearing = false;

// RTC_DATA_ATTR int bootCount = 0;

void movePointerAndSleep(){
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
  if(blePearing == true){
    delay(5000);
  }
  M5.dis.drawpix(0, 0x000000);  // LED OFF
  delay(100);
  Serial.println("Going to sleep now");
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

//Print the wakeup reason for ESP32
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup(){
  // M5.begin(SerialEnable, I2CEnable, DisplayEnable);
  M5.begin(true, false, true);

  M5.dis.drawpix(0, 0x000000);  // LED OFF

  // Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();

/*    
 //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
*/

  print_wakeup_reason();

  // Set wakeup interval
  esp_sleep_enable_timer_wakeup(interval_sec * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(interval_sec) + " Seconds");
}

void loop(){
  M5.update();
  bleEnabled = bleMouse.isConnected();

  if (M5.Btn.wasPressed()){
    if(bleEnabled == false){
      M5.dis.drawpix(0, 0xf0f000);  // LED Yellow
      blePearing = true;
      Serial.println("Wait Pairing..");
      delay(1000);
    }
  }
  
  if(bleEnabled){
    M5.dis.drawpix(0, 0xf00000);
    movePointerAndSleep();
  }else{
    if(blePearing == true){
      M5.dis.drawpix(0, 0xf0f000);  // LED Yellow
    }else{
      M5.dis.drawpix(0, 0x00f000);
    }
  }

  delay(100);
}
