# GateMate Lab

GateMate Lab is the development and validation environment for
**GateMate**, a Home Assistant--based physical computing system for
detecting vehicle presence at a gate.

The system uses a distance sensor and configurable detection logic to
determine when a vehicle is present and trigger notifications and
automations.

This repository contains the working implementation used to develop and
validate the GateMate architecture.

------------------------------------------------------------------------

## Project Overview

GateMate detects vehicle presence using distance measurements and dwell
logic implemented in Home Assistant.

The design emphasises:

-   modular configuration
-   separation of concerns
-   transport abstraction
-   practical reliability for real-world deployment

Two transport paths are supported:

### BLE Transport (baseline)

Shelly BLU Distance sensor\
→ BLE\
→ ESP32 Bluetooth Proxy\
→ Wi-Fi\
→ Home Assistant

### LoRa Transport (experimental / stretch goal)

Shelly BLU Distance sensor\
→ BLE\
→ Heltec LoRa sender\
→ LoRa\
→ Heltec LoRa receiver\
→ MQTT\
→ Home Assistant

Detection logic inside Home Assistant operates only on **authoritative
sensors**, allowing the transport mechanism to change without modifying
automation logic.

------------------------------------------------------------------------

## Repository Structure

    config/
    ├── automations/        Gate detection, notifications, fail-safe logic
    ├── scripts/            Reusable notification and state scripts
    ├── templates/          Derived and authoritative sensors
    ├── helpers/            Input helpers used for configuration and state
    ├── ui/                 Diagnostics dashboard
    ├── esphome/            ESP32 Bluetooth proxy configuration
    ├── firmware/           LoRa sender and receiver firmware
    ├── integrations/       External integrations (MQTT notifier, display)
    ├── docs/               Project documentation and environment notes

Home Assistant acts as the **system orchestration layer**.

External components (LoRa relay, MQTT integrations, display endpoints)
are kept separate from the core detection logic.

------------------------------------------------------------------------

## Key Concepts

### Authoritative Sensors

Transport-specific sensors are normalised into authoritative sensors,
for example:

    sensor.lab_gate_distance_authoritative
    sensor.lab_gate_battery_authoritative

Automations reference only these entities.

### Transport Switching

Transport is selected using:

    input_select.lab_gate_transport_mode

Available modes:

    BLE
    LoRa

This allows the underlying communication mechanism to change without
modifying detection automations.

### Vehicle Detection Logic

Vehicle presence is determined using:

-   configurable distance band
-   dwell time
-   clearance time
-   sensor health monitoring
-   fail-safe clearing on sensor dropout

All configuration values are exposed through Home Assistant helpers.

------------------------------------------------------------------------

## Diagnostics

A diagnostics dashboard exposes:

-   BLE transport values
-   LoRa transport values
-   authoritative sensors
-   detection thresholds
-   vehicle presence state

This interface is used for validation and testing.

------------------------------------------------------------------------
