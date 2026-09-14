/*
  Safe Environment Smart Warning System — ESP8266 NodeMCU (connectivity module)
  ------------------------------------------------------------------------------
  Responsibilities:
    - Receive sensor readings from the Arduino Uno over Serial (CSV format)
    - Push live readings to the Blynk mobile app
    - Drive a secondary LED/buzzer alert bank for redundancy
    - Handle a second gas sensor channel wired directly to the NodeMCU

  Requires: Blynk library (install via Arduino Library Manager)
  Configure your Wi-Fi + Blynk credentials in config_template.h before flashing.
*/

#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_DEVICE_NAME   "SafeHomeWarningSystem"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "config_template.h" // WIFI_SSID, WIFI_PASS, BLYNK_AUTH_TOKEN

// ---------- Pin Definitions ----------
#define GAS_SENSOR_PIN2 A0   // Secondary gas sensor on the NodeMCU
#define LED_GREEN_PIN   D1
#define LED_RED_PIN     D2
#define BUZZER_PIN      D3

// ---------- Blynk Virtual Pins ----------
#define V_TEMP   V0
#define V_HUM    V1
#define V_GAS    V2
#define V_HAZARD V3

const int GAS_THRESHOLD_2 = 400;
BlynkTimer timer;

void setup() {
  Serial.begin(9600); // Listening for CSV data from the Arduino Uno

  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_GREEN_PIN, HIGH);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
  timer.setInterval(2000L, checkLocalGasSensor);
}

void loop() {
  Blynk.run();
  timer.run();
  readFromArduino();
}

// Parses "temperature,humidity,gas,hazard" sent by the Arduino Uno
void readFromArduino() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  float temperature, humidity;
  int gasLevel, hazard;

  int parsed = sscanf(line.c_str(), "%f,%f,%d,%d",
                       &temperature, &humidity, &gasLevel, &hazard);
  if (parsed != 4) return; // Ignore malformed lines

  Blynk.virtualWrite(V_TEMP, temperature);
  Blynk.virtualWrite(V_HUM, humidity);
  Blynk.virtualWrite(V_GAS, gasLevel);
  Blynk.virtualWrite(V_HAZARD, hazard);

  setAlertOutputs(hazard == 1);

  if (hazard == 1) {
    Blynk.logEvent("hazard_detected", "Warning: hazard detected at home!");
  }
}

// Independent check of the NodeMCU's own gas sensor channel,
// in case the Arduino <-> ESP8266 serial link drops.
void checkLocalGasSensor() {
  int gasLevel = analogRead(GAS_SENSOR_PIN2);
  bool hazard = gasLevel > GAS_THRESHOLD_2;
  if (hazard) {
    setAlertOutputs(true);
    Blynk.logEvent("hazard_detected", "Warning: secondary gas sensor triggered!");
  }
}

void setAlertOutputs(bool hazard) {
  digitalWrite(LED_GREEN_PIN, hazard ? LOW : HIGH);
  digitalWrite(LED_RED_PIN, hazard ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, hazard ? HIGH : LOW);
}
