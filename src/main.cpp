#include <Arduino.h>
#include <ESP8266WiFi.h>
// voor wake up reason magic
extern "C" {
  #include "user_interface.h"
}
// beeper: D5 + D6
// knop: D1 + D2

void klik();
void zzz();

int interval = 1000;
volatile boolean pushed = false;

void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_OFF); // stroom besparen
  delay(2000); // even wachten op een eventuele firmware update
  Serial.begin(115200);
  Serial.println("HYPERKNOP");

  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D1, OUTPUT); 
  pinMode(D8, OUTPUT); 
  pinMode(D2, INPUT_PULLUP);
  digitalWrite(D5, LOW); // ground beeper
  digitalWrite(D1, LOW); // ground knop
  digitalWrite(D8, LOW); // LED uit
  attachInterrupt(digitalPinToInterrupt(D2), klik, FALLING);

  struct rst_info *rstInfo = system_get_rst_info();
  uint32_t reason = rstInfo->reason;
  Serial.println(reason);
  // na sleep 5: meteen bliepen
  // na reset 6 (reset tijdens sleep geeft ook 5!)
  // na poweron 0: niet meteen bliepen, eerst slapen
  if (reason != 5) {
    // led even aan zodat je weet dat ie het doet
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(D8, HIGH);
    delay(2000);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(D8, LOW);
    zzz(); // code execution stopt, goto setup()
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D6, HIGH);
  digitalWrite(LED_BUILTIN, LOW); // led = inverted
  digitalWrite(D8, HIGH); // externe led = niet inverted
  delay(100);
  digitalWrite(D6, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(D8, LOW);
  delay(interval);
  interval = interval * 0.9;

  if (pushed) {
    zzz();
  }
}

void zzz() {
  interval = 1000; 
  pushed = false; 
  Serial.println("zzz"); 
  ESP.deepSleep(600e6); // 600 sec in microsec
}

// interrupt service routine
ICACHE_RAM_ATTR // run deze functie in RAM
void klik() {
  pushed = true;
}
