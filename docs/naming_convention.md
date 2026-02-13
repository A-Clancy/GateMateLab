# GateMate Lab – Entity Reference

This document defines naming conventions and lists all current entities in the GateMate Lab environment.

---

## 1. Global Naming Rules

All GateMate Lab entities use the prefix:

lab_gate_

This prevents namespace collision and clarifies project scope.

Units are included in helper names:
- `_m` for metres
- `_s` for seconds

Binary state uses:
- `input_boolean` for authoritative state
- `binary_sensor` for read-only mirrors

---

## 2. Physical Layer Entities

### BLE Sensor (Shelly BLU Distance)
- sensor.lab_gate_distance_sensor_distance (mm)
- sensor.lab_gate_distance_sensor_battery
- binary_sensor.lab_gate_distance_sensor_vibration

### BLE Proxy (ESPHome)
- Device: gatemate-gate-ble-proxy
- Integration: ESPHome

---

## 3. Logical / State Layer

### Authoritative State
- input_boolean.lab_gate_vehicle_present
- input_boolean.lab_gate_alert_acknowledged

### Read-Only Mirror
- binary_sensor.lab_gate_vehicle_present

### Configuration Helpers
- input_number.lab_gate_distance_min_m
- input_number.lab_gate_distance_max_m
- input_number.lab_gate_dwell_time_s
- input_number.lab_gate_clearance_time_s

---

## 4. Integration Layer

### Automations
- lab_gate_vehicle_detected_after_dwell
- lab_gate_vehicle_cleared_after_clearance
- lab_gate_notify_presence_detected
- lab_gate_notify_presence_cleared
- lab_gate_notification_acknowledge

### Scripts
- script.lab_gate_set_vehicle_present
- script.lab_gate_set_vehicle_clear

---

## 5. Reserved MQTT Topic (Future)

Topic:
gatemate/vehicle

Payload:
- detected
- cleared

No raw distance data will be published.
