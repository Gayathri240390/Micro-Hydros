import argparse
import json
import math
import time


def create_measurement(timestamp_ms, cycle):
    phase = cycle % 20
    return {
        "timestamp_ms": timestamp_ms,
        "air_inside_c": round(22.0 + phase * 0.05, 2),
        "air_outside_c": round(18.0 + phase * 0.03, 2),
        "water_c": round(20.0 + phase * 0.02, 2),
        "humidity_inside_pct": round(56.0 + phase * 0.1, 2),
        "valid": True,
    }


def validate(measurement):
    limits = {
        "air_inside_c": (-10.0, 60.0),
        "air_outside_c": (-30.0, 60.0),
        "water_c": (0.0, 50.0),
        "humidity_inside_pct": (0.0, 100.0),
    }
    for field, (minimum, maximum) in limits.items():
        value = measurement[field]
        if not math.isfinite(value) or not minimum <= value <= maximum:
            return False
    return True


def main():
    parser = argparse.ArgumentParser(description="Run the MicroHydros simulated measurement demo.")
    parser.add_argument("--count", type=int, default=5, help="number of readings; 0 runs continuously")
    parser.add_argument("--interval", type=float, default=1.0, help="seconds between readings")
    parser.add_argument("--invalid-every", type=int, default=0, help="make every Nth water reading invalid")
    parser.add_argument("--mqtt-host", help="optional MQTT broker hostname")
    parser.add_argument("--mqtt-port", type=int, default=1883)
    args = parser.parse_args()

    if args.count < 0 or args.interval < 0 or args.invalid_every < 0:
        parser.error("count, interval, and invalid-every must be non-negative")

    publisher = None
    if args.mqtt_host:
        try:
            import paho.mqtt.client as mqtt
        except ImportError:
            parser.error("MQTT publishing requires paho-mqtt; install requirements.txt")
        publisher = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        publisher.connect(args.mqtt_host, args.mqtt_port, keepalive=30)
        publisher.loop_start()

    cycle = 0
    try:
        while args.count == 0 or cycle < args.count:
            measurement = create_measurement(int(time.monotonic() * 1000), cycle)
            if args.invalid_every and (cycle + 1) % args.invalid_every == 0:
                measurement["water_c"] = -1.0
            measurement["valid"] = validate(measurement)
            payload = json.dumps(measurement, separators=(",", ":"))
            print(payload, flush=True)
            if publisher:
                publisher.publish("microhydros/measurement", payload, qos=0)
            cycle += 1
            if args.count == 0 or cycle < args.count:
                time.sleep(args.interval)
    except KeyboardInterrupt:
        pass
    finally:
        if publisher:
            publisher.loop_stop()
            publisher.disconnect()


if __name__ == "__main__":
    main()