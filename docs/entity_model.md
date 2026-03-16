# GateMate Lab -- Entity Naming Conventions

Part of: GateMate Lab Documentation

This document defines the naming conventions used for entities in the
GateMate Lab environment and lists the key entities used by the system.

Clear and consistent naming is important in Home Assistant projects
where many entities share a global namespace.

------------------------------------------------------------------------

## Global Naming Rule

All GateMate entities use the prefix:

    lab_gate_

Example:

    sensor.lab_gate_distance_authoritative

This makes project entities easy to identify and prevents collisions
with unrelated Home Assistant components.

------------------------------------------------------------------------

## Units in Helper Names

Configuration helpers include units in their names.

Examples:

-   `_m` → metres
-   `_s` → seconds

Example:

    input_number.lab_gate_distance_min_m

------------------------------------------------------------------------

## State Entities

### Authoritative State

The authoritative vehicle presence state is stored as:

    input_boolean.lab_gate_vehicle_present

This entity represents the system's final decision after dwell logic and
filtering.

------------------------------------------------------------------------

### Read‑Only Mirror

A read‑only mirror is provided for UI or integration use:

    binary_sensor.lab_gate_vehicle_present

This prevents automations from accidentally modifying the authoritative
state.

------------------------------------------------------------------------

## Physical Layer Entities

### Distance Sensor (Shelly BLU Distance)

    sensor.lab_gate_distance_sensor_distance
    sensor.lab_gate_distance_sensor_battery
    binary_sensor.lab_gate_distance_sensor_vibration

These values originate from the physical BLE sensor.

------------------------------------------------------------------------

### BLE Proxy (ESPHome)

Device:

    gatemate-gate-ble-proxy

Role:

Relays BLE advertisements to Home Assistant over Wi‑Fi.

------------------------------------------------------------------------

## Configuration Helpers

System behaviour is controlled through Home Assistant helpers:

    input_number.lab_gate_distance_min_m
    input_number.lab_gate_distance_max_m
    input_number.lab_gate_dwell_time_s
    input_number.lab_gate_clearance_time_s

These allow system tuning without modifying automation code.

------------------------------------------------------------------------

## Automation Layer

Key automations include:

    lab_gate_vehicle_detected_after_dwell
    lab_gate_vehicle_cleared_after_clearance
    lab_gate_notify_presence_detected
    lab_gate_notify_presence_cleared

Automations operate only on authoritative entities and not on
transport‑specific sensors.

------------------------------------------------------------------------

## MQTT Topics

All external integrations consume the same retained MQTT topic:

    gatemate/lab/gate/vehicle_present

Payload:

    true | false

Optional topics (reserved for future extensions):

    gatemate/lab/gate/event
    gatemate/lab/gate/last_change
