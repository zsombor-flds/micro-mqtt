// Selected device in Arduino IDE: LOLIN(Wemos) D1 R1

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "DHTesp.h" // DHT sensor library for ESP8266
#include "config.h" // Include configuration settings

WiFiClient espClient;                       // WiFi client object
PubSubClient mqtt_client(espClient);        // MQTT client object
DHTesp dht;                                 // DHT sensor object

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, gmt_offset_sec, ntp_update_interval_ms);  // NTP client for time sync

void connectToWiFi();                       
void connectToMQTTBroker();                 
void mqttCallback(char *topic, byte *payload, unsigned int length);  // Handle incoming MQTT messages
void publishSensorData();                   // Read DHT data and publish to MQTT
String getFormattedTime();                 

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    
    // Setup MQTT client with broker details and callback function
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    
    connectToMQTTBroker();
    
    // Initialize DHT sensor
    dht.setup(dht_pin, DHTesp::DHT22); 
    
    // Start NTP client for time synchronization
    timeClient.begin();
}

void loop() {
    // Ensure MQTT client stays connected
    if (!mqtt_client.connected()) {
        connectToMQTTBroker();
    }

    // Handle incoming MQTT messages and keep the connection alive
    mqtt_client.loop();

    publishSensorData();
    
    delay(1000); 
}

/**
 * Connect to WiFi network and wait until connected.
 */
void connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

/**
Connect to the MQTT broker with username and password. Retry every 5 seconds if connection fails.
 */
void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        // Generate a unique client ID based on MAC address
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.print("Connecting to MQTT Broker as ");
        Serial.println(client_id);

        // Connect to MQTT broker with authentication
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker.");
        } else {
            // Log the connection error and retry
            Serial.print("MQTT connection failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" trying again in 5 seconds.");
            delay(5000);
        }
    }
}

/**
Handle incoming MQTT messages (not used in this specific scenario).
 */
void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println("\n-----------------------");
}

/**
Publish temperature and humidity data from DHT22 sensor to MQTT broker.
 */
void publishSensorData() {
    // Read temperature and humidity values from DHT22
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();

    // Check for valid readings
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor.");
        return;
    }

    // Get the current timestamp
    String timestamp = getFormattedTime();

    // Construct the JSON payload
    String payload = "{\"temperature\": " + String(temperature, 1) + 
                     ", \"humidity\": " + String(humidity, 1) +
                     ", \"timestamp\": \"" + timestamp + "\"}";

    // Publish the payload to the specified MQTT topic
    mqtt_client.publish(mqtt_topic, payload.c_str());
    
    // Log the published data
    Serial.println("Published data: " + payload);
}

/**
Get the current time formatted as an ISO 8601 string with microseconds.
use the NTP client to fetch the current UTC time.
 */
String getFormattedTime() {
    // Update the time from the NTP server
    timeClient.update();

    // Get the epoch time (Unix timestamp)
    unsigned long epochTime = timeClient.getEpochTime();

    // Get milliseconds since the last whole second
    unsigned long fractionalMillis = millis() % 1000;

    // Convert to human-readable time structure (year, month, day, etc.)
    time_t rawTime = epochTime;
    struct tm *timeinfo = gmtime(&rawTime);

    // Extract and format the date and time with microseconds
    // Format: "YYYY-MM-DDTHH:MM:SS.uuuuuu (ISO 8601) for RisingWave"
    char timestamp[30];
    sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02d.%06lu",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
            fractionalMillis * 1000); // Convert milliseconds to microseconds

    return String(timestamp);
}
