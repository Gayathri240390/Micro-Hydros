# Testplan och resultat

## Regel för testunderlag

Ett test markeras som **Godkänt** först när det har körts och resultatet har observerats. Simulering visar inte sensorernas mätnoggrannhet; seriell utdata visar inte att MQTT-leverans har skett; en lokal broker visar inte att den publika brokern fungerar. Varje rad anger var testet kördes.

## Testmiljö (körningar den 14 november 2026)

Linux-byggrigg (Desmond, med AI-stöd): PlatformIO Core 6.2.0, espressif32-plattform 8.0.0, g++ 13.3.0, Python 3.13, paho-mqtt 2.2.0, matplotlib 3.11.2, Mosquitto 2.0.18 (lokal broker). Tester som återstår för teamet på egna datorer och i Wokwi framgår under ”Teamets tester” nedan.

## Så körs testerna

```bash
pio test -e native                    # enhetstester för firmware
python -m pytest gateway              # gatewaytester
pio run -e esp32dev -e wokwi -e esp32dev_real -e native_sim
```

## Resultat

### Enhets- och byggtester

| ID | Vad | Hur | Förväntat resultat | Faktiskt resultat | Status |
|---|---|---|---|---|---|
| T-01 | Rimlighetsvalidering (normala värden, gränser, värden utanför intervallet, NaN, mask per kanal och spik) | `pio test -e native` → test_validator | 6 tester godkända | 6/6 godkända | Godkänt |
| T-02 | Simulerade sensorer: determinism, värden inom profilen under ett dygn, tidsskalning, felinjektion (läsfel, NaN, spik) och offset | test_simulation | 6 tester godkända | 6/6 godkända | Godkänt |
| T-03 | SensorManager via HAL: giltig avläsning, sekvensnummer, läsfel isolerat till en kanal, orimligt värde ger fel men inget larm, gränslarm samt funktion med simulerad HAL och handskrivna testdubblar | test_sensor_manager | 6 tester godkända | 6/6 godkända | Godkänt |
| T-04 | JSON-meddelande: exakt format, `null` för felaktiga kanaler, inget `nan`, värsta fallet ryms och för liten buffert ger 0 | test_payload | 4 tester godkända | 4/4 godkända | Godkänt |
| T-05 | Offlinebuffert: FIFO-ordning, äldsta meddelandet tas bort och räknas vid överfyllnad samt säker trunkering | test_buffer | 3 tester godkända | 3/3 godkända | Godkänt |
| T-06 | Gateway: validering av meddelanden (7 felaktiga indata avvisas, äldre format godtas), sekvenslucka/dubblett/omstart, trend och tid till gräns, mottagaren lagrar/avvisar/upptäcker luckor, tidig varning vid stigande temperatur, ingen varning under ett normalt dygn vid 120x eller under de första brusiga sekunderna samt tidsskala | `python -m pytest gateway` | 19 tester godkända | 19/19 godkända | Godkänt |
| T-07 | Byggning av firmware | `pio run` för esp32dev, wokwi, esp32dev_real, native_sim | Alla lyckas | Alla lyckades. esp32dev: RAM 6.9 %, flash 21.1 %; wokwi (Wi-Fi + MQTT): RAM 17.0 %, flash 68.5 %; esp32dev_real: RAM 7.3 %, flash 22.6 % | Godkänt |

### End-to-end-tester (värddatorbygge av firmwarekärnan)

