# GateMate System Overview

Part of: GateMate Lab Documentation

This document provides a high-level overview of the GateMate
architecture.

GateMate is a Home Assistant--based physical computing system designed
to detect vehicle presence at a gate and publish an authoritative
presence state that can be consumed by external systems.

------------------------------------------------------------------------

## Core Architectural Idea

The system is designed around **transport abstraction**.

Distance telemetry may arrive through different transport mechanisms
(BLE proxy or LoRa relay), but Home Assistant consumes only a single set
of **authoritative sensors**.

Detection automations therefore remain unchanged regardless of how the
data reaches Home Assistant.

------------------------------------------------------------------------

## System Layers

### Sensor Layer

The primary sensing device is:

Shelly BLU Distance

The device broadcasts distance telemetry using Bluetooth Low Energy
(BLE).

------------------------------------------------------------------------

### Transport Layer

Two transport paths are supported.

**BLE Transport (baseline)**

Shelly BLU Distance\
→ BLE\
→ ESP32 Bluetooth Proxy\
→ Wi‑Fi\
→ Home Assistant

**LoRa Relay Transport (stretch goal)**

Shelly BLU Distance\
→ BLE\
→ Heltec LoRa Sender\
→ LoRa\
→ Heltec LoRa Receiver\
→ MQTT\
→ Home Assistant

------------------------------------------------------------------------

### Orchestration Layer

Home Assistant performs system orchestration including:

-   vehicle detection logic
-   dwell filtering
-   clearance timing
-   sensor health monitoring
-   notification triggering
-   MQTT publication

Transport‑specific sensors are normalised into authoritative entities
such as:

    sensor.lab_gate_distance_authoritative
    sensor.lab_gate_battery_authoritative

------------------------------------------------------------------------

### External Integration Layer

External systems subscribe to the retained MQTT topic:

    gatemate/lab/gate/vehicle_present

Example consumers:

-   ntfy push notifications
-   Sense HAT display
-   future automation integrations
