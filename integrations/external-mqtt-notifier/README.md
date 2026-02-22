# GateMate External MQTT → ntfy Notifier

## Overview

This component demonstrates transport-layer decoupling within the
GateMate system.

A lightweight Python subscriber runs on a separate Raspberry Pi and:

-   Subscribes to the retained MQTT topic\
    `gatemate/lab/gate/vehicle_present`
-   Receives authoritative presence state updates (`true` / `false`)
-   Forwards detection events to an ntfy topic
-   Delivers push notifications to subscribed smartphones
-   Drives a physical Sense HAT visual display (extension)

The Sense HAT extension is implemented as a separate module
(`display.py`) to preserve separation of concerns. The subscriber
remains responsible for MQTT transport and routing only.

------------------------------------------------------------------------

## Architecture

Home Assistant\
→ Mosquitto (retained MQTT)\
→ External Python subscriber (Raspberry Pi)\
→ ntfy topic\
→ Smartphone push notification

and (in parallel)

→ Sense HAT visual display

------------------------------------------------------------------------

## Requirements

-   Python 3
-   `paho-mqtt`
-   `requests`
-   `sense-hat` (for display extension)
-   MQTT broker credentials
-   ntfy topic subscription on receiving device

------------------------------------------------------------------------

## Installation (Raspberry Pi)

### Copy folder

``` bash
scp -r integrations/external-mqtt-notifier user@<pi-ip>:~
```

### Create environment file

Create `/etc/gatemate-ntfy.env`:

``` bash
MQTT_HOST=<host ip>
MQTT_PORT=1883
MQTT_USER=mqtt_subscriber
MQTT_PASS=your_password
MQTT_TOPIC=gatemate/lab/gate/vehicle_present

NTFY_SERVER=https://ntfy.sh
NTFY_TOPIC=gatemate-lab-alerts
NOTIFY_ON_FALSE=false
```

### Create systemd service

Create `/etc/systemd/system/gatemate-ntfy.service`:

``` ini
[Unit]
Description=GateMate MQTT to ntfy notifier
After=network-online.target
Wants=network-online.target

[Service]
Type=simple
User=<user>
WorkingDirectory=/home/<user>/external-mqtt-notifier
EnvironmentFile=/etc/gatemate-ntfy.env
ExecStart=/usr/bin/python3 /home/<user>/external-mqtt-notifier/subscriber.py
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
```

Enable:

``` bash
sudo systemctl daemon-reload
sudo systemctl enable --now gatemate-ntfy.service
```

------------------------------------------------------------------------

## Restart Behaviour

On Raspberry Pi reboot:

1.  systemd restarts the subscriber service.
2.  Subscriber reconnects to Mosquitto.
3.  Broker re-sends retained `vehicle_present` state.
4.  ntfy and Sense HAT reflect the current authoritative state
    automatically.