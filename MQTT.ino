


// connect to MQTT
void reconnectMQTT() {
  
  while (!client.connected()) {
    
    Serial.print("Connection to MQTT...");
    

    // П
    uint64_t chipId = ESP.getEfuseMac();  // get ESP32 MAC
    String clientId = "ESP32Client_" + String(chipId, DEC);  // 

    // connect to MQTT via unique clientID
    if (client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT");

      // subscription to topic
      if (client.subscribe(MQTT_TOPIC)) {
        Serial.println("Subscribed to topic");
      } else {
        Serial.println("Subscription error");
      }
    } else {
      Serial.print("Conect error: ");
      Serial.print(client.state());
      Serial.println(" Rejoining in 1 secods...");
      delay(1000);  //
    }
  }
}


// read data from serial Serial
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {  // 
      stringComplete = true;
    }
  }
}




