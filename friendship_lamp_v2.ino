#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h> // MQTT
#include <Adafruit_NeoPixel.h> // LED strip


/*
██╗███╗░░██╗██╗████████╗██╗░█████╗░██╗░░░░░██╗███████╗░█████╗░████████╗██╗░█████╗░███╗░░██╗
██║████╗░██║██║╚══██╔══╝██║██╔══██╗██║░░░░░██║╚════██║██╔══██╗╚══██╔══╝██║██╔══██╗████╗░██║
██║██╔██╗██║██║░░░██║░░░██║███████║██║░░░░░██║░░███╔═╝███████║░░░██║░░░██║██║░░██║██╔██╗██║
██║██║╚████║██║░░░██║░░░██║██╔══██║██║░░░░░██║██╔══╝░░██╔══██║░░░██║░░░██║██║░░██║██║╚████║
██║██║░╚███║██║░░░██║░░░██║██║░░██║███████╗██║███████╗██║░░██║░░░██║░░░██║╚█████╔╝██║░╚███║
╚═╝╚═╝░░╚══╝╚═╝░░░╚═╝░░░╚═╝╚═╝░░╚═╝╚══════╝╚═╝╚══════╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░╚════╝░╚═╝░░╚══╝
*/

// modes
bool isServerStarted = false;


// 
extern bool initStorage();
extern bool writeData(const char* path, const String& data);
extern String readData(const char* path);
extern void WifiConnect(); // Connection to Wi-Fi

//Server initialisation
extern WebServer server;

// LED pins
const int LED_PIN_CONNECTED = 4;  // Blue LED
const int LED_PIN_FAILED = 18;    // White LED
const int LED_PIN_WORK = 5; // Green LED

// MQTT server settings
#define MQTT_SERVER "domain.net"
#define MQTT_PORT 12345
#define MQTT_USER "user"
#define MQTT_PASSWORD "passwd"
#define MQTT_TOPIC "topic/subtopic/verysubtopic"

WiFiClient espClient;
PubSubClient client(espClient);
String inputString = "";  // serial buffer
bool stringComplete = false;  // 

//stopwatch 
unsigned long previousMillis = 0;  // 
const long interval = 10000;  // 
unsigned long startMillis = 0;  // 

//import from other tab (MQTT)
extern void reconnectMQTT(); //MQTT connection
extern void mqttCallback(char* topic, byte* payload, unsigned int length); //получение с ообщений. поменять с войда на другое
extern void serialEvent(); // read from serial


//
//potentiometers
#define POTENTIOMETER_PIN_R 34  // 
#define POTENTIOMETER_PIN_G 32  // 
#define POTENTIOMETER_PIN_B 35  // 

#define LED_PIN 23  // LED strip pin
#define LED_COUNT 15  // strip LED count

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);

// potentiometer data store
int ref_red = 0;
int ref_green = 0;
int ref_blue = 0;
bool isUpdateNeeded = false;
String unite_colors = "";  // to sending colors

byte noise_corr = 2; // noise correction 
bool isWhiteDiodeNeeded = false;

#define NUM_SAMPLES 10
int valuesR[NUM_SAMPLES] = {0};
int valuesG[NUM_SAMPLES] = {0};
int valuesB[NUM_SAMPLES] = {0};
int index2 = 0;

// potentiometer smoothed value
int getSmoothedValue(int *values) {
    int sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += values[i];
    }
    return sum / NUM_SAMPLES;
}


/*
░██████╗███████╗████████╗██╗░░░██╗██████╗░
██╔════╝██╔════╝╚══██╔══╝██║░░░██║██╔══██╗
╚█████╗░█████╗░░░░░██║░░░██║░░░██║██████╔╝
░╚═══██╗██╔══╝░░░░░██║░░░██║░░░██║██╔═══╝░
██████╔╝███████╗░░░██║░░░╚██████╔╝██║░░░░░
╚═════╝░╚══════╝░░░╚═╝░░░░╚═════╝░╚═╝░░░░░
*/


void setup() {
  delay(1000); // startup delay
  Serial.begin(115200);


  pinMode(LED_PIN_CONNECTED, OUTPUT); // Blue
  pinMode(LED_PIN_FAILED, OUTPUT); // White
  pinMode(LED_PIN_WORK, OUTPUT);  // Green
  

  // SPIFFS initialization
  if (!initStorage()) {
    Serial.println("SPIFFS initialization error");
    return;
  }

  // Wi-Fi
  WifiConnect();

 
  // setup of the MQTT s
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);  // 
  client.setSocketTimeout(5);  // 
  client.setKeepAlive(60);  // 

  


  // led strip
    strip.begin();
    strip.show(); // 
    pinMode(POTENTIOMETER_PIN_R, INPUT);
    pinMode(POTENTIOMETER_PIN_G, INPUT);
    pinMode(POTENTIOMETER_PIN_B, INPUT);

}

