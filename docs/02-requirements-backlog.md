# Requirements and Backlog

## Requirements

| ID | Requirement | Priority | Verification |
|---|---|---|---|
| FR-01 | Measure air temperature inside the growing area. | Must | Record contains `air_inside_c` |
| FR-02 | Measure air temperature outside the growing area. | Must | Record contains `air_outside_c` |
| FR-03 | Measure nutrient-water temperature. | Must | Record contains `water_c` |
| FR-04 | Measure relative humidity inside the growing area. | Must | Record contains `humidity_inside_pct` |
| FR-05 | Produce repeated measurements over time. | Must | Run cycles and inspect timestamps |
| FR-06 | Process readings in the embedded application. | Must | Firmware creates and validates a common record |
| FR-07 | Communicate readings externally. | Must | Verify serial/host output; verify MQTT separately if claimed |
| FR-08 | Reject non-finite and unreasonable values without stopping future cycles. | Must | Unit and repeated-cycle fault tests |
| FR-09 | Do not use DHT11 or DHT22. | Must | BOM and code review |
| NFR-01 | Separate acquisition, validation, and communication. | Should | Code review |
| NFR-02 | Keep credentials out of version control. | Must | Ignore/configuration review |
| NFR-03 | Document limitations, evidence, and next steps. | Must | Handover review |

## Prioritized backlog

| ID | Task | Priority | Status/evidence to update |
|---|---|---|---|
| B-01 | Analyze customer requirements | Must | Drafted here; team confirmation pending |
| B-02 | Compare air and water sensors | Must | Draft recommendation; datasheet verification pending |
| B-03 | Implement measurement model and simulation | Must | Code included; run demo |
| B-04 | Implement range and finite-value validation | Must | Code/tests included; run native tests |
| B-05 | Produce JSON records on host and ESP32 Serial | Must | Code included; verify locally |
| B-06 | Verify MQTT with an independent subscriber | Must | Not verified until observed and recorded |
| B-07 | Integrate SHT40 and sealed DS18B20 probe | Later | Not implemented |
| B-08 | Test stability against reference instruments | Later | Not performed |
| B-09 | Add persistent history and alerts | Future | Out of scope |

Update statuses from actual team work before hand-in. Documentation or placeholder code alone does not make an item Done.