/*
  Safe Environment Smart Warning System — Arduino Uno (main controller)
  ------------------------------------------------------------------
  Responsibilities:
    - Read temperature/humidity (DHT11) and gas concentration (MQ-series sensor)
    - Drive the 16x2 LCD status display
    - Drive local alert outputs (LEDs + buzzer)
    - Trigger the relay (fan) when a hazard is detected
    - Forward sensor readings to the ESP8266 over Serial for Wi-Fi/Blynk alerts

  NOTE: Pin numbers below match the project schematic in /hardware/schematic.png.
  Adjust if your physical wiring differs.
*/

#include <DHT.h>
#include <LiquidCrystal.h>

// ---------- Pin Definitions ----------
#define DHTPIN        2       // DHT11 data pin
#define DHTTYPE       DHT11
#define GAS_SENSOR_PIN A0     // Analog gas sensor output

#define LED_GREEN_PIN 8       // "Safe" indicator
#define LED_RED_PIN   9       // "Alert" indicator
#define BUZZER_PIN    10      // Local alarm buzzer
#define RELAY_PIN     11      // Controls cooling fan

// LCD pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 6, 5, 4, 3, 12);

DHT dht(DHTPIN, DHTTYPE);

// ---------- Thresholds (tune during calibration) ----------
const int   GAS_THRESHOLD   = 400;   // Raw analog reading (0-1023) that trips a gas alert
const float TEMP_THRESHOLD  = 45.0;  // Degrees C considered "high temperature"
const float HUMIDITY_MAX    = 60.0;  // % RH — upper bound for healthy indoor air
const unsigned long READ_INTERVAL = 2000; // ms between sensor reads

unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(9600);   // Also used to forward data to the ESP8266
  lcd.begin(16, 2);
  dht.begin();

  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(LED_GREEN_PIN, HIGH); // Start in "safe" state
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
}

void loop() {
  unsigned long now = millis();
  if (now - lastReadTime < READ_INTERVAL) return;
  lastReadTime = now;

  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature(); // Celsius
  int   gasLevel     = analogRead(GAS_SENSOR_PIN);

  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error   ");
    return;
  }

  bool gasAlert  = gasLevel > GAS_THRESHOLD;
  bool tempAlert = temperature > TEMP_THRESHOLD;
  bool hazard    = gasAlert || tempAlert;

  updateDisplay(temperature, humidity, gasLevel, hazard);
  updateOutputs(hazard);

  // Forward a simple CSV line to the ESP8266 for Wi-Fi/Blynk reporting:
  // temperature,humidity,gasLevel,hazard(0/1)
  Serial.print(temperature); Serial.print(",");
  Serial.print(humidity);    Serial.print(",");
  Serial.print(gasLevel);    Serial.print(",");
  Serial.println(hazard ? 1 : 0);
}

void updateDisplay(float temp, float hum, int gas, bool hazard) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temp, 1); lcd.print("C H:"); lcd.print(hum, 0); lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(hazard ? "!! HAZARD !!" : "Status: Safe");
}

void updateOutputs(bool hazard) {
  digitalWrite(LED_GREEN_PIN, hazard ? LOW : HIGH);
  digitalWrite(LED_RED_PIN, hazard ? HIGH : LOW);
  digitalWrite(BUZZER_PIN, hazard ? HIGH : LOW);
  digitalWrite(RELAY_PIN, hazard ? HIGH : LOW); // Trip fan/ventilation on hazard
}
