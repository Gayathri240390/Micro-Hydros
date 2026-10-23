# Test Plan and Results

## Evidence rule

Mark a test Passed only after it has been run and observed. Source and procedures are included, but results must be updated on the actual machine and hardware. Simulation does not prove sensor accuracy; Serial output does not prove MQTT delivery.

## Automated checks

Run native validator tests:

```powershell
pio test -e native
```

Expected: four Unity tests pass: normal readings accepted, inclusive boundaries accepted, out-of-range humidity rejected, and NaN rejected.

Run the host demo:

```powershell
py -3 tools/demo.py --count 5 --interval 0 --invalid-every 3
```

Expected: five JSON records; record 3 is invalid; the remaining cycles continue.

## Test record

| Test | Method | Expected | Actual evidence | Status |
|---|---|---|---|---|
| T-01 Normal values | `pio test -e native` | In-range values accepted | Fill from output | Not run yet |
| T-02 Boundaries | `pio test -e native` | Inclusive limits accepted; outside limits rejected | Fill from output | Not run yet |
| T-03 Non-finite input | `pio test -e native` | NaN rejected | Fill from output | Not run yet |
| T-04 Repeated host simulation | `py -3 tools/demo.py --count 5 --interval 0.02` | Five complete records with increasing timestamps | Observed five complete JSON records; timestamps increased by about 20 ms | Passed |
| T-05 Invalid-reading recovery | `py -3 tools/demo.py --count 5 --interval 0.02 --invalid-every 2` | Bad record flagged; later record emitted | Records 2 and 4 had `water_c: -1.0` and `valid: false`; records 3 and 5 continued with `valid: true` | Passed |
| T-06 ESP32 build and serial | Build/upload and inspect serial monitor | Firmware builds and emits every five seconds | Record board, versions, and output | Not run; hardware required |
| T-07 MQTT delivery | Subscribe, then publish from configured firmware/host demo | Subscriber receives JSON | Record broker, subscriber, payload; no credentials | Not run yet |
| T-08 Physical sensor reasonableness | Compare installed channels to references | Stable, plausible readings within uncertainty | Record setup, samples, and references | Not run; sensors not integrated |

## Interpretation

Software guardrails are inside air -10 to 60 C, outside air -30 to 60 C, water 0 to 50 C, and humidity 0 to 100 percent. These are not crop-growth targets. Attach actual output before submission and distinguish unit, simulation, firmware, broker, and physical-measurement tests.