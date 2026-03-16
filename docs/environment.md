# GateMate Lab -- Home Assistant Environment Specification

Part of: GateMate Lab Documentation

This document describes the minimal Home Assistant environment required
to reproduce the GateMate Lab instance used for development and
validation.

The goal is reproducibility: another developer should be able to
recreate the working GateMate system with a similar Home Assistant
installation and hardware setup.

Scope: GateMate components only. Unrelated Home Assistant integrations
and add‑ons are intentionally excluded.

------------------------------------------------------------------------

## Platform

GateMate Lab runs on the following base platform:

-   Home Assistant OS
-   Raspberry Pi hardware
-   Supervisor enabled

The system is managed via a Samba share and Git-controlled configuration
to maintain traceability of configuration changes.

------------------------------------------------------------------------

## Required Add-ons

### ESPHome Device Builder

Purpose:

-   Compile and deploy ESPHome firmware
-   Manage the ESP32 Bluetooth proxy used by the BLE transport path

Used for:

-   `gatemate_gate_ble_proxy.yaml`
-   OTA firmware updates

Status: Required

------------------------------------------------------------------------

### Mosquitto Broker

Purpose:

-   Local MQTT broker used for system integration
-   Publishes authoritative vehicle state to external consumers

Primary topic:

    gatemate/lab/gate/vehicle_present

Status: Required

------------------------------------------------------------------------

### Samba Share

Purpose:

-   Access `/config` directory from VS Code
-   Manage configuration files under Git version control

Rationale:

This workflow supports reproducible development and aligns with software
engineering practices expected in an academic project.

Status: Required

------------------------------------------------------------------------

### Advanced SSH & Web Terminal

Purpose:

-   Git CLI operations
-   system maintenance
-   service inspection

Status: Required

------------------------------------------------------------------------

### File Editor

Purpose:

-   quick YAML edits within Home Assistant

Status: Optional (used for convenience during development)

------------------------------------------------------------------------

## Integrations Relevant to GateMate

### ESPHome

Device:

-   GateMate Gate BLE Proxy

Role:

-   Relays BLE advertisements from the distance sensor to Home Assistant
    over Wi‑Fi.

------------------------------------------------------------------------

### Bluetooth

Provides the Bluetooth stack used for:

-   ESP32 BLE Proxy integration
-   local BLE testing during development

------------------------------------------------------------------------

### BTHome

Device:

-   Shelly BLU Distance sensor

Provides:

-   distance telemetry
-   battery status
-   signal strength (RSSI)

------------------------------------------------------------------------

### MQTT

Backed by the Mosquitto broker add‑on.

Purpose:

-   publish authoritative vehicle state
-   enable external integrations such as notifications and physical
    display endpoints
