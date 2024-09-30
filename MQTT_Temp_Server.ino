#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AsyncMqttClient.h>
#include <AsyncMqttClient.hpp>
#include <DallasTemperature.h>
#include <DNSServer.h>
//#include <ESPAsyncTCP.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <OneWire.h>
#include <SPI.h>
#include <Ticker.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13

// Raspberri Pi Mosquitto MQTT Broker
#define MQTT_HOST IPAddress(192, 168, 15, 11)
// For a cloud MQTT broker, type the domain name
//#define MQTT_HOST "example.com"
#define MQTT_PORT 1883

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OneWire oneWireBus(D3);
DallasTemperature sensors(&oneWireBus);

/* Set these to your desired softAP credentials. They are not configurable at runtime */
const char *softAP_ssid = "ESP_ap";
const char *softAP_password = "12345678";
// Web server
ESP8266WebServer server(80);
// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

// WiFi parameters to be configured
String ssid = "";
String password = "";
String ipAddress = "";
bool wifiDisconnected = true;

//const int buttonPin_RST = 2;     // the number of the reset button pin
//const int buttonPin_SET = 3;     // the number of the set button pin
const int buttonPin_MID = 3;     // the number of the middle button pin
const int buttonPin_RHT = D7;     // the number of the right button pin
const int buttonPin_LFT = D6;     // the number of the left button pin
const int buttonPin_DWN = D5;     // the number of the down button pin
const int buttonPin_UP = D4;      // the number of the up button pin

float temperatureC = -1;
float temperatureC1 = -1;
int buffer = 0;
int maxOptions = 5;
int wifiRetryCount = 0;
int cursorX = 0;
int cursorY = 0;

/** Should I connect to WLAN asap? */
boolean connect;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  initializeButtons();
  initializeDisplay();
  sensors.begin();
  initializeWifi();
  initializeMqtt();
  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  initializeServer();

  // Connect to WiFi
  connectToWifi(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  readTemperature();
  displaymenu();
  checkWifi();
  //Serial.println(connect?"Connect":"Do not Connect");
}
