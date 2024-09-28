import os
import json
from datetime import datetime
from random import randint

import dotenv
import paho.mqtt.client as mqtt

env = dotenv.find_dotenv(".env")
dotenv.load_dotenv(env)

MQTT_BROKER = os.getenv("MQTT_BROKER")
MQTT_PORT = int(os.getenv("MQTT_PORT"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC")
MQTT_USERNAME = os.getenv("MQTT_USERNAME")
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")


def generate_message() -> dict:
    my_message = dict()
    my_message["timestamp"] = datetime.now().isoformat()
    my_message["temperature"] = randint(-10, 35)
    my_message["humidity"] = randint(0, 100)
    my_message = json.dumps(my_message)
    return my_message


def on_publish(client, userdata, mid, reason_code=None, properties=None):
    # reason_code and properties will only be present in MQTTv5. It's always unset in MQTTv3
    userdata.remove(mid)


if __name__ == "__main__":
    # Set to store unacknowledged message IDs
    unacked_publish = set()

    # Create the MQTT client object with the appropriate callback API version
    mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

    # Set the on_publish callback function
    mqttc.on_publish = on_publish

    # Attach the unacked_publish set to user data
    mqttc.username_pw_set(username=MQTT_USERNAME, password=MQTT_PASSWORD)
    mqttc.user_data_set(unacked_publish)

    # Connect to the EMQX broker running locally via Docker
    mqttc.connect(MQTT_BROKER, MQTT_PORT)
    # Start the loop to process network traffic
    mqttc.loop_start()
    print("Starting to publish messages")
    while True:
        msg_info = mqttc.publish(MQTT_TOPIC, generate_message(), qos=1)
        unacked_publish.add(msg_info.mid)

        msg_info.wait_for_publish()
    else:
        # Disconnect the client
        mqttc.disconnect()
        mqttc.loop_stop()
