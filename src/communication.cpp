#include <Arduino.h>

#include "communication.h"

#if defined(ENABLE_MQTT)
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

namespace {
WiFiClient networkClient;
PubSubClient mqttClient(networkClient);
constexpr char mqttTopic[] = "microhydros/measurement";
constexpr std::uint32_t mqttRetryIntervalMs = 5000;
std::uint32_t lastMqttAttemptMs = 0;
}
#endif

namespace {
String toJson(const MeasurementData& measurement) {
    String json;
    json.reserve(160);
    json += "{\"timestamp_ms\":";
    json += String(measurement.timestampMs);
    json += ",\"air_inside_c\":";
    json += String(measurement.airInsideC, 2);
    json += ",\"air_outside_c\":";
    json += String(measurement.airOutsideC, 2);
    json += ",\"water_c\":";
    json += String(measurement.waterC, 2);
    json += ",\"humidity_inside_pct\":";
    json += String(measurement.humidityInsidePct, 2);
    json += ",\"valid\":";
    json += measurement.valid ? "true" : "false";
    json += "}";
    return json;
}
}

void Communication::begin() {
#if defined(ENABLE_MQTT)
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif
}

void Communication::loop() {
#if defined(ENABLE_MQTT)
    const std::uint32_t now = millis();
    if (WiFi.status() == WL_CONNECTED && !mqttClient.connected() &&
        now - lastMqttAttemptMs >= mqttRetryIntervalMs) {
        lastMqttAttemptMs = now;
        mqttClient.connect("microhydros-esp32");
    }
    if (mqttClient.connected()) {
        mqttClient.loop();
    }
#endif
}

void Communication::send(const MeasurementData& measurement) {
    const String payload = toJson(measurement);
    Serial.println(payload);

#if defined(ENABLE_MQTT)
    if (mqttClient.connected() && !mqttClient.publish(mqttTopic, payload.c_str())) {
        Serial.println("WARNING: MQTT publish failed");
    }
#endif
}