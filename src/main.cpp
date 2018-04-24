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
byte red = 255;
byte green = 255;
byte blue = 255;
byte brightness = 204;           // 80%
String effect = "rainbow cycle"; // default effect ("static", "rainbow cycle" etc.)
int speed = 50;

/******************************** OTHER GLOBALS *******************************/
const char *on_cmd = "ON";
const char *off_cmd = "OFF";
const char *effectString = "rainbow cycle";
bool stateOn = true;

WiFiClient espClient;
PubSubClient client(espClient);
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long rgbToHex(int r, int g, int b) {
  return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void setEffect(const char *effect) {
  Serial.println(effect);

  if (effect == "static") {
    ws2812fx.setMode(FX_MODE_STATIC);
  }
  if (effect == "blink") {
    ws2812fx.setMode(FX_MODE_BLINK);
  }
  if (effect == "breath") {
    ws2812fx.setMode(FX_MODE_BREATH);
  }
  if (effect == "color wipe") {
    ws2812fx.setMode(FX_MODE_COLOR_WIPE);
  }
  if (effect == "color wipe inverted") {
    ws2812fx.setMode(FX_MODE_COLOR_WIPE_INV);
  }
  if (effect == "color wipe reverse") {
    ws2812fx.setMode(FX_MODE_COLOR_WIPE_REV);
  }
  if (effect == "color wipe reverse inverted") {
    ws2812fx.setMode(FX_MODE_COLOR_WIPE_REV_INV);
  }
  if (effect == "color wipe random") {
    ws2812fx.setMode(FX_MODE_COLOR_WIPE_RANDOM);
  }
  if (effect == "random color") {
    ws2812fx.setMode(FX_MODE_RANDOM_COLOR);
  }
  if (effect == "single dynamic") {
    ws2812fx.setMode(FX_MODE_SINGLE_DYNAMIC);
  }
  if (effect == "multi dynamic") {
    ws2812fx.setMode(FX_MODE_MULTI_DYNAMIC);
  }
  if (effect == "rainbow") {
    ws2812fx.setMode(FX_MODE_RAINBOW);
  }
  if (effect == "rainbow cycle") {
    ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
  }
  if (effect == "scan") {
    ws2812fx.setMode(FX_MODE_SCAN);
  }
  if (effect == "dual scan") {
    ws2812fx.setMode(FX_MODE_DUAL_SCAN);
  }
  if (effect == "fade") {
    ws2812fx.setMode(FX_MODE_FADE);
  }
  if (effect == "theater chase") {
    ws2812fx.setMode(FX_MODE_THEATER_CHASE);
  }
  if (effect == "theater chase rainbow") {
    ws2812fx.setMode(FX_MODE_THEATER_CHASE_RAINBOW);
  }
  if (effect == "running lights") {
    ws2812fx.setMode(FX_MODE_RUNNING_LIGHTS);
  }
  if (effect == "twinkle") {
    ws2812fx.setMode(FX_MODE_TWINKLE);
  }
  if (effect == "twinkle random") {
    ws2812fx.setMode(FX_MODE_TWINKLE_RANDOM);
  }
  if (effect == "twinkle fade") {
    ws2812fx.setMode(FX_MODE_TWINKLE_FADE);
  }
  if (effect == "twinkle fade random") {
    ws2812fx.setMode(FX_MODE_TWINKLE_FADE_RANDOM);
  }
  if (effect == "sparkle") {
    ws2812fx.setMode(FX_MODE_SPARKLE);
  }
  if (effect == "flash sparkle") {
    ws2812fx.setMode(FX_MODE_FLASH_SPARKLE);
  }
  if (effect == "hyper sparkle") {
    ws2812fx.setMode(FX_MODE_HYPER_SPARKLE);
  }
  if (effect == "strobe") {
    ws2812fx.setMode(FX_MODE_STROBE);
  }
  if (effect == "strobe rainbow") {
    ws2812fx.setMode(FX_MODE_STROBE_RAINBOW);
  }
  if (effect == "multi strobe") {
    ws2812fx.setMode(FX_MODE_MULTI_STROBE);
  }
  if (effect == "blink rainbow") {
    ws2812fx.setMode(FX_MODE_BLINK_RAINBOW);
  }
  if (effect == "chase white") {
    ws2812fx.setMode(FX_MODE_CHASE_WHITE);
  }
  if (effect == "chase_color") {
    ws2812fx.setMode(FX_MODE_CHASE_COLOR);
  }
  if (effect == "chase random") {
    ws2812fx.setMode(FX_MODE_CHASE_RANDOM);
  }
  if (effect == "chase rainbow") {
    ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
  }
  if (effect == "chase flash") {
    ws2812fx.setMode(FX_MODE_CHASE_FLASH);
  }
  if (effect == "chase random") {
    ws2812fx.setMode(FX_MODE_CHASE_FLASH_RANDOM);
  }
  if (effect == "chase rainbow white") {
    ws2812fx.setMode(FX_MODE_CHASE_RAINBOW_WHITE);
  }
  if (effect == "chase blackout") {
    ws2812fx.setMode(FX_MODE_CHASE_BLACKOUT);
  }
  if (effect == "chase blackout rainbow") {
    ws2812fx.setMode(FX_MODE_CHASE_BLACKOUT_RAINBOW);
  }
  if (effect == "color sweep random") {
    ws2812fx.setMode(FX_MODE_COLOR_SWEEP_RANDOM);
  }
  if (effect == "running color") {
    ws2812fx.setMode(FX_MODE_RUNNING_COLOR);
  }
  if (effect == "running red blue") {
    ws2812fx.setMode(FX_MODE_RUNNING_RED_BLUE);
  }
  if (effect == "runnning random") {
    ws2812fx.setMode(FX_MODE_RUNNING_RANDOM);
  }
  if (effect == "larson scanner") {
    ws2812fx.setMode(FX_MODE_LARSON_SCANNER);
  }
  if (effect == "comet") {
    ws2812fx.setMode(FX_MODE_COMET);
  }
  if (effect == "fireworks") {
    ws2812fx.setMode(FX_MODE_FIREWORKS);
  }
  if (effect == "fireworks random") {
    ws2812fx.setMode(FX_MODE_FIREWORKS_RANDOM);
  }
  if (effect == "merry christmas") {
    ws2812fx.setMode(FX_MODE_MERRY_CHRISTMAS);
  }
  if (effect == "fire flicker") {
    ws2812fx.setMode(FX_MODE_FIRE_FLICKER);
  }
  if (effect == "fire flicker soft") {
    ws2812fx.setMode(FX_MODE_FIRE_FLICKER_SOFT);
  }
  if (effect == "fire flicker intense") {
    ws2812fx.setMode(FX_MODE_FIRE_FLICKER_INTENSE);
  }
  if (effect == "circus combustus") {
    ws2812fx.setMode(FX_MODE_CIRCUS_COMBUSTUS);
  }
  if (effect == "halloween") {
    ws2812fx.setMode(FX_MODE_HALLOWEEN);
  }
  if (effect == "bicolor chase") {
    ws2812fx.setMode(FX_MODE_BICOLOR_CHASE);
  }
  if (effect == "tricolor chase") {
    ws2812fx.setMode(FX_MODE_TRICOLOR_CHASE);
  }
  if (effect == "icu") {
    ws2812fx.setMode(FX_MODE_ICU);
  }
  if (effect == "custom") {
    ws2812fx.setMode(FX_MODE_CUSTOM);
  }

  Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
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
  color["r"] = red;
  color["g"] = green;
  color["b"] = blue;

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
      if (!ws2812fx.isRunning())
        ws2812fx.start();
    } else if (strcmp(root["state"], off_cmd) == 0) {
      stateOn = false;
      ws2812fx.stop();
    } else {
      sendState();
      return false;
    }
  }

  if (root.containsKey("color")) {
    red = root["color"]["r"];
    green = root["color"]["g"];
    blue = root["color"]["b"];
    ws2812fx.setColor(rgbToHex(red, green, blue));
  }

  if (root.containsKey("brightness")) {
    brightness = root["brightness"];
    ws2812fx.setBrightness(brightness);
  }

  if (root.containsKey("effect")) {
    effectString = root["effect"];
    effect = effectString;
    setEffect(effectString);    
  }

  if (root.containsKey("speed")) {
    speed = root["speed"];
    ws2812fx.setSpeed(speed * 100);
  }

  return true;
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

  if (!processJson(message))
    return;

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
  ws2812fx.setBrightness(brightness);
  ws2812fx.setSpeed(speed);
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

  // Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
}
