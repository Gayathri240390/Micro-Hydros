# Handover and Recommendations

For HydroGreen Fingers AB (customer) and the next development team.

## How we interpreted the request

The customer wants a first technical prototype that collects four environmental values repeatedly,
gets them off the device to an external system, handles bad data, and is designed so that history and
early warnings are possible later. A small, stable and well-motivated solution matters more than many
features. DHT11/DHT22 are excluded. Our constraint: no hardware or budget, so the sensors are simulated
behind a HAL that the teacher required, with real drivers prepared.

## What the prototype does

- ESP32 firmware reads inside/outside air temperature, water temperature and inside humidity every 5 s
  (simulated), validates each value, raises threshold alarms (LED) and publishes JSON over MQTT and Serial.
- Faults are isolated per channel: one broken sensor does not stop the others, and a broken sensor is
  reported as a fault, not as "too hot".
- Lost connectivity: up to 20 readings are buffered and sent later; the gateway detects lost messages by
  sequence number and device disappearance via MQTT Last Will.
- The gateway stores all history in SQLite, warns when a value is heading to a limit within an hour,
  and produces a report with statistics, faults, alarms and a graph.

## Answers to the customer's questions

| Question | Our answer (prototype level) |
|---|---|
| Which sensors suit MicroHydros? | SHT40 for inside air and humidity, DS18B20 probes for water and outside air. Motivation and sources in [04-sensor-decisions.md](04-sensor-decisions.md). Not yet verified with physical parts. |
| How often should data be measured? | 30–60 s in the product (5 s in the demo). Sensors respond within seconds; the small enclosure changes over minutes. Validate with real data. |
| How can data be structured and communicated? | One versioned JSON object per reading with device id and sequence number, MQTT topic per device, history in a database. See README and [03-architecture.md](03-architecture.md). |
| How can faulty or unreasonable values be detected? | Driver errors, NaN and physically impossible values are flagged per channel and stored as missing; thresholds are separate from plausibility; the gateway rejects malformed payloads and detects gaps and restarts. |
| How should a future product be built to be easy to develop further? | Keep the HAL; add drivers, not changes to the core. Keep the payload versioned. Move the gateway logic to a cloud service subscribing to the same topics. |
| Risks and limitations? | Below. |

## Known limitations

- Sensor values are simulated; real drivers compile but are untested on hardware.
- The demo broker is public and unencrypted; no authentication.
- The device buffer is RAM only (20 readings); data is lost on reboot during an outage.
- Device time is uptime, not wall-clock time; the gateway adds UTC time on arrival.
- DS18B20 sensors are addressed by bus index, which is not stable when probes are added or swapped.
- Alarm thresholds are assumptions, not agronomic values from the customer.
- Wokwi end-to-end and long-run tests passed in the local 19 Nov simulation. Reconnection and buffering were tested in controlled scenarios; network blackhole, Wi-Fi loss and broker shutdown were not verified. See [05-test-report.md](05-test-report.md).

## Recommendations for the next phase

1. Buy one SHT40 breakout and two sealed DS18B20 probes; run `esp32dev_real`, compare with a reference
   thermometer/hygrometer and record offsets.
2. Get agreed alarm thresholds per crop from HydroGreen Fingers.
3. Own MQTT broker with TLS and per-device credentials; then a cloud ingestion service using
   `gateway.schema` validation.
4. NTP time on the device and persistent buffering (flash) for longer outages.
5. Address DS18B20 probes by ROM code; non-blocking conversions.
6. Evaluate the SHT40 heater for high-humidity creep after a week of real data.
