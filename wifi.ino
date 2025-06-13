extern bool isServerStarted;
extern bool isWhiteDiodeNeeded;

// 
extern String readData(const char* path);

// Wi-Fi
void WifiConnect() {
  String wifiData = readData("/data.txt");
  if (wifiData == "") {
    Serial.println("Can't get data from file!");
    return;
  }

  int delimiterIndex = wifiData.indexOf("\n");
  if (delimiterIndex == -1) {
     Serial.println("Wrong data format. Resetting file");
    
    writeData("/data.txt", "wifiName \n wifiPass");
    return;
  }

  String ssid = wifiData.substring(0, delimiterIndex);  // First row is SSID
  String password = wifiData.substring(delimiterIndex + 1);  // Second row is password

  Serial.print("Connecting to: ");
  Serial.println(ssid);

  digitalWrite(LED_PIN_CONNECTED, HIGH);  // Turn off the blue led
  WiFi.begin(ssid.c_str(), password.c_str());

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {  // 5 seconds to connect
    delay(500);
    Serial.print(".");
    attempts++;
  }

  digitalWrite(LED_PIN_CONNECTED, LOW);  // Turn off the blue led

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to Wi-Fi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else 
  
  {
    Serial.println("\nConnection error!");
   // Local mode setup
  isServerStarted = true;
  isWhiteDiodeNeeded = true;
  // Server initialization
extern void setupServer();
// server setup
const char* ssid = "LampESP322";  
const char* password = "";  

  WiFi.softAP(ssid, password);
  Serial.print("Server IP: ");
  Serial.println(WiFi.softAPIP());

  // 
  setupServer();

    // Server start
    WiFi.softAP("LampESP322", "");
  }
}
