# MicroHydros Prototype

IoT/embedded prototype for HydroGreen Fingers AB's MicroHydros concept (Projekt 1, IoT25, JENSEN YH).
An ESP32 reads four environmental values, validates them, raises alarms and publishes JSON over MQTT
to a gateway that stores history and warns when a value is heading towards a limit.

| Measurement | JSON field | Planned physical sensor |
|---|---|---|
| Air temperature inside | `air_inside_c` | Sensirion SHT40 |
| Air temperature outside | `air_outside_c` | DS18B20 |
| Water / nutrient temperature | `water_c` | DS18B20 in sealed probe |
| Relative humidity inside | `humidity_inside_pct` | Sensirion SHT40 |

**Sensors are simulated.** The team has no hardware budget, so values come from simulated sensors behind
a Hardware Abstraction Layer (HAL); see [docs/03-architecture.md](docs/03-architecture.md). Drivers for
SHT40 and DS18B20 implement the same HAL and compile (`esp32dev_real`) but have **not** been tested on
hardware. DHT11/DHT22 are not used (customer requirement).

```
Simulated sensors ─┐
                   ├─ HAL ─ SensorManager ─ Validator + Alarms ─ JSON ─┬─ Serial
SHT40 / DS18B20 ───┘        (ESP32 firmware, C++)                       └─ Wi-Fi/MQTT ─ broker ─ gateway ─ SQLite ─ analysis
```

## Repository layout

| Path | Content |
|---|---|
| `include/hal/`, `src/hal/` | HAL interfaces, simulated sensors, SHT40/DS18B20 drivers |
| `src/sensor_manager.cpp`, `validator.cpp`, `alarm.cpp`, `payload.cpp` | Hardware-independent firmware core |
| `src/main.cpp`, `src/communication.cpp` | ESP32 application: schedule, LEDs, buttons, Wi-Fi/MQTT |
| `src/host/host_main.cpp` | Same core compiled for the PC (testing without an ESP32) |
| `test/` | Native Unity tests of the core |
| `gateway/` | Python receiver (MQTT/stdin → SQLite), trend warnings, analysis report and graph |
| `diagram.json`, `wokwi.toml` | Wokwi simulation of the ESP32 with LEDs and buttons |
| `docs/` | Project plan, requirements, architecture, sensors, tests, sprint log, handover |
| `tools/demo.py` | Original Python-only demo (kept for reference; the firmware is the main path) |

## Dependencies

- [PlatformIO](https://platformio.org/) (VS Code extension or `pip install platformio`)
- Python 3.10+ and `pip install -r requirements.txt` (paho-mqtt, matplotlib, pytest)
- For the simulator: VS Code extension **Wokwi Simulator** (requires a free Wokwi account/licence)
- Optional: an MQTT client such as MQTT Explorer or `mosquitto_sub`

## Quick start

### 1. Run the tests (no hardware)

```bash
pio test -e native            # 30 firmware unit tests
python -m pytest gateway      # 19 gateway tests
```

### 2. Run the ESP32 in Wokwi and receive data on your PC

```bash
pio run -e wokwi                                                    # build firmware for the simulator
# VS Code: F1 -> "Wokwi: Start Simulator" (uses diagram.json + wokwi.toml)
python -m gateway.receiver --mqtt-host broker.hivemq.com --device mh-grupp6
```

In the simulator: the green LED lights when MQTT is connected, the red LED on alarm or sensor fault.
**Heat event** (orange button) ramps the inside temperature up so the trend warning and alarm can be
demonstrated; **Sensor fault** (blue button) injects read errors and NaN values. Time is accelerated
120x in this build (one simulated day = 12 minutes).

> The Wokwi build uses the public broker `broker.hivemq.com` without TLS or login. Anyone can read the
> topic. This is acceptable only because the data is simulated and harmless; see the security section in
> [docs/03-architecture.md](docs/03-architecture.md).

### 3. Analyse the stored history

```bash
python -m gateway.analyze --db data/microhydros.db --out reports
```

Produces `reports/<device>_rapport.md` (statistics, faults, alarms, trend, insights; in Swedish for the
customer) and `reports/<device>_graf.png`.

### 4. Without Wokwi: same firmware core on the PC

```bash
pio run -e native_sim
.pio/build/native_sim/program --count 400 --time-scale 120 --fault-at 100 --heat-at 300 \
  | python -m gateway.receiver --stdin --db data/host.db
```

This runs the same C++ core (HAL, SensorManager, validation, alarms, JSON) as the ESP32. It exists for
testing; the embedded deliverable is the ESP32 firmware.

### 5. Physical ESP32

```bash
pio run -e esp32dev -t upload && pio device monitor -b 115200     # simulated sensors, JSON on Serial
```

For MQTT on a real board: copy `include/secrets.example.h` to `include/secrets.h` (ignored by Git),
fill in Wi-Fi and broker, and add `-D ENABLE_MQTT` to `build_flags`. With real sensors:
`pio run -e esp32dev_real` (SHT40 on I2C SDA 21/SCL 22, DS18B20 bus on GPIO 4 with 4.7 kΩ pull-up).

## Data format (schema 1)

```json
{"schema":1,"device":"mh-grupp6","seq":42,"uptime_ms":210000,"time_scale":120,
 "air_inside_c":23.41,"air_outside_c":21.02,"water_c":null,"humidity_inside_pct":61.80,
 "valid":false,"faults":["water"],"alarms":[]}
```

`seq` lets the receiver detect lost messages; faulty channels are `null`; `time_scale` is 1 on real
hardware. Topics: `microhydros/<device>/measurement` and `microhydros/<device>/status`
(`online`/`offline`, retained, Last Will).

## Documentation

1. [Project plan (current, Swedish)](docs/projektplan.md) · [original plan](docs/01-project-plan.md)
2. [Requirements and backlog](docs/02-requirements-backlog.md)
3. [Architecture](docs/03-architecture.md)
4. [Sensor decisions](docs/04-sensor-decisions.md)
5. [Test plan and results](docs/05-test-report.md)
6. [Sprint log](docs/06-sprint-log.md)
7. [Handover and recommendations](docs/07-handover.md)
8. [Demo script for the customer video (Swedish)](docs/08-demomanus.md)

## Status (14 Nov 2026)

Firmware builds for `esp32dev`, `wokwi` and `esp32dev_real`; native and gateway tests pass; the data path
was verified end to end on a PC (host build → MQTT broker → receiver → SQLite → analysis). Not yet done:
a run in Wokwi against the public broker and the team's own test runs; see the test report. The repository
was started by Gayathri (first commit 4 Sep, main upload 23 Oct 2026); the HAL, MQTT and gateway work
was added in November 2026 during the re-take. Dates are recorded as they happened.

Local testing update (19 Nov 2026): Khaled ran Wokwi T-13–T-17 and T-18 on Windows. After a test setup fix, 30/30 firmware and 19/19 gateway tests passed. T-16 covers controlled disconnection and reconnection scenarios with limits described in the [test report](docs/05-test-report.md). These results do not establish the status of another team member's separate test run.
