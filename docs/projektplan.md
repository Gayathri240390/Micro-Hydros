# Projektplan: MicroHydros – version 2
Simulerad IoT-prototyp med HAL för HydroGreen Fingers AB

**Kurs:** Projekt 1 (IOT25M_PRO1), IoT- och embeddedutvecklare, JENSEN YH  
**Beställare (kursens case):** HydroGreen Fingers AB, kontakt Sebastian Green Öhman  
**Grupp:** Desmond Kobba (Scrum Master), Gayathri Jayaprakash, Khaled Khalosi  
**Repo:** github.com/Gayathri240390/Micro-Hydros  
**Version 2:** 2026-11-14 (ersätter v1 från 2026-11-10)  
**Prototyp klar (internt mål):** 2026-11-20 · **Deadline:** 2026-11-27 (bekräftad av Sebastian 2026-11-11)

### Ändringar mot version 1
- Bygger nu på Gayathris befintliga repo (PlatformIO, datamodell, validering) i stället för att börja om.
- Sebastians krav inlagda: simulering är godkänd **om** det finns ett HAL; redovisning med inspelad demovideo för kund.
- Khaleds granskning (8 punkter) inarbetad, se avsnitt 13.
- Individuell reflektionsrapport tillagd som leverabel (uppgiftens punkt 10.6).
- Sensorval anpassat till Gayathris utredning: SHT40 i stället för SHT31, med källor.

## 1. Sammanfattning av uppdraget
HydroGreen Fingers vill ha en första teknisk prototyp av MicroHydros, ett litet hydroponiskt odlingsskåp. Prototypen ska mäta lufttemperatur inne och ute, vattentemperatur och luftfuktighet inne, återkommande över tid. Den ska skicka mätdata till ett externt system och hantera felaktiga värden. Kunden vill också att systemet byggs med tanke på historiska data och framtida varningar. DHT11/DHT22 får inte användas. Kunden värderar en liten, stabil och välmotiverad lösning högre än många halvfärdiga funktioner.

## 2. Mål
1. Fyra mätpunkter läses var 5:e sekund via ett HAL (simulerade sensorer).
2. Varje mätvärde valideras. Fel isoleras per mätpunkt och stoppar inte systemet.
3. Mätdata skickas som JSON via MQTT till en mottagare som sparar historik.
4. Larm vid gränsvärden och tidig varning när en trend är på väg mot en gräns.
5. Dokumentation och tester som gör att en annan utvecklare kan förstå och bygga vidare.
6. Demovideo för kund och individuella reflektionsrapporter.

## 3. Avgränsningar
- **Allt är simulerat.** Gruppen har varken komponenter eller budget. ESP32 körs i simulatorn Wokwi och sensorvärdena genereras i mjukvara. Drivrutiner för riktiga sensorer finns och kompilerar men testas inte mot hårdvara.
- Ingen molnplattform, app, webbgränssnitt eller styrning av pump och belysning.
- Publik MQTT-broker utan kryptering i demon, eftersom datan är simulerad och ofarlig.

## 4. Krav
Fullständig lista med verifiering och status: `docs/02-requirements-backlog.md`. Kort:

| Typ | Krav |
|---|---|
| Måste | Fyra mätpunkter, återkommande mätning, behandling i embedded-system, kommunikation till externt system, felhantering, ingen DHT, HAL, dokumentation |
| Bör | Historik i databas, separerade moduler, automatisk återanslutning, inga hemligheter i Git |
| Kan | Tidig varning via trend, analysrapport med graf |

## 5. Teknisk lösning (kort)
- **ESP32-firmware (C++, PlatformIO):** HAL-gränssnitt (`ITemperatureSensor`, `IHumiditySensor`), simulerade sensorer med dygnscykel, brus och felinjicering, `SensorManager`, `Validator`, `AlarmEvaluator`, JSON, MQTT med buffert och Last Will, lysdioder och simuleringsknappar.
- **Gateway (Python):** tar emot via MQTT, validerar, sparar i SQLite, upptäcker tappade meddelanden och ger tidig varning. Analysen ger rapport och graf.
- **Sensorval för framtida hårdvara:** SHT40 (luft och fukt inne), DS18B20 (vatten och luft ute). Motivering och källor finns i `docs/04-sensor-decisions.md`.
- **Arkitekturdiagram:** `docs/03-architecture.md`.

## 6. Prioriterad backlog
| Prio | Uppgift | Ansvarig | Status 14/11 |
|---|---|---|---|
| 1 | HAL + simulerade sensorer med fel | Desmond | Klar (PR) |
| 2 | Validering, larm, JSON, sekvensnummer | Gayathri (bas), Desmond (utökning) | Klar (PR) |
| 3 | MQTT med återanslutning och buffert | Desmond | Klar (PR) |
| 4 | Gateway: ta emot, validera, spara | Desmond | Klar (PR) |
| 5 | Grunddokumentation (README, arkitektur) | Desmond, Gayathri | Klar, granskas |
| 6 | Granska och merga PR | Gayathri | Öppen |
| 7 | Tester i Wokwi och testprotokoll (T-13–T-18) | Khaled | Öppen |
| 8 | Gränsvärdestester för larm (`test/test_alarm`) | Khaled | Öppen |
| 9 | Analysrapport och graf | Desmond | Klar (PR) |
| 10 | Manus och inspelning av demovideo | Alla | Öppen |
| 11 | Individuell reflektionsrapport | Var och en | Öppen |

MVP är prio 1–5 och gjordes först. Analysen (9) lades till efter att kärnflödet fungerade.

