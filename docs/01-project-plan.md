# Project Plan

## Purpose and scope

Demonstrate a small, maintainable IoT measurement pipeline for the MicroHydros hydroponic environment. The prototype covers four repeated measurements, data representation, plausibility validation, JSON output, and a communication path. The ESP32 firmware defaults to simulated inputs and Serial output; MQTT is optional. Physical drivers, cloud storage, alerts, controls, and a user interface are out of scope.

Success means each record contains all four readings, validation rejects out-of-range and non-finite values, the simulation runs repeatedly, and native validator tests pass. MQTT is a demonstrated result only after a real broker/subscriber test.

## Requirements interpretation

The customer requests inside/outside air temperature, nutrient-water temperature, inside relative humidity, repeated collection, external communication, and error handling. DHT11 and DHT22 are prohibited. The assignment also requires justified component selection, testing, documentation, and handover.

## Schedule and status

The assignment window was 31 August–25 September 2026. This package is assembled on 23 October 2026, after that window. This is a recovery/completion checklist, not a claim that work happened on the original dates.

| Work item | Sequence | Evidence/status |
|---|---|---|
| Requirements and architecture | First | Drafted in this package |
| Sensor research and selection | Before hardware integration | Provisional recommendation; exact part verification pending |
| Simulated data path and validation | Implementation | Source included; run and record tests |
| ESP32 build and serial demo | Integration | Requires PlatformIO and ESP32; record actual result |
| MQTT demo | Integration | Optional; requires broker and subscriber |
| Tests, handover, and reflection | Before submission | Complete from real results and personal experience |

## Team workflow

Proposed workflow: a Kanban board with Backlog, In Progress, Review, and Done; small commits; peer review; and shared responsibility for explaining the complete data path. Replace this proposal with the team's actual workflow and contribution evidence before submission.

## Risks

| Risk | Mitigation |
|---|---|
| Simulated values mistaken for sensor results | Label simulation clearly; do not claim physical measurement accuracy |
| Condensation affects the indoor sensor | Keep it away from droplets; evaluate a protected variant and enclosure |
| Network/broker unavailable | Keep Serial/host simulation as fallback; test broker before presenting |
| Credentials committed | Keep local secrets ignored; rotate any exposed credentials |
| Probe leaks or corrodes | Evaluate the purchased probe's seals and wetted materials, not only its sensor IC |
| Work is after the stated project window | Be transparent about dates and confirm submission expectations with the instructor |