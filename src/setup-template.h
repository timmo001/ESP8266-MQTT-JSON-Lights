/* Add your keys & rename this file to auth.h */
#ifndef _AUTH_DETAILS
#define _AUTH_DETAILS

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

#define MQTT_SERVER "DOMAIN_OR_IP"
#define MQTT_USER "USERNAME"
#define MQTT_PASSWORD "PASSWORD"
#define MQTT_PORT 1883
char *MQTT_STATE_TOPIC_PREFIX = "light/"; // e.g. led/<deviceName> and led/<deviceName>/set

#define OTApassword "PASSWORD" //the password you will need to enter to upload remotely via the ArduinoIDE
#define OTAport 8266

#define PIN 14 // D5 on ESP8266
#define LED_COUNT 12

char *deviceName = "crystal001";

byte defaultBrightness = 209;    // 80%
String effect = "rainbow cycle"; // default effect ("solid", "rainbow cycle" etc.)
int speed = 50;                  // default speed (1-150)

void setup_config() {
  Serial.println();
  Serial.print("ESP8266 Chip id: ");
  Serial.println(ESP.getChipId());
  Serial.print("Device Name: ");
  Serial.println(deviceName);
}

#endif