void loop() {

  if (isServerStarted) // Server is started. Local mode is active
  {
    server.handleClient();  // 

      //Local use mode

      // 
    int valueR = analogRead(POTENTIOMETER_PIN_R);  
    int valueG = analogRead(POTENTIOMETER_PIN_G);  
    int valueB = analogRead(POTENTIOMETER_PIN_B);  
    // Cut the brihtness (0-152). Values over 152 should not be used, because strip will warm up too much.
    valueR = valueR/27;
    valueG = valueG/27;
    valueB = valueB/27;

    // low value noise correction   
    if (valueR < 10) {valueR = 0;}
    if (valueG < 10) {valueG = 0;}
    if (valueB < 10) {valueB = 0;}

    strip.fill(strip.Color(valueR, valueG, valueB)); // (R, G, B)
    strip.show();
    
    //Turn off the white LED if lamp is used. 

    
    if(isWhiteDiodeNeeded)
    {
      if (valueR > ref_red+noise_corr || valueR < ref_red-noise_corr)     
      {digitalWrite(LED_PIN_FAILED, LOW);}
      //зелёный
      if (valueG > ref_green+noise_corr || valueG < ref_green-noise_corr)     
      {digitalWrite(LED_PIN_FAILED, LOW);}
      //синий
      if (valueB > ref_blue+noise_corr || valueB < ref_blue-noise_corr)
     {digitalWrite(LED_PIN_FAILED, LOW);}
    }
  //
 




  } // end of local mode

  else // local server is not started, online mode is on
  {
  // MQTT connection check
  if (!client.connected()) {
    digitalWrite(LED_PIN_WORK, HIGH);  // Show that MQTT is not connected
    reconnectMQTT();
  }
  else
  {
    digitalWrite(LED_PIN_WORK, LOW);  // MQTT is connected
  }
  client.loop();  // 

  // 
  if (stringComplete) {
    Serial.print("Sending: ");
    Serial.println(inputString);

    client.publish(MQTT_TOPIC, inputString.c_str());
    inputString = "";  // Очищаем буфер
    stringComplete = false;
  }

    // strip
    int valueR = analogRead(POTENTIOMETER_PIN_R);  
    int valueG = analogRead(POTENTIOMETER_PIN_G); 
    int valueB = analogRead(POTENTIOMETER_PIN_B);  
    // Cut the brihtness (0-152). Values over 152 should not be used, because strip will warm up too much.
    valueR = valueR/27;
    valueG = valueG/27;
    valueB = valueB/27;
    

    // save last values to array
    valuesR[index2] = valueR;
    valuesG[index2] = valueG;
    valuesB[index2] = valueB;
    
    index2 = (index2 + 1) % NUM_SAMPLES; 
    valueR = getSmoothedValue(valuesR);
    valueG = getSmoothedValue(valuesG);
    valueB = getSmoothedValue(valuesB);

    // low values noise correction
    if (valueR < 10) {valueR = 0;}
    if (valueG < 10) {valueG = 0;}
    if (valueB < 10) {valueB = 0;}

. 
  //red
  if (valueR > ref_red+noise_corr || valueR < ref_red-noise_corr)
      {ref_red = valueR; // 
      isUpdateNeeded = true;} // 
  else
      {valueR = ref_red;} // 
  //green
  if (valueG > ref_green+noise_corr || valueG < ref_green-noise_corr)
      {ref_green = valueG;
      isUpdateNeeded = true;} // 
  else
      {valueG = ref_green;}
  //blue
  if (valueB > ref_blue+noise_corr || valueB < ref_blue-noise_corr)
      {ref_blue = valueB;
      isUpdateNeeded = true;} //
  else
      {valueB = ref_blue;}

  //T
  if (isUpdateNeeded) // 
  {
    //first lamp be updated locally, then data will be sent to MQTT server and readed from it (in case if ping is high, it will make changins smooth)
    strip.fill(strip.Color(valueR, valueG, valueB)); //(R, G, B)
    strip.show();


    unite_colors = "Color," + String(valueG) + "," + String(valueR) + "," + String(valueB);
    client.publish(MQTT_TOPIC, unite_colors.c_str());    
    isUpdateNeeded = false; 
  }
  else
  {
    

    

  }
  } // end isServerStarted
  


   
  
} //loop end





// 
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  // 
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];  // 
  }
  
  // if message starts with "Color"
    if (message.startsWith("Color")) {
        // split by commas
        int firstCommaIndex = message.indexOf(','); // 
        int secondCommaIndex = message.indexOf(',', firstCommaIndex + 1); //
        int thirdCommaIndex = message.indexOf(',', secondCommaIndex + 1); // 

        // get colors
        String redStr = message.substring(firstCommaIndex + 1, secondCommaIndex); // red
        String greenStr = message.substring(secondCommaIndex + 1, thirdCommaIndex); // green
        String blueStr = message.substring(thirdCommaIndex + 1); // blue

        //
        int red = redStr.toInt();
        int green = greenStr.toInt();
        int blue = blueStr.toInt();

        // check the range (0-255)
        if (red >= 0 && red <= 160 && green >= 0 && green <= 160 && blue >= 0 && blue <= 160) {
            // 
            //if data from MQTT, make the 2 diodes blink so user knows it's working
                   
             digitalWrite(LED_PIN_CONNECTED, HIGH); // blue
             digitalWrite(LED_PIN_FAILED, HIGH); // white

            
            strip.fill(strip.Color(green, red, blue)); //  (R, G, B)
            strip.show();

             digitalWrite(LED_PIN_CONNECTED, LOW); // blue
             digitalWrite(LED_PIN_FAILED, LOW); // white   

            Serial.println(String(red) + "//" + String(green) + "//" + String(blue));
        } else {
            Serial.println("Received invalid RGB values.");
        }
    } else {
        Serial.println(message); // it's not color
    }
  
}