| ID | Vad | Hur | Förväntat resultat | Faktiskt resultat | Status |
|---|---|---|---|---|---|
| T-08 | Upprepade mätningar, felhantering och lagring | 400 avläsningar, tidsskala 120x, fel injiceras vid avläsning 100, värmehändelse vid 300, tre rader tas bort (148→152), ett felaktigt formaterat meddelande; `program … \| gateway.receiver --stdin` | Alla korrekta avläsningar lagras; felaktiga kanaler lagras som saknade; lucka på 3 upptäcks; felaktigt formaterat meddelande avvisas; systemet fortsätter | 397 lagrade (394 giltiga, 3 med fel: water ×3, humidity ×2), lucka ”3 message(s) lost between seq 148 and 152”, 1 meddelande avvisat, mottagaren fortsatte | Godkänt |
| T-09 | Tidig varning före larm | Samma körning som T-08 | Varning före `air_inside_high` | Varning vid avläsning 302 (”+2.42/h, når 28.0 om cirka 38 min”), första larm vid avläsning 305 (= 30 simulerade minuter senare) | Godkänt |
| T-10 | Inga falska varningar vid normal drift | 2000 avläsningar vardera vid 1x, 10x och 120x, ingen värmehändelse | 0 varningar | 0 / 0 / 0 | Godkänt |
| T-11 | MQTT-leverans och status | Lokal Mosquitto; mottagaren prenumererade på `microhydros/+/measurement`; 60 avläsningar (fel vid 20) publicerades med QoS 1; kvarhållen `online` på statusämnet; ett meddelande som inte var JSON | 60 lagrade, statushändelse, meddelandet som inte är JSON avvisas | 60 lagrade (57 giltiga), status `online` loggad, 1 avvisat (”invalid JSON”) | Godkänt |
| T-12 | Analysrapport och diagram | `python -m gateway.analyze` på T-08-data | Statistik per kanal, fel, larm, insikter, PNG | Rapport och diagram skapades; fel visades som saknade värden, inte 0 | Godkänt |
| T-13 | Firmware i Wokwi publicerar till publik broker | `pio run -e wokwi`, starta Wokwi, mottagare med `--mqtt-host broker.hivemq.com --device mh-grupp6` | Avläsningar kommer var 5:e sekund | Körning 18 november 2026 på Desmonds MacBook: minst 43 avläsningar lagrade i SQLite, en cirka var 10:e sekund i verklig tid (5 sekunder enhetstid vid simulatorns hastighet på cirka 51 %), statushändelser `online`/`offline` loggade, omstart upptäckt (seq 5→0). LED-status kontrollerades inte | Godkänt (LED ej verifierad) |
| T-14 | Felknapp i Wokwi | Tryck på den blå knappen | `faults` listar water/humidity under några avläsningar; därefter återhämtning | Avläsning 29–31: `faults` = water + humidity_inside, värden lagrade som saknade, `valid`=0; från avläsning 32 åter `faults`=[]. Övriga kanaler fortsatte rapportera. Röd LED kontrollerades inte | Godkänt (LED ej verifierad) |
| T-15 | Värmehändelse i Wokwi | Tryck på den orange knappen | Mottagaren varnar före `air_inside_high` | Tidig varning kl. 19:08:09 (”+1.98/h, når 28.0 om cirka 20 min”), `air_inside_high` vid avläsning 29 (28.28 °C), inneluften planar ut vid cirka 33 °C; ytterligare tidig varning för luftfuktighet (−4.30/h). Röd LED kontrollerades inte | Godkänt (LED ej verifierad) |

## Teamets tester

| ID | Vad | Hur | Förväntat resultat | Ansvarig | Status |
|---|---|---|---|---|---|
| T-16 | Avbrott till broker | Koppla den simulerade enheten från brokern och återställ sedan anslutningen | Enheten buffrar upp till 20 meddelanden och skickar dem efter återanslutning; gateway rapporterar en lucka endast om fler har förlorats | Khaled | Godkänt för kontrollerade simulerade frånkopplingar; se begränsningarna nedan |
| T-17 | Långkörning (stabilitet) | Wokwi i minst 30 min | Ingen omstart (`seq` fortsätter öka), inga växande luckor | Khaled | Godkänt lokalt den 19 november; se körningen nedan |
| T-18 | Tester på en andra dator | Kör `pio test -e native` och `pytest` på Windows | Samma resultat som ovan | Gayathri, Khaled | Godkänt efter rättning av testuppsättning; se Windows-körningarna nedan |
| T-19 | Fysiska sensorer | Kräver hårdvara | Avläsningar inom databladets noggrannhet jämfört med referens | — | Utanför projektets omfattning (ingen hårdvara) |

### Lokal verifiering på Windows (19 november 2026)

Dessa observationer är separata från de tidigare körningarna och ersätter inte Desmonds registrerade resultat. De blockerade licensförsöken nedan är historiska; senare Wokwi-körningar efter aktivering redovisas i nästa avsnitt.

