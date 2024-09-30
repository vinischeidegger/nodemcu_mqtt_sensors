unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 2000;        // Interval at which to publish sensor readings

void readTemperature(){
    unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    sensors.requestTemperatures(); 
    temperatureC = sensors.getTempCByIndex(0);
    temperatureC1 = sensors.getTempCByIndex(1);
    Serial.printf("Temperature: %.2f \n", temperatureC);
    Serial.printf("Temperature 1: %.2f \n", temperatureC1);
    //Serial.printf("X: %i Y: %i\n", cursorX, cursorY);

    if(!wifiDisconnected) {
      // Temperature MQTT Topics
      char mqtt_temp_topic[] = "esp/dht/temperature";
      // Publish an MQTT message on topic esp/dht/temperature
      uint16_t packetIdPub1 = mqttClient.publish(mqtt_temp_topic, 1, true, String(temperatureC).c_str());                            
      Serial.printf("Publishing on topic %s at QoS 1, packetId: %i ", mqtt_temp_topic, packetIdPub1);
      Serial.printf("Message: %.2f \n", temperatureC);
    }
  }
}