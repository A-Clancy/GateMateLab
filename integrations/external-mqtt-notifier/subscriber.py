import os
import sys
import time
import requests
import paho.mqtt.client as mqtt

MQTT_HOST = os.getenv("MQTT_HOST", "192.168.0.23")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_USER = os.getenv("MQTT_USER", "mqtt_subscriber")
MQTT_PASS = os.getenv("MQTT_PASS")  # required

TOPIC = os.getenv("MQTT_TOPIC", "gatemate/lab/gate/vehicle_present")

NTFY_TOPIC = os.getenv("NTFY_TOPIC")  # required (gatemate-lab-alerts)
NTFY_SERVER = os.getenv("NTFY_SERVER", "https://ntfy.sh")

NOTIFY_ON_FALSE = os.getenv("NOTIFY_ON_FALSE", "false").lower() == "true"

_last_payload = None


def ntfy_send(title: str, message: str) -> None:
    url = f"{NTFY_SERVER.rstrip('/')}/{NTFY_TOPIC}"
    headers = {"Title": title}
    resp = requests.post(url, data=message.encode("utf-8"), headers=headers, timeout=10)
    resp.raise_for_status()


def on_connect(client, userdata, flags, rc, properties=None):
    if rc != 0:
        print(f"[mqtt] connect failed rc={rc}", file=sys.stderr)
        return
    print(f"[mqtt] connected to {MQTT_HOST}:{MQTT_PORT}, subscribing to {TOPIC}")
    client.subscribe(TOPIC, qos=0)


def on_message(client, userdata, msg):
    global _last_payload
    payload = msg.payload.decode("utf-8").strip().lower()

    # Deduplicate identical consecutive payloads
    if payload == _last_payload:
        return
    _last_payload = payload

    ts = time.strftime("%Y-%m-%d %H:%M:%S")
    print(f"[mqtt] {ts} topic={msg.topic} payload={payload}")

    if payload == "true":
        ntfy_send("GateMate", "Vehicle detected at gate.")
    elif payload == "false":
        if NOTIFY_ON_FALSE:
            ntfy_send("GateMate", "Vehicle cleared at gate.")
    else:
        # Unexpected payload
        ntfy_send("GateMate", f"Unexpected payload on {TOPIC}: {payload}")


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