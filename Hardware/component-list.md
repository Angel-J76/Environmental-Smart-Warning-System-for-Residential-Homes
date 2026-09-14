# Component List

Full bill of materials used in the prototype build. Total prototype cost: **~$133** (budgeted $600).

| # | Component | Description | Unit Cost | Qty |
|---|-----------|--------------|-----------|-----|
| 1 | Arduino Uno REV3 | Primary microcontroller — sensor read + relay/alarm logic | $27.60 | 1 |
| 2 | LCD Display (16x2) | On-device status/readout screen | $5.99 | 2 |
| 3 | ESP8266 NodeMCU | Wi-Fi microcontroller — pushes data/alerts to the Blynk mobile app | $16.39 | 1 |
| 4 | DHT11 | Temperature and humidity sensor | $11.99 | 1 |
| 5 | SunFounder 2-Channel DC 5V Relay | Triggers cooling fan and buzzer(s) on alert | $6.79 | 1 |
| 6 | Tatoko DC 3–24V Electronic Buzzer Alarm Sounder | Audible alarm output | $9.89 | 2 |
| 7 | MQ-series Gas Sensor(s) | Detects harmful/combustible gases (e.g. methane) | $15.88 | 1 |
| 8 | DC Brushless Cooler Fan | Ventilation response on hazard detection | $10.99 | 1 |
| 9 | LED (red/green) | Visual status indicators (safe/alert) | $11.99 | 1 |
| 10 | Breadboard | Prototyping connections | $8.79 | 1 |
| 11 | Breadboard Jumper Wires | Wiring | $6.98 | 1 |

**Also used:** C2G 2m USB cables (x2) for Arduino and NodeMCU power/data.

## Power & Connectivity Notes
- Arduino Uno is powered/programmed via USB from a laptop or 5V wall adapter.
- ESP8266 NodeMCU connects to the same 2.4GHz Wi-Fi network as the Blynk mobile app for remote alerts.
- Relay module switches the fan and buzzer circuits independently of the microcontroller's logic-level output.

## Reference Images
- [`schematic.png`](./schematic.png) — full wiring schematic (breadboard-level)
- [`system-architecture.png`](./system-architecture.png) — component/data-flow architecture
- [`parts-table.png`](./parts-table.png) — original parts summary slide (visual reference)
