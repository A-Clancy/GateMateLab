# GateMate External MQTT → ntfy Notifier

## Overview

This component demonstrates transport-layer decoupling within the
GateMate architecture.

A lightweight Python subscriber runs on a separate Raspberry Pi and
performs the following tasks:

-   Subscribes to the retained MQTT topic\
    `gatemate/lab/gate/vehicle_present`
-   Receives authoritative presence updates (`true` / `false`)
-   Forwards events to an `ntfy` topic
-   Delivers push notifications to subscribed smartphones
-   Optionally drives a physical Sense HAT display endpoint

The Sense HAT display logic is implemented separately in `display.py` so
the subscriber remains responsible only for MQTT transport and routing.

------------------------------------------------------------------------

## Architecture

    Home Assistant
          ↓
    Mosquitto (retained MQTT)
          ↓
    External Python subscriber (Raspberry Pi)
          ↓
    ntfy topic
          ↓
    Smartphone push notification

In parallel, the same subscriber can also drive a **Sense HAT visual
display**.

------------------------------------------------------------------------

## Requirements

-   Python 3
-   `paho-mqtt`
-   `requests`
-   `sense-hat` (only required if using the display extension)
-   MQTT broker credentials
-   ntfy topic subscription on receiving device

------------------------------------------------------------------------

## Installation (Raspberry Pi)

### Copy folder

    scp -r integrations/external-mqtt-notifier user@<pi-ip>:~

### Create environment file

Create:

    /etc/gatemate-ntfy.env

Example:

    MQTT_HOST=<host ip>
    MQTT_PORT=1883
    MQTT_USER=mqtt_subscriber
    MQTT_PASS=your_password
    MQTT_TOPIC=gatemate/lab/gate/vehicle_present

    NTFY_SERVER=https://ntfy.sh
    NTFY_TOPIC=gatemate-lab-alerts
    NOTIFY_ON_FALSE=false

### Create systemd service

Create:

    /etc/systemd/system/gatemate-ntfy.service

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

Enable the service:

    sudo systemctl daemon-reload
    sudo systemctl enable --now gatemate-ntfy.service

------------------------------------------------------------------------

## Restart Behaviour

After a Raspberry Pi reboot:

1.  `systemd` restarts the subscriber service.
2.  The subscriber reconnects to the MQTT broker.
3.  The broker re-sends the retained `vehicle_present` state.
4.  ntfy notifications and the Sense HAT display reflect the current
    system state automatically.

This behaviour ensures the external notification layer remains
consistent with the authoritative Home Assistant state.
