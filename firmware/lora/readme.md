# LoRa Relay Firmware

This folder contains the firmware used for the LoRa relay transport path
in GateMate.

The LoRa relay was implemented as a stretch goal to demonstrate that the
system architecture can support alternative transport mechanisms without
modifying the detection logic in Home Assistant.

The relay path operates as follows:

Shelly BLU Distance Sensor\
→ BLE\
→ Heltec LoRa Sender\
→ LoRa\
→ Heltec LoRa Receiver\
→ Wi‑Fi / MQTT\
→ Home Assistant

The sender reads distance data locally and transmits compact telemetry
packets over LoRa.\
The receiver listens for packets and publishes them to the MQTT broker
used by Home Assistant.

Home Assistant then converts the incoming MQTT payloads into the
standard GateMate sensor entities.

## Sender

`sender/` contains firmware for the Heltec LoRa node located near the
gate sensor.

Responsibilities:

-   receive local distance telemetry
-   encode telemetry payload
-   transmit packet over LoRa

## Receiver

`receiver/` contains firmware for the Heltec LoRa node connected to the
network.

Responsibilities:

-   receive LoRa packets
-   decode telemetry
-   publish MQTT message for Home Assistant

### Firmware Installation

Both LoRa nodes are built using PlatformIO and can be flashed directly over USB.

To install the firmware, connect the Heltec board to a development machine via USB and open the corresponding `sender` or `receiver` project in PlatformIO. Update the `config.h` file with the required Wi-Fi and MQTT settings if applicable. The firmware can then be compiled and uploaded using the standard PlatformIO workflow (`Upload` task in VS Code or `pio run --target upload` from the command line).

Once flashed, the sender node begins transmitting telemetry packets over LoRa and the receiver node listens for packets and publishes the decoded payload to the MQTT broker used by Home Assistant.

## Notes

The LoRa relay does not change the detection logic.

Home Assistant selects the active transport source using:

`input_select.lab_gate_transport_mode`

Automations always reference the authoritative sensors:

    sensor.lab_gate_distance_authoritative
    sensor.lab_gate_battery_authoritative

This demonstrates the transport abstraction approach used in the
GateMate architecture.
