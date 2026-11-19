# Requirements and Backlog

## Requirements

| ID | Requirement | Priority | Verification | Status |
|---|---|---|---|---|
| FR-01 | Measure air temperature inside the growing area | Must | `air_inside_c` in every record | Done (simulated) |
| FR-02 | Measure air temperature outside the growing area | Must | `air_outside_c` | Done (simulated) |
| FR-03 | Measure nutrient-water temperature | Must | `water_c` | Done (simulated) |
| FR-04 | Measure relative humidity inside | Must | `humidity_inside_pct` | Done (simulated) |
| FR-05 | Repeated measurements over time | Must | `seq`/`uptime_ms` increase; T-08 | Done |
| FR-06 | Processing in the embedded application | Must | Firmware validates, alarms, serialises | Done |
| FR-07 | Communicate readings to an external system | Must | MQTT to gateway; T-11 (local), T-13 (Wokwi) | Verified in local Wokwi run; see test report |
| FR-08 | Handle faulty/unreasonable values without stopping | Must | T-01 – T-03, T-08 | Done |
| FR-09 | No DHT11/DHT22 | Must | BOM and code review | Done |
| FR-10 | HAL so simulation can be replaced by real sensors (teacher) | Must | `include/hal/sensors.h`, T-03 | Done |
| FR-11 | Store history for future analysis (customer letter) | Should | SQLite, T-08 | Done |
| FR-12 | Early warning before a limit is reached (customer letter) | Could | T-09, T-10 | Done |
| NFR-01 | Separate acquisition, validation and communication | Should | Code review | Done |
| NFR-02 | No credentials in Git | Must | `.gitignore` | Done |
| NFR-03 | Non-blocking firmware, reconnects automatically | Should | Code review, T-16/T-17 | Controlled reconnect and long run verified locally; T-16 limitations in test report |
| NFR-04 | Documentation another developer can follow | Must | README + docs | Done; team review open |

## Prioritised backlog

| ID | Task | Priority | Owner | Status |
|---|---|---|---|---|
| B-01 | Analyse customer request, requirements | Must | Gayathri, Desmond | Done |
| B-02 | Sensor comparison with sources | Must | Gayathri (draft), Desmond (sources) | Done |
| B-03 | Measurement model, validation, serial JSON | Must | Gayathri | Done (23 Oct) |
| B-04 | HAL + simulated sensors with faults | Must | Desmond | Done (PR) |
| B-05 | MQTT with reconnect, buffer, Last Will | Must | Desmond | Done (PR) |
| B-06 | Gateway: receive, validate, store | Must | Desmond | Done (PR) |
| B-07 | Review and merge PR | Must | Gayathri | Open |
| B-08 | Wokwi tests T-13 – T-17, update test report | Must | Khaled | Completed locally 19 Nov; T-16 scenario limitations documented |
| B-09 | Alarm boundary tests (`test/test_alarm`: exactly at 18/28/24/40/80 → no alarm) | Should | Khaled | Completed locally 19 Nov: 5/5 passed |
| B-10 | Run tests on own machine (T-18) | Should | Gayathri, Khaled | Khaled's Windows run completed locally: 30/30 firmware and 19/19 gateway; Gayathri's separate run not verified |
| B-11 | Analysis report + graph | Could | Desmond | Done (PR) |
| B-12 | Demo script and recorded customer video | Must | All | Open |
| B-13 | Individual reflection report | Must | Each member | Open |
| B-14 | SHT40/DS18B20 drivers | Later | Desmond | Compiles; hardware test out of scope |
| B-15 | Real hardware validation, own broker with TLS | Future | — | Recommendation |
