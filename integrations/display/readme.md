# GateMate Sense HAT Display Extension

## Purpose

This module adds a simple physical display endpoint to the GateMate
external MQTT subscriber using the Raspberry Pi Sense HAT.

It consumes the retained MQTT topic used throughout the system:

`gatemate/lab/gate/vehicle_present`

No changes are required in Home Assistant or the MQTT topic structure.
The display simply reflects the authoritative vehicle presence state
already published by the system.

------------------------------------------------------------------------

## Behaviour

When a message is received:

-   `true` → LED matrix turns **red** and scrolls `PRESENT`
-   `false` → LED matrix turns **green** and scrolls `CLEAR`

Scrolling occurs **only when the state changes** to avoid unnecessary
updates.

------------------------------------------------------------------------

## Architecture

The display functionality is intentionally isolated from the MQTT
transport layer.

Display logic:

    display.py

MQTT subscription and routing:

    subscriber.py

This separation keeps the subscriber focused on message transport while
allowing additional endpoints (display, notifications, etc.) to be added
independently.

------------------------------------------------------------------------

## Restart and State Recovery

The MQTT topic is retained by the broker. This means:

1.  The Raspberry Pi can restart safely.
2.  The subscriber reconnects to the broker.
3.  The broker immediately re-sends the last retained state.
4.  The display updates automatically.

No additional state storage is required on the Raspberry Pi.

------------------------------------------------------------------------

## Possible Extensions

Potential future enhancements include:

-   Amber display state for `alert_acknowledged`
-   Optional audible alerts via the Sense HAT
-   Refactoring display and notification endpoints into modular notifier
    components
