# GateMate Sense HAT Display Extension

## Purpose

This module extends the external MQTT subscriber with a physical visual
endpoint using the Raspberry Pi Sense HAT.

It consumes the same retained MQTT topic documented in the External MQTT
→ ntfy README:

`gatemate/lab/gate/vehicle_present`

No changes are made to Home Assistant or MQTT topic structure.

------------------------------------------------------------------------

## Behaviour

When a message is received:

-   `true` → LED matrix turns **red** and scrolls `PRESENT`
-   `false` → LED matrix turns **green** and scrolls `CLEAR`

Scrolling occurs only on state change.\

------------------------------------------------------------------------

## Architecture

The display logic is isolated in:

    display.py

The MQTT transport logic remains in:

    subscriber.py

------------------------------------------------------------------------

## Restart & State Recovery

Because the MQTT topic is retained:

-   After Raspberry Pi restart,
-   Subscriber reconnects,
-   Broker re-sends last state,
-   Display updates automatically.

No additional state persistence is required on the Pi.

------------------------------------------------------------------------

## Future Extensions (Planned)

-   Amber matrix state for `alert_acknowledged`
-   Audible alert via Sense HAT buzzer (optional)
-   Modular notifier refactor (`notifier.py`)
