void initializeMqtt(){
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  String text; 
    switch( reason) {
    case AsyncMqttClientDisconnectReason::TCP_DISCONNECTED:
      text = "TCP_DISCONNECTED"; 
      break; 
    case AsyncMqttClientDisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION:
      text = "MQTT_UNACCEPTABLE_PROTOCOL_VERSION"; 
      break; 
    case AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED:
      text = "MQTT_IDENTIFIER_REJECTED";  
      break;
    case AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE: 
      text = "MQTT_SERVER_UNAVAILABLE"; 
      break;
    case AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS:
      text = "MQTT_MALFORMED_CREDENTIALS"; 
      break;
    case AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED:
      text = "MQTT_NOT_AUTHORIZED"; 
      break;
    }
  Serial.printf(" [%8u] Disconnected from the broker reason = %s\n", millis(), text.c_str() );
  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