#### T-18-körning på Windows (19 november 2026)

- PlatformIO Core 6.1.19: `pio test -e native` kördes med det installerade programmets fullständiga sökväg. `test_alarm` klarade alla 5 fall; `test_buffer`, `test_payload`, `test_sensor_manager`, `test_simulation` och `test_validator` misslyckades i länkningssteget med odefinierade referenser till Unitys `setUp` och `tearDown`. Deras testfall kördes inte. Den fullständiga native-sviten motsvarade därför inte den tidigare lyckade körningen.
- Python 3.14.2 och pytest 9.2.0: `.pio/pytest-env/Scripts/python.exe -m pytest gateway` klarade 19/19 tester. Den isolerade, Git-ignorerade miljön installerades från `requirements.txt`.
- Det ursprungliga kommandot `pio test -e native` var inte tillgängligt via PATH; det första försöket med full sökväg nekades åtkomst till PlatformIOs låskatalog, och testet kördes om med åtkomst till den befintliga installationen. Standardversionen av Python saknade först pytest. Ingen testkällkod ändrades inför denna körning.

**Omkörning efter rättning:** Varje Unity-svit länkas separat. De fem sviterna som misslyckades saknade de nödvändiga definitionerna av `setUp` och `tearDown`; tomma definitioner lades till i dessa testfiler, i linje med `test_alarm`. Med samma PlatformIO-installation på Windows klarade `pio test -e native` 30/30 fall i alla sex sviterna. Den isolerade Python-miljöns `python -m pytest gateway` klarade åter 19/19. Ingen firmwareimplementation ändrades.

| Kontroll | Metod | Förväntat resultat | Faktiskt resultat | Status |
|---|---|---|---|---|
| Byggning av Wokwi-firmware | `pio run -e wokwi`, PlatformIO Core 6.1.19, Espressif 32 6.13.0 | Bygget lyckas | Bygget lyckades; firmware-ELF och binärfil skapades | Godkänt (endast bygge) |
| Larmgränser | `pio test -e native -f test_alarm` på Windows med `test/test_alarm/test_alarm.cpp` | Värden under och över varje gräns utlöser respektive larm; värden exakt på gränserna gör det inte. Felaktiga kanaler undertrycker sina larm; uteluft utlöser inget larm. | 5 testfall godkända, 0 misslyckade | Godkänt |
| T-13–T-17, lokal Wokwi-körning | Starta Wokwi Simulator efter bygget | Kör de begärda simulatortesterna | Wokwi rapporterade att licensen hade gått ut och startade inte. Inga lokala resultat för MQTT, knappar, avbrott eller stabilitet observerades. | Blockerat; ej kört |

Det första försöket att köra den nya larmsviten misslyckades vid länkning eftersom Unitys nödvändiga setup- och teardown-funktioner saknades; dessa lades till. Ett senare PlatformIO-försök stötte på en ofullständig installation medan VS Code-tillägget initierades. Sviten kompilerades och kördes därefter direkt med GCC/Unity (5/5 godkända), och efter att PlatformIO hade initierats klarade även kommandot ovan 5/5. Ett Wokwi-bygge i sig visar inte att en simulering lyckats.

För T-16 innebär ett stopp av endast mottagaren inte att den simulerade enheten kopplas från brokern; testet måste bryta enhetens anslutning till brokern eller nätverket för att pröva firmwarens offlinebuffert. Använd en isolerad testmiljö och bevara tidigare registrerade körresultat.

Efter att licensförnyelse hade rapporterats startades Wokwi på nytt i VS Code-fönstret för Micro-Hydros den 19 november 2026. Tillägget visade fortfarande `Your Wokwi license has expired` och simulatorn startade inte. Den förnyade licensen kunde inte verifieras som aktiv i den VS Code-sessionen. T-13–T-17 förblev därför blockerade i den lokala körningen; inga nya simuleringsresultat erhölls då.

### Wokwi-körning efter aktivering (19 november 2026)

