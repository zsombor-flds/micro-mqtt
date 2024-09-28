// This file contains the configuration settings for the ESP8266 sensor to MQTT producer.
// Remove the .example extension from the file name before using it.
#ifndef CONFIG_H
#define CONFIG_H

/******************************************
 *          CONFIGURATION SETTINGS        *
 ******************************************/

// WiFi credentials
const char *ssid = "";                     // Replace with your WiFi SSID (name)
const char *password = "";   // Replace with your WiFi password

// MQTT Broker settings
const char *mqtt_broker = "";       // MQTT broker address
const char *mqtt_topic = "raw/sensor";       // MQTT topic to publish data
const int mqtt_port = 1883;                       // MQTT broker port (default: 1883)
const char *mqtt_username = "test";      // Replace with your MQTT username
const char *mqtt_password = "test";      // Replace with your MQTT password

// DHT sensor configuration
const int dht_pin = D2;                           // GPIO pin where DHT22 is connected

// NTP Client configuration
const char *ntp_server = "pool.ntp.org";          // NTP server for time sync
const long gmt_offset_sec = 0;                    // Time offset in seconds from GMT (UTC)
const int ntp_update_interval_ms = 60000;         // NTP update interval in milliseconds

#endif // CONFIG_H
