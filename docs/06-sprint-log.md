# Sprint Log

Only events that actually happened are listed, with their real dates. The project ran late; this is a
re-take after the course period (31 Aug – 25 Sep 2026).

| Date | Event | Source |
|---|---|---|
| 2026-09-04 | Gayathri creates the repository (first commit) | Git history |
| 2026-09-07 | Group 6 formed by the teacher (Desmond, Gayathri, Lucas, Simon) | E-mail from Sebastian |
| 2026-10-23 | Gayathri uploads firmware baseline, docs and Python demo | Git history |
| 2026-10-26 | Sebastian: examination not passed; second chance, all parts (planning, sprints, presentation) must be redone as a team | E-mail |
| 2026-11-10 | Kick-off call Desmond + Khaled (Discord, ~8 min): simulated sensors, GitHub, Desmond Scrum Master; project plan v1 sent | Discord, e-mail |
| 2026-11-11 | Sebastian approves simulation **if there is a HAL**; asks for a complete, demonstrable prototype; demo as recorded customer video; deadline 27 Nov | E-mail |
| 2026-11-11 | Khaled reviews plan v1 (8 points: HAL, lost data, Wokwi/MQTT, priorities, tests, sensor sources, work split, schedule/security); finds the individual reflection requirement | E-mail, Discord |
| 2026-11-12 | Gayathri shares her repository and proposes the split: Khaled testing, Desmond simulation, both via pull requests to her repo | Discord |
| 2026-11-11 – 14 | Meeting proposals: 11 Nov declined by Khaled (busy), 13 and 14 Nov not answered yet; Desmond continues on the agreed simulation task | Discord |
| 2026-11-14 | Branch `feature/hal-mqtt-gateway`: HAL, MQTT, Wokwi, gateway, tests, docs (this pull request) | Git history |

## Sprint 1 (10 – 16 Nov): plan and core

Goal: approved plan, HAL-based firmware, data path to an external system.
Done: plan v1 and v2, HAL and simulation, MQTT with buffering, gateway with history and warnings,
44 automated tests (25 firmware, 19 gateway), documentation. Open: review and merge by Gayathri, test runs by Khaled.

## Sprint 2 (17 – 20 Nov): verify and demonstrate

Goal: Wokwi tests T-13 – T-18, update the test report with real results, demo script and recording.

Local testing update, 19 Nov: Khaled ran T-13–T-17 in Wokwi and T-18 on Windows, added alarm boundary tests and recorded the results in `docs/05-test-report.md`. The Windows rerun passed 30/30 firmware and 19/19 gateway tests after fixing the Unity test setup. T-16 passed the controlled scenarios described in the report; their limitations remain. Gayathri's separate T-18 run and the video recording are not verified by these results.

## Buffer (23 – 27 Nov)

Fixes after feedback, individual reflection reports, final hand-in.

## Retrospective notes (to fill in at each sprint review)

- What went well:
- What did not:
- What we change:
