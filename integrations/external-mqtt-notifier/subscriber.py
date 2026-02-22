import os
import sys
import time
import requests
import paho.mqtt.client as mqtt

from display import SenseHatDisplay

# TODO: Refactor outputs into modules:
# - notifier.py for ntfy (and any future channels)
# - display.py for Sense HAT
# - widen payload parsing to accept ON/OFF and add optional ack topic support.

MQTT_HOST = os.getenv("MQTT_HOST", "192.168.0.23")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_USER = os.getenv("MQTT_USER", "mqtt_subscriber")
MQTT_PASS = os.getenv("MQTT_PASS")  # required

TOPIC = os.getenv("MQTT_TOPIC", "gatemate/lab/gate/vehicle_present")

NTFY_TOPIC = os.getenv("NTFY_TOPIC")  # required (gatemate-lab-alerts)
NTFY_SERVER = os.getenv("NTFY_SERVER", "https://ntfy.sh")

NOTIFY_ON_FALSE = os.getenv("NOTIFY_ON_FALSE", "false").lower() == "true"

_last_payload = None
_display = SenseHatDisplay()


def ntfy_send(title: str, message: str) -> None:
    url = f"{NTFY_SERVER.rstrip('/')}/{NTFY_TOPIC}"
    headers = {"Title": title}
    resp = requests.post(url, data=message.encode("utf-8"), headers=headers, timeout=10)
    resp.raise_for_status()


def parse_presence(payload: str) -> bool | None:
    p = payload.strip().lower()
    if p in ["true", "on", "1", "present", "yes"]:
        return True
    if p in ["false", "off", "0", "clear", "no"]:
        return False
    return None


def on_connect(client, userdata, flags, rc, properties=None):
    if rc != 0:
        print(f"[mqtt] connect failed rc={rc}", file=sys.stderr)
        return
    print(f"[mqtt] connected to {MQTT_HOST}:{MQTT_PORT}, subscribing to {TOPIC}")
    client.subscribe(TOPIC, qos=0)


def on_message(client, userdata, msg):
    global _last_payload
    payload = msg.payload.decode("utf-8", errors="ignore").strip().lower()

    # Deduplicate identical consecutive payloads
    if payload == _last_payload:
        return
    _last_payload = payload

    ts = time.strftime("%Y-%m-%d %H:%M:%S")
    print(f"[mqtt] {ts} topic={msg.topic} payload={payload}")

    presence = parse_presence(payload)
    if presence is None:
        ntfy_send("GateMate", f"Unexpected payload on {TOPIC}: {payload}")
        return

    # Sense HAT output: matrix shows current state; scroll only on state change.
    _display.update_presence(presence, scroll_on_change=True)

    if presence is True:
        ntfy_send("GateMate", "Vehicle detected at gate.")
    else:
        if NOTIFY_ON_FALSE:
            ntfy_send("GateMate", "Vehicle cleared at gate.")


def main():
    if not MQTT_PASS:
        print("MQTT_PASS env var is required", file=sys.stderr)
        sys.exit(1)
    if not NTFY_TOPIC:
        print("NTFY_TOPIC env var is required", file=sys.stderr)
        sys.exit(1)

    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(MQTT_USER, MQTT_PASS)
    client.on_connect = on_connect
    client.on_message = on_message

    # Auto-reconnect loop
    while True:
        try:
            client.connect(MQTT_HOST, MQTT_PORT, keepalive=60)
            client.loop_forever()
        except Exception as e:
            print(f"[mqtt] error: {e}. Reconnecting in 5s...", file=sys.stderr)
            time.sleep(5)


if __name__ == "__main__":
    main()