/* Add your keys & rename this file to setup.h */
using namespace std;

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

#define MQTT_SERVER "DOMAIN_OR_IP"
#define MQTT_USER "USERNAME"
#define MQTT_PASSWORD "PASSWORD"
#define MQTT_PORT 1883
char *MQTT_STATE_TOPIC_PREFIX = "light/"; // e.g. led/<deviceName> and led/<deviceName>/set

#define OTApassword "PASSWORD" //the password you will need to enter to upload remotely via the ArduinoIDE
#define OTAport 8266

#define LED_PIN 14 // D5 on ESP8266
#define LED_COUNT 12
#define NEO_PIXEL_TYPE NEO_RGB + NEO_KHZ800
#define DEFAULT_EFFECT FX_MODE_RAINBOW_CYCLE
#define DEFAULT_POWER_ON_STATE true

char *deviceName = "led";