**Lokal uppdatering 19/11:** Khaled har genomfört T-13–T-17 i Wokwi och T-18 på Windows samt uppdaterat testprotokollet. Larmgränstesterna klarade 5/5. T-16 är godkänt för de kontrollerade scenarier som beskrivs i `docs/05-test-report.md`, med angivna begränsningar. Gayathris separata T-18-körning har inte verifierats. Tabellen ovan visar fortsatt statusen den 14/11.

## 7. Tidsplan
| Period | Innehåll | Avslut |
|---|---|---|
| Sprint 1: 10–16 nov | Plan v1/v2, HAL, firmware, MQTT, gateway, tester, dokumentation, PR | Sprintgranskning + retro mån 16/11 (tid bestäms i Discord) |
| Sprint 2: 17–20 nov | Merge, Wokwi-tester, testprotokoll, larmtester, demomanus, inspelning | Demovideo klar fre 20/11 (internt mål) |
| Buffert: 23–27 nov | Rättningar efter feedback, reflektionsrapporter, slutinlämning | Deadline fre 27/11 |

## 8. Arbetsfördelning
Uppgiften kräver att alla deltar i det tekniska arbetet. Därför har var och en en egen teknisk uppgift som syns i Git.

| Person | Roll | Teknisk uppgift | Övrigt |
|---|---|---|---|
| Desmond | Scrum Master | HAL, simulering, MQTT, gateway, analys, integration | Backlog, möten, sprintlogg, demomanus |
| Gayathri | Repoägare, kodgranskare | Grundfirmware (datamodell, validering), sensorutredning, granskning och merge av PR, körning på egen dator (T-18) | Del av demon |
| Khaled | Testansvarig | Wokwi-tester T-13–T-17, egna enhetstester för larmgränser (`test/test_alarm`), testprotokoll | Del av demon |

Arbetsflöde: feature-branch, pull request till Gayathris repo, granskning av en annan medlem och sedan merge. Daglig avstämning i Discord, fem minuter eller en kort text (igår, idag, hinder).

## 9. Risker
| Risk | Sannolikhet | Konsekvens | Åtgärd |
|---|---|---|---|
| Gruppmedlem otillgänglig | Hög | Hög | Små, tydliga uppgifter. Asynkront via Discord och GitHub. Scrum Master tar över kritiska uppgifter, men bara efter avstämning. |
| Wokwi når inte den publika brokern | Låg | Hög | Testas först i sprint 2 (T-13). Reservbroker: test.mosquitto.org. Seriell JSON fungerar alltid. En ren Python-simulering används inte som ersättning för ESP32, eftersom den inte uppfyller embedded-kravet. |
| En medlems bidrag kan inte styrkas | Medel | Hög | Egna commits, granskningar och sprintlogg |
| Tiden räcker inte | Medel | Hög | MVP först, buffertvecka till 27/11 |
| Simulering tas för verkliga mätningar | Låg | Medel | Tydligt märkt i kod, README och demo |
| Publik broker utan kryptering | Hög | Låg | Endast simulerad data. Ingen styrning av enheten. Produktionsrekommendation dokumenterad. |

## 10. Testplan
Vad som testas, hur det görs, förväntat och faktiskt resultat finns i `docs/05-test-report.md`. Kort:
- 25 enhetstester för firmware och 19 för gateway. Alla gick igenom den 14/11.
- Ett helt flöde har körts på PC: värdkörning av firmwarekärnan, MQTT, mottagare, SQLite och analys. Det täcker fel, tappade meddelanden och tidig varning.
- Lokal uppdatering 19/11: Wokwi-tester, kontrollerade anslutningsavbrott och långkörning samt Khaleds Windows-körning har genomförts. T-16:s avgränsningar och resultaten finns i `docs/05-test-report.md`; Gayathris separata T-18-körning har inte verifierats.

## 11. Förlorad data (Khaled punkt 2)
Enheten buffrar upp till 20 mätningar i RAM när anslutningen är nere och skickar dem i ordning när den kommer tillbaka. Vid längre avbrott tappas de äldsta, och det räknas. Mottagaren upptäcker luckor via sekvensnummer och omstarter via att numret börjar om. Inget sparas över en omstart, vilket är en dokumenterad begränsning. Tidsstämpeln är drifttid; mottagaren lägger till UTC-tid när datan kommer in.

## 12. Leverabler
1. Projektplan (det här dokumentet) och kravlista
2. Git-repo med historik, branches och pull request
3. Fungerande prototyp (Wokwi + gateway)
4. Teknisk dokumentation (`docs/03`, `docs/04`, `docs/07`)
5. Testdokumentation (`docs/05`)
6. Demovideo för kund
7. Individuell reflektionsrapport från varje medlem, skriven självständigt

## 13. Khaleds granskning – så hanteras punkterna
| # | Punkt | Hantering |
|---|---|---|
| 1 | HAL och simulering | Gränssnitt, enheter och felrepresentation beskrivna i `docs/03`. Test visar att `SensorManager` fungerar med olika implementationer. |
| 2 | Förlorad data | Buffert, sekvensnummer och Last Will (avsnitt 11) |
| 3 | Wokwi och MQTT | ESP32 i Wokwi är huvudspåret. Värdkörningen används bara för tester. |
| 4 | Prioritering | MVP först (avsnitt 6) |
| 5 | Tester | Förväntat och faktiskt resultat per test, även för det som återstår |
| 6 | Sensorval | SHT40 och DS18B20 med databladskällor. Tydligt att de är förslag och att värdena är simulerade. |
| 7 | Arbetsfördelning | Egen teknisk uppgift per person, individuell reflektion (avsnitt 8, 12) |
| 8 | Tidsplan och säkerhet | Mål 20/11, deadline 27/11. Riskerna med den publika brokern dokumenterade. |
