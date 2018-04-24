/*
  To use this code you will need the following dependancies:

  - Support for the ESP8266 boards.
        - You can add it to the board manager by going to File -> Preference and pasting http://arduino.esp8266.com/stable/package_esp8266com_index.json into the Additional Board Managers URL field.
        - Next, download the ESP8266 dependancies by going to Tools -> Board -> Board Manager and searching for ESP8266 and installing it.

  - You will also need to download the follow libraries by going to Sketch -> Include Libraries -> Manage Libraries
      - Adafruit NeoPixel
      - PubSubClient
      - ArduinoJSON

  Sample MQTT Payload:
  {
    "brightness": 120,
    "color": {
      "r": 255,
      "g": 100,
      "b": 100
    },
    "flash": 2,
    "state": "ON"
  }
*/
using namespace std;

#include "setup.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <WS2812FX.h>
#include <WiFiUdp.h>

/****************************************FOR JSON***************************************/
const int BUFFER_SIZE = JSON_OBJECT_SIZE(10);
#define MQTT_MAX_PACKET_SIZE 512

/*********************************** Defaults ********************************/
byte defaultBrightness = 209;    // 80%
String effect = "rainbow cycle"; // default effect ("solid", "rainbow cycle" etc.)
int speed = 50;                  // default speed (1-150)

/*********************************** LED Defintions ********************************/
// Real values as requested from the MQTT server
byte realRed = 255;
byte realGreen = 255;
byte realBlue = 255;

// Previous requested values
byte previousRed = 255;
byte previousGreen = 255;
byte previousBlue = 255;

// Values as set to strip
byte red = 255;
byte green = 255;
byte blue = 255;
byte brightness = 204; // 80%

/******************************** OTHER GLOBALS *******************************/
const char *on_cmd = "ON";
const char *off_cmd = "OFF";
const char *effectString = "solid";
String previousEffect = "solid";
bool stateOn = true;
bool transitionDone = true;
bool transitionAbort = false;

WiFiClient espClient;
PubSubClient client(espClient);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// #include "effects.h"

unsigned long rgbToHex(int r, int g, int b) {
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to SSID: ");
  Serial.println(WIFI_SSID);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.hostname(deviceName);

  if (WiFi.status() != WL_CONNECTED) { // FIX FOR USING 2.3.0 CORE (only .begin if not connected)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendState() {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();

  root["state"] = (stateOn) ? on_cmd : off_cmd;
  JsonObject &color = root.createNestedObject("color");
  color["r"] = realRed;
  color["g"] = realGreen;
  color["b"] = realBlue;

  root["brightness"] = brightness;
  root["speed"] = speed;
  root["effect"] = effect.c_str();

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  char combinedArray[sizeof(MQTT_STATE_TOPIC_PREFIX) + sizeof(deviceName)];
  sprintf(combinedArray, "%s%s", MQTT_STATE_TOPIC_PREFIX, deviceName); // with word space
  client.publish(combinedArray, buffer, true);
}

bool processJson(char *message) {
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject &root = jsonBuffer.parseObject(message);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return false;
  }

  if (root.containsKey("state")) {
    if (strcmp(root["state"], on_cmd) == 0) {
      stateOn = true;
    } else if (strcmp(root["state"], off_cmd) == 0) {
      stateOn = false;
    } else {
      sendState();
      return false;
    }
  }

  if (root.containsKey("speed")) {
    speed = root["speed"];
  }

  if (root.containsKey("color")) {
    realRed = root["color"]["r"];
    realGreen = root["color"]["g"];
    realBlue = root["color"]["b"];
    ws2812fx.setColor(rgbToHex(realRed, realGreen, realBlue));
  }

  if (root.containsKey("brightness")) {
    brightness = root["brightness"];
    ws2812fx.setBrightness(brightness);
  }

  if (root.containsKey("effect")) {
    effectString = root["effect"];
    // effect = effectString.;
    // ws2812fx.setMode(FX_MODE_);
  }

  return true;
}

void setOff() {
  stateOn = false;
  previousRed, previousGreen, previousBlue = 0;

  Serial.println("LED: OFF");
}

void setOn() {
  stateOn = true;

  Serial.println("LED: ON");
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.println("");
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  previousEffect = effect;

  if (!processJson(message)) {
    return;
  }

  previousRed = red;
  previousGreen = green;
  previousBlue = blue;

  Serial.print("realRed: ");
  Serial.println(realRed);
  Serial.print("realGreen: ");
  Serial.println(realGreen);
  Serial.print("realBlue: ");
  Serial.println(realBlue);
  Serial.print("brightness: ");
  Serial.println(brightness);

  if (stateOn) {
    red = map(realRed, 0, 255, 0, brightness);
    green = map(realGreen, 0, 255, 0, brightness);
    blue = map(realBlue, 0, 255, 0, brightness);
  } else {
    red = 0;
    green = 0;
    blue = 0;
  }

  Serial.print("effect: ");
  Serial.println(effect);

  if (stateOn) {
    setOn();
  } else {
    setOff(); // NOTE: Will change transitionDone
  }

  sendState();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(deviceName, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");

      char combinedArray[sizeof(MQTT_STATE_TOPIC_PREFIX) + sizeof(deviceName) + 4];
      sprintf(combinedArray, "%s%s/set", MQTT_STATE_TOPIC_PREFIX, deviceName); // with word space
      client.subscribe(combinedArray);

      // setOff();
      sendState();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);   // Initialize the LED_BUILTIN pin as an output (So it doesnt float as a LED is on this pin)
  digitalWrite(LED_BUILTIN, LOW); // Turn the status LED on
  // pinMode(DATA_LED_PIN_RELAY, OUTPUT);    // Initialize the P-Channel MOSFET for the LED strip
  // digitalWrite(DATA_LED_PIN_RELAY, LOW);  // Turn the LED strip on

  Serial.begin(115200);

  delay(500); // Wait for Leds to init and Cap to charge
  // setup_config();

  // End of trinket special code
  ws2812fx.init();
  ws2812fx.setBrightness(defaultBrightness);
  ws2812fx.setSpeed(200);
  ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  ws2812fx.start();
  stateOn = true;

  setup_wifi();

  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //OTA SETUP
  ArduinoOTA.setPort(OTAport);
  ArduinoOTA.setHostname(deviceName);                // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setPassword((const char *)OTApassword); // No authentication by default

  ArduinoOTA.onStart([]() {
    Serial.println("Starting");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("Error[%u]: " + error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  Serial.println("Ready");

  // OK we are connected
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    Serial.print("WIFI Disconnected. Attempting reconnection.");
    setup_wifi();
    return;
  }

  client.loop(); // Check MQTT

  ArduinoOTA.handle(); // Check OTA Firmware Updates

  ws2812fx.service();
}
