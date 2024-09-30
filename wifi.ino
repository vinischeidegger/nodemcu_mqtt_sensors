void initializeWifi(){
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  loadCredentials();
  connect = (ssid.length() > 0) && (WiFi.getMode() == WIFI_STA);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  //wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event) {
    Serial.println("Station disconnected");
    wifiDisconnected = true;
   // WiFi.disconnect();
    //delay(10000);
    //connectToWifi();
  });
}

void connectToWifi(bool wait) {
  connect = false;
  Serial.println("\nConnecting to Wi-Fi...");
  Serial.printf("%s\n", ssid.c_str());
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  int retryCount = 0;

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  while (wait && WiFi.status() != WL_CONNECTED && retryCount < 10) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }
  int status = WiFi.status();
  Serial.printf("\nWiFi status: %i\n", status);
  wifiDisconnected = (status != WL_CONNECTED);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  wifiDisconnected = false;
  ipAddress = WiFi.localIP().toString();
  Serial.println(ipAddress);
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  //Serial.println("Disconnected from Wi-Fi.");
  if(!wifiDisconnected) {
    //Serial.println("Just disconnected from WiFi");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiRetryCount = 0;
  }
  if(wifiRetryCount < 3) {
    wifiRetryCount++;
    //Serial.printf("Retry count %i\n", wifiRetryCount);
    WiFi.disconnect(true);
    delay(500);
    //connectToWifi();
    wifiReconnectTimer.once(2, connectToWifi, false);
  }
}

unsigned long wifiPreviousMillis = 0;   // Stores last time temperature was published
const long wifiInterval = 10000;        // Interval at which to publish sensor readings

void checkWifi(){
    unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  if (wifiDisconnected && currentMillis - wifiPreviousMillis >= wifiInterval) {
    // Save the last time a new reading was published
    wifiPreviousMillis = currentMillis;
    int status = WiFi.status();
    Serial.printf("\nWiFi status: %i\n", status);
    wifiDisconnected = (status != WL_CONNECTED);
    Serial.println("Reconnecting to Wi-Fi.");
    if(wifiDisconnected)
      connectToWifi(false);
  }
}