Efter aktiveringen startade simulatorn. En tillfällig byggkonfiguration under den Git-ignorerade katalogen `.pio` ändrade endast enhets-ID till `mh-test-kh-1119`: det offentliga standardämnet tog redan emot meddelanden innan denna simulator startade. Den ursprungliga Git-spårade konfigurationen ändrades inte. Python MQTT-mottagaren på Windows prenumererade på det separata testämnet och lagrade mätningar i `.pio/wokwi-isolated.db`; denna ignorerade byggkatalog är tillfällig och ingår inte i versionshanteringen. Endast simulerade, icke-känsliga värden skickades via den publika brokern. Tiderna nedan är UTC.

| ID | Observerat resultat | Lokalt resultat |
|---|---|---|
| T-13 | ESP32 i Wokwi anslöt via MQTT; den externa mottagaren lagrade alla fyra fälten. Normala avläsningar ökade med 5 000 ms i enhetens drifttid. Den gröna MQTT-lampan syntes lysa. | Godkänt |
| T-14 | Den blå knappen gav fel på water och humidity vid seq 5–6, där båda värdena lagrades som `NULL`; vid seq 7 kvarstod endast vattenfelet. Vid seq 8 återkom giltiga avläsningar. Den röda larmlampan syntes lysa under felet. | Godkänt |
| T-15 | Den orange knappen höjde innertemperaturen. Mottagaren loggade en tidig varning kl. 18:51:51, före det första `air_inside_high`-larmet vid seq 15, 28.04 °C, kl. 18:52:07. Den röda larmlampan syntes lysa. | Godkänt |
| T-16 | Sessionsövertaganden orsakade upprepade offline-/online-händelser utan saknade mätningar. Ett separat kontrollerat frånkopplingstest visade återuppspelning från bufferten och överfyllnad; se detaljerna nedan. | Godkänt för kontrollerade simulerade frånkopplingar |
| T-17 | Samma simulatorkörning översteg 30 minuter både på Wokwi-klockan och i verklig tid. SQLite innehöll 363 mätningar, seq 0–362, från 18:50:48 till 19:22:49 UTC (32 min 1 s); drifttiden ökade från 5 000 till 1 817 987 ms utan minskning, saknade sekvenser eller dubbletter. Körningen inkluderade de avsiktligt framkallade återanslutningarna i T-16. | Godkänt för denna körning |

#### T-16: kontrollerad frånkoppling och överfyllnad

En separat tillfällig firmwarekopia under den ignorerade katalogen `.pio/outage-check` kopplade från MQTT-klienten och hindrade återanslutning under två programmerade intervall i enhetens drifttid, 30–60 s och 100–225 s. Efter varje intervall återupptogs den ordinarie kommunikationslogiken. Den Git-spårade firmwarekällan och konfigurationen ändrades inte. En separat mottagare lagrade underlag i den tillfälliga `.pio/wokwi-outage.db`; mottagaren startade efter seq 0 och registrerade från seq 1.

- Kort frånkoppling: seq 5–10 togs emot i ordning kl. 19:26:06 UTC efter återanslutning, utan sekvenslucka.
- Längre frånkoppling: efter seq 18 kom de 20 behållna meddelandena seq 24–43 i ordning kl. 19:28:55 UTC. Mottagaren rapporterade fem förlorade meddelanden mellan seq 18 och 24, vilket motsvarar de bortkastade äldsta seq 19–23; normal leverans fortsatte från seq 44.

Detta verifierar buffring, återuppspelning i ordning och överfyllnadsbeteendet vid 20 meddelanden i den kontrollerade simuleringen. Den publika brokern stoppades inte, och just denna körning verifierade inte automatiska återförsök eftersom återanslutningen avsiktligt hindrades under testintervallen. Det senare återförsöksscenariot beskrivs nedan. Det tidigare sessionsövertagandet påverkade endast den isolerade testklientens ID.

#### T-16: automatiskt återförsök med en otillgänglig ändpunkt (19 november 2026)

En separat tillfällig Wokwi-firmwarekopia under den ignorerade katalogen `.pio/reconnect-check` använde ett isolerat enhets-ID. Vid 30 sekunders drifttid kopplade testkod uttryckligen från MQTT och ändrade destinationen till en otillgänglig testadress; vid 70 sekunder återställdes den publika brokerns adress. Den ursprungliga återförsökslogiken var aktiv hela tiden. Den Git-spårade firmwarekoden och konfigurationen ändrades inte; den publika brokern och Wi-Fi stoppades inte.

