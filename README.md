# MicroHydros Prototype

A small IoT prototype for collecting four environmental measurements for the HydroGreen Fingers MicroHydros concept. It provides ESP32/PlatformIO firmware with simulated sensors and a no-hardware Python demo.

## What it demonstrates

- Repeated inside/outside air temperature, nutrient-water temperature, and inside relative-humidity readings.
- A shared measurement record, range and finite-value validation, and JSON output.
- ESP32 serial output, with optional Wi-Fi/MQTT publishing.
- Host simulation for a demonstration without an ESP32 or sensors.
- Native C++ validator tests.

All firmware readings are simulated; physical sensor drivers are not integrated. DHT11 and DHT22 are not used. Do not claim physical sensor or broker results unless you perform and document those tests.

## Run the no-hardware demo

Requires Python 3.9 or newer:

```powershell
py -3 tools/demo.py --count 5 --interval 1
```

Every output line is a JSON measurement. Add `--invalid-every 3` to demonstrate invalid-reading handling. To publish to MQTT, install the optional dependency and provide a broker:

```powershell
py -3 -m pip install -r requirements.txt
py -3 tools/demo.py --count 5 --interval 1 --mqtt-host localhost
```

In another terminal, subscribe with an MQTT client such as:

```powershell
mosquitto_sub -h localhost -t microhydros/measurement -v
```

The Python publisher uses MQTT without TLS or authentication by default. Use only a trusted local broker for the demo; configure security before deployment.

## Build the ESP32 firmware

Install Visual Studio Code and PlatformIO, connect an ESP32 DevKit, then run:

```powershell
pio run -e esp32dev
pio run -e esp32dev -t upload
pio device monitor -b 115200
```

The default firmware emits simulated JSON over serial every five seconds. Run native tests with:

```powershell
pio test -e native
```

## Enable ESP32 MQTT

1. Copy `include/secrets.example.h` to `include/secrets.h` and enter local Wi-Fi and broker settings.
2. Add `-DENABLE_MQTT` to the `esp32dev` environment's `build_flags` in `platformio.ini`.
3. Build/upload and verify `microhydros/measurement` with a subscriber.

`include/secrets.h` is ignored by Git. Never commit real credentials. Shared public brokers are not suitable for sensitive data.

## Project material

- [Project plan and schedule](docs/01-project-plan.md)
- [Requirements and backlog](docs/02-requirements-backlog.md)
- [Architecture](docs/03-architecture.md)
- [Sensor decisions](docs/04-sensor-decisions.md)
- [Test plan and results](docs/05-test-report.md)
- [Demo and handover](docs/06-demo-handover.md)
- [Individual reflection report](docs/07-individual-reflection.md)

## Status

This package was assembled on 23 October 2026, after the assignment's stated 31 August–25 September 2026 project period. The date is recorded rather than backdated. Run the demo and tests before submission and update the report with actual results, hardware, broker, and contributors.