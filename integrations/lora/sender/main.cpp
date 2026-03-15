#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RadioLib.h>

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

const String TARGET_SERVICE_UUID = "0000fcd2-0000-1000-8000-00805f9b34fb";

BLEScan* pBLEScan;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
SPIClass radioSPI(FSPI);
SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY, radioSPI);

uint16_t lastPacketId = 65535;

void showMessage(const String& line1, const String& line2 = "", const String& line3 = "") {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(line1);
  if (line2.length() > 0) {
    display.println(line2);
  }
  if (line3.length() > 0) {
    display.println(line3);
  }
  display.display();
}

uint16_t readLE16(const std::string& data, size_t index) {
  return (uint8_t)data[index] | ((uint8_t)data[index + 1] << 8);
}

bool decodeBTHome(const std::string& data, uint16_t& packetId, int& battery, int& distanceMm) {
  packetId = 0;
  battery = -1;
  distanceMm = -1;

  if (data.length() < 3) {
    return false;
  }

  uint8_t deviceInfo = (uint8_t)data[0];

  // Expect BTHome v2 style payload, as seen from the Shelly BLU Distance.
  if ((deviceInfo & 0xE0) != 0x40) {
    return false;
  }

  size_t i = 1;
  while (i < data.length()) {
    uint8_t objId = (uint8_t)data[i++];

    switch (objId) {
      case 0x00: { // packet id, uint8
        if (i + 1 > data.length()) return false;
        packetId = (uint8_t)data[i++];
        break;
      }

      case 0x01: { // battery, uint8 %
        if (i + 1 > data.length()) return false;
        battery = (uint8_t)data[i++];
        break;
      }

      case 0x2C: { // vibration / boolean-like field in observed payload
        if (i + 1 > data.length()) return false;
        i += 1;
        break;
      }

      case 0x40: { // distance mm, uint16 little-endian
        if (i + 2 > data.length()) return false;
        distanceMm = readLE16(data, i);
        i += 2;
        break;
      }

      default: {
        // Stop on unknown object for now.
        return true;
      }
    }
  }

  return true;
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

  showMessage("GateMate", "BLE -> LoRa", "Starting");

  radioSPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  int loraState = radio.begin(868.0);

  if (loraState != RADIOLIB_ERR_NONE) {
    showMessage("LoRa init", "FAILED", String(loraState));
    while (true) {
      delay(1000);
    }
  }

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  Serial.println("BLE -> LoRa bridge ready");
  showMessage("GateMate", "BLE -> LoRa", "Ready");
}

void loop() {
  BLEScanResults results = pBLEScan->start(5, false);

  for (int j = 0; j < results.getCount(); j++) {
    BLEAdvertisedDevice device = results.getDevice(j);

    if (!device.haveServiceData()) {
      continue;
    }

    int count = device.getServiceDataCount();

    for (int k = 0; k < count; k++) {
      String uuid = device.getServiceDataUUID(k).toString().c_str();

      if (uuid != TARGET_SERVICE_UUID) {
        continue;
      }

      std::string serviceData = device.getServiceData(k);

      uint16_t packetId = 0;
      int battery = -1;
      int distanceMm = -1;

      bool ok = decodeBTHome(serviceData, packetId, battery, distanceMm);

      if (!ok || distanceMm < 0 || battery < 0) {
        continue;
      }

      if (packetId == lastPacketId) {
        continue;
      }

      lastPacketId = packetId;

      Serial.println("Shelly packet decoded");
      Serial.print("Packet ID: ");
      Serial.println(packetId);
      Serial.print("Battery: ");
      Serial.println(battery);
      Serial.print("Distance mm: ");
      Serial.println(distanceMm);

      showMessage(
        "Shelly Seen",
        "Dist: " + String(distanceMm) + "mm",
        "Bat: " + String(battery) + "%"
      );

      String payload = "distance_mm=" + String(distanceMm) +
                       ",battery=" + String(battery) +
                       ",packet_id=" + String(packetId);

      Serial.print("LoRa payload: ");
      Serial.println(payload);

      int txState = radio.transmit(payload);

      if (txState == RADIOLIB_ERR_NONE) {
        Serial.println("LoRa TX OK");
        showMessage(
          "GateMate Bridge",
          "Dist: " + String(distanceMm) + "mm",
          "Bat: " + String(battery) + "%"
        );
      } else {
        Serial.print("LoRa TX failed: ");
        Serial.println(txState);
        showMessage(
          "TX FAILED",
          "Err: " + String(txState),
          ""
        );
      }
    }
  }

  delay(1000);
}