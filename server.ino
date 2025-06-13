

// create webserver on port 80
WebServer server(80);

// 
extern bool writeData(const char* path, const String& data);

// HTML
void handleRoot() {
  String html = "<html><head><meta charset='UTF-8'></head><body><h1>FRIENDSHIP LAMP</h1>";
  html += "<p>WiFi Name:</p>";
  html += "<form action='/save' method='POST'>";
  html += "<input type='text' name='wifi_name' placeholder='Enter WifI SSID'><br><br>";
  html += "<p>WiFi password:</p>";
  html += "<input type='text' name='wifi_pass' placeholder='Enter password'><br><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form></body></html>";
  server.send(200, "text/html; charset=utf-8", html); // UTF-8
}

// 
void handleSave() {
  String wifiName = "";
  String wifiPass = "";

  if (server.hasArg("wifi_name")) {
    wifiName = server.arg("wifi_name");
  }

  if (server.hasArg("wifi_pass")) {
    wifiPass = server.arg("wifi_pass");
  }

  // Save data to flash
  String dataToSave = wifiName + "\n" + wifiPass;  // First row is SSID, second is password
  if (writeData("/data.txt", dataToSave)) {
    // 
    Serial.println("Data saved:");
    Serial.println("WiFi SSID: " + wifiName);
    Serial.println("WiFi Password: " + wifiPass);
    server.send(200, "text/html; charset=utf-8", "<html><body><h1>Data saved!</h1><a href='/'>Back</a></body></html>");
  } else {
    server.send(500, "text/html; charset=utf-8", "<html><body><h1>Writing error!</h1><a href='/'>Back</a></body></html>");
  }
}

// 
void setupServer() {
  server.on("/", HTTP_GET, handleRoot);  // Main page
  server.on("/save", HTTP_POST, handleSave);  // 

  // Запуск сервера
  server.begin();
  Serial.println("Server started");
  digitalWrite(LED_PIN_FAILED, HIGH);  // Enable white LED on D16 pin
}