Wokwis seriella utdata visade upprepade `MQTT connect failed, state=-2` under avbrottet, följt av `MQTT connected` när adressen hade återställts. Mottagaren registrerade offline kl. 21:24:07 UTC och online kl. 21:24:47 UTC. Mätningarna seq 6–13, som skapades under avbrottet, kom i ordning kl. 21:24:47–21:24:48 UTC; seq 14–15 följde normalt. SQLite registrerade seq 0–15 utan lucka eller dubblett. Underlaget finns i den tillfälliga, ignorerade `.pio/wokwi-retry.db`.

Resultat: **Godkänt för detta kontrollerade scenario med otillgänglig ändpunkt**. Anslutningen kopplades avsiktligt från och destinationen ändrades av testkod; denna körning visar därför inte i sig att en oväntat bruten TCP-anslutning, ett faktiskt brokerstopp eller ett Wi-Fi-avbrott upptäcks.

#### T-16: sessionsövertagande initierat av brokern (19 november 2026)

Den oförändrade kommunikationskoden kördes i Wokwi med endast ett isolerat enhets-ID ändrat i en tillfällig byggkonfiguration. En andra TCP-klient anslöt till den publika brokern med samma ID, vilket trängde undan den befintliga MQTT-sessionen. Den andra klienten fick en lyckad MQTT CONNACK (`20020000`) kl. 21:36:17 UTC. Ett tidigare försök gav ingen bekräftad CONNACK och räknas inte som testhändelse.

Mottagaren registrerade `offline` kl. 21:36:17 och `online` kl. 21:36:18 UTC; Wokwis seriella utdata visade `INFO: MQTT connected`. Mottagaren lagrade seq 0–15 i ordning, utan luckor eller dubbletter, inklusive seq 10 före och seq 11 efter övertagandet. SQLite-integritetskontrollen lyckades. En lokal granskningskopia finns i `test-evidence-2026-11-19/wokwi-drop.db`, utanför versionshanteringen.

Resultat: **Godkänt för ett av brokern initierat undanträngande av den isolerade MQTT-sessionen**. Frånkopplingen var kort, så denna körning visar inte att någon mätning buffrades under avbrottet. Den simulerar inte heller en nätverks-blackhole, Wi-Fi-förlust eller brokerstopp; de registrerade statustiderna har en sekunds upplösning.

#### Lokal granskning av underlag

De fyra SQLite-databaserna klarade `PRAGMA integrity_check` den 19 november 2026. Lokala granskningskopior, kommunikationskällor endast för test, konfigurationer och en kontrollsummeförteckning finns utanför versionshanteringen i den intilliggande katalogen `test-evidence-2026-11-19`, så att underlaget inte enbart är beroende av den tillfälliga `.pio`-katalogen. Databaserna stöder de registrerade mätningarna och gatewayhändelserna; LED-status och seriella återförsöksmeddelanden observerades under körningarna, men ingen separat skärmbild eller seriell logg sparades. Kommandoutdata för T-18 observerades vid körning men ingen separat logg sparades. Kopiorna är lokala och ingår inte automatiskt i Git.

## Värdenas rimlighet

- De simulerade värdena följer en dygnscykel som styrs av odlingsbelysningen: inneluft 21–26 °C, uteluft 20–22 °C, vatten 19.7–21.3 °C med eftersläpning jämfört med luften samt luftfuktighet 54–70 %RH som rör sig motsatt temperaturen (varmare luft → lägre relativ luftfuktighet). Det är rimligt för en hydroponisk inomhusanläggning, men värdena är **våra antaganden**, inte mätningar.
- Under den simulerade värmehändelsen nådde inneluften 33.1 °C och luftfuktigheten sjönk till 42.9 %RH; larmet utlöstes över 28 °C enligt konstruktionen.
- Värden som en fungerande sensor inte kan producera (NaN, spik på 150 °C, negativ luftfuktighet) avvisas som fel och lagras som saknade, så att de inte förvränger statistik eller utlöser missvisande larm.
- Gränsvärdena (inneluft 18–28 °C, vatten 18–24 °C, RH 40–80 %) är antaganden och måste bekräftas med HydroGreen Fingers.
