# GateMate Lab – Home Assistant Environment Specification

This document defines the minimal Home Assistant environment required to reproduce the GateMate BLE Proxy lab instance.

Scope: GateMate only.  
Non-related integrations and add-ons are intentionally excluded.

---

## 1. Platform

- Home Assistant OS
- Raspberry Pi hardware
- Supervisor mode enabled

---

## 2. Add-ons (Required for GateMate)

### 2.1 ESPHome Device Builder
Purpose:
- Compile and deploy ESPHome firmware
- Manage ESP32 BLE Proxy configuration

Used For:
- `gatemate_gate_ble_proxy.yaml`
- Deterministic firmware deployment
- OTA updates (via static IP)

Status: Required

---

### 2.2 Mosquitto Broker
Purpose:
- Local MQTT broker
- Publish authoritative vehicle state

Used For:
- Topic: `gatemate/lab/gate/vehicle_present`

Status: Required

---

### 2.3 Samba Share
Purpose:
- Access `/config` directory from VS Code
- Git repository management
- Version-controlled configuration

Rationale:
Required for reproducible development workflow and assessment traceability.

Status: Required

---

### 2.4 Advanced SSH & Web Terminal
Purpose:
- Git CLI operations
- Commit management
- Controlled configuration changes

Status: Required

---

### 2.5 File Editor
Purpose:
- YAML configuration edits within HA
- Rapid automation adjustments

Status: Required

---

## 3. Integrations (GateMate Relevant Only)

### 3.1 ESPHome
- Device: GateMate Gate BLE Proxy
- Role: BLE relay only

---

### 3.2 Bluetooth
- Includes:
  - ESP32 BLE Proxy
  - Raspberry Pi internal Bluetooth (lab validation only)

---

### 3.3 BTHome
- Device: Shelly BLU Distance
- Provides:
  - Distance sensor
  - Battery status
  - RSSI

---

### 3.4 MQTT
- Backed by Mosquitto Broker add-on
- Local broker only (no internet exposure)

Authoritative Published Topic:
