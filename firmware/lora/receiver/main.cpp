#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RadioLib.h>
#include "config.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define VEXT_CTRL 36

#define LORA_NSS   8
#define LORA_SCK   9
#define LORA_MOSI 10
#define LORA_MISO 11
#define LORA_RST  12
#define LORA_BUSY 13
#define LORA_DIO1 14

const char* TOPIC_DISTANCE = "gatemate/lab/gate/lora/distance_mm";
const char* TOPIC_BATTERY = "gatemate/lab/gate/lora/battery";
const char* TOPIC_HEALTH = "gatemate/lab/gate/lora/healthy";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
SPIClass radioSPI(FSPI);
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY, radioSPI);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void showMessage(const String& line1, const String& line2 = "", const String& line3 = "") {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(line1);
  if (line2.length() > 0) display.println(line2);
  if (line3.length() > 0) display.println(line3);
  display.display();
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  showMessage("GateMate", "WiFi connecting", "");
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  showMessage("GateMate", "WiFi connected", WiFi.localIP().toString());
}

void connectMQTT() {
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    showMessage("GateMate", "MQTT connecting", MQTT_HOST);

    if (mqttClient.connect("gatemate-lora-rx", MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      mqttClient.publish(TOPIC_HEALTH, "true", true);
      showMessage("GateMate", "MQTT connected", "LoRa RX ready");
    } else {
      Serial.print("failed, rc=");
      Serial.println(mqttClient.state());
      showMessage("MQTT failed", "rc=" + String(mqttClient.state()), "Retrying...");
      delay(2000);
    }
  }
}

bool extractValue(const String& payload, const String& key, String& valueOut) {
  String prefix = key + "=";
  int start = payload.indexOf(prefix);
  if (start == -1) {
    return false;
  }

  start += prefix.length();
  int end = payload.indexOf(",", start);
  if (end == -1) {
    end = payload.length();
  }

  valueOut = payload.substring(start, end);
  valueOut.trim();
  return true;
}

void publishLoRaPayload(const String& payload) {
  String distanceValue;
  String batteryValue;

  bool hasDistance = extractValue(payload, "distance_mm", distanceValue);
  bool hasBattery = extractValue(payload, "battery", batteryValue);

  Serial.print("WiFi status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "connected" : "not connected");

  Serial.print("MQTT status: ");
  Serial.println(mqttClient.connected() ? "connected" : "not connected");

  if (hasDistance) {
    bool ok = mqttClient.publish(TOPIC_DISTANCE, distanceValue.c_str(), true);
    Serial.print("Publish distance_mm ");
    Serial.print(distanceValue);
    Serial.print(" -> ");
    Serial.println(ok ? "OK" : "FAILED");
  }

  if (hasBattery) {
    bool ok = mqttClient.publish(TOPIC_BATTERY, batteryValue.c_str(), true);
    Serial.print("Publish battery ");
    Serial.print(batteryValue);
    Serial.print(" -> ");
    Serial.println(ok ? "OK" : "FAILED");
  }

  bool ok = mqttClient.publish(TOPIC_HEALTH, "true", true);
  Serial.print("Publish healthy -> ");
  Serial.println(ok ? "OK" : "FAILED");
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, LOW);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true) {
      delay(1000);
    }
  }

  showMessage("GateMate", "LoRa RX", "Starting");

  radioSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  int radioState = radio.begin(868.0);
  if (radioState != RADIOLIB_ERR_NONE) {
    showMessage("LoRa init", "FAILED", String(radioState));
    while (true) {
      delay(1000);
    }
  }

  connectWiFi();
  connectMQTT();

  Serial.println("LoRa receiver ready");
  showMessage("GateMate", "LoRa RX ready", "MQTT online");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    connectMQTT();
  }

  mqttClient.loop();

  String str;
  int state = radio.receive(str);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Received: ");
    Serial.println(str);

    publishLoRaPayload(str);

    String distanceValue;
    String batteryValue;
    extractValue(str, "distance_mm", distanceValue);
    extractValue(str, "battery", batteryValue);

    showMessage(
      "LoRa Packet",
      "Dist: " + distanceValue + "mm",
      "Bat: " + batteryValue + "%"
    );
  }
}