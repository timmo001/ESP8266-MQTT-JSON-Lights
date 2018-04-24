# ESP8266-MQTT-JSON-Lights
ESP8266 MQTT JSON Lights. Supports flash, fade, transitions, speed, effects, and OTA uploads. Based on https://github.com/DotNetDann/ESP8266-MQTT-JSON-SK6812RGBW-HomeAssistant and https://github.com/bruhautomation/ESP-MQTT-JSON-Digital-LEDs

## Setup
See https://github.com/bruhautomation/ESP-MQTT-JSON-Digital-LEDs/blob/master/README.md for initial hardware setup.

- Rename src/setup-template.h to src/setup.h and add your network, MQTT and lighting setup information. Take note of the 'deviceName' you set. You will need this later to send MQTT messages to
- Using Atom or VS Code, install Platform IO https://platformio.org/platformio-ide
- Once setup, install the esp8266 embedded platform
- Install library dependencies:
    - Adafruit NeoPixel
    - ArduinoJson
    - PubSubClient
- Build the project (Ctrl+Alt+B) and check for any errors
- Upload to your board of choice (Ctrl+Alt+U). This project was created specifically for the NodeMCU but can be configured to work with another WiFi board with some tinkering.

## Example Home Assistant Configuration
```yaml
light:
  platform: mqtt_json
  name: 'WS2812 Lights'
  state_topic: 'light/led'
  command_topic: 'light/led/set'
  effect: true
  effect_list:
    - static
    - blink
    - breath
    - color wipe
    - color wipe inverted
    - color wipe reverse
    - color wipe reverse inverted
    - color wipe random
    - random color
    - single dynamic
    - multi dynamic
    - rainbow
    - rainbow cycle
    - scan
    - dual scan
    - fade
    - theater chase
    - theater chase rainbow
    - running lights
    - twinkle
    - twinkle random
    - twinkle fade
    - twinkle fade random
    - sparkle
    - flash sparkle
    - hyper sparkle
    - strobe
    - strobe rainbow
    - multi strobe
    - blink rainbow
    - chase white
    - chase_color
    - chase random
    - chase rainbow
    - chase flash
    - chase random
    - chase rainbow white
    - chase blackout
    - chase blackout rainbow
    - color sweep random
    - running color
    - running red blue
    - running random
    - larson scanner
    - comet
    - fireworks
    - fireworks random
    - merry christmas
    - fire flicker
    - fire flicker soft
    - fire flicker intense
    - circus combustus
    - halloween
    - bicolor chase
    - tricolor chase
    - icu
  brightness: true
  flash: true
  rgb: true
  speed: true
  optimistic: false
  qos: 0
```
