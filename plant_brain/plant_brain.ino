#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT11.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include "esp_system.h"

#define LED 2
#define delay_readings 5000

/* Sensors */
DHT11 dht11(26);
/* END Sensors */

/* Wifi */
/* const char* ssid = "Vodafone-50632873";
const char* password = "zemaFKUdaX1."; */
const char* mqtt_server = "mqtt-dashboard.com";

const char* ssids[] = {"Vodafone-50632873","Vodafone-A83245675","Vodafone-A83245675-EXT"};
const char* passwords[] = {"zemaFKUdaX1.","3qxhfYKxpEYgdtad","3qxhfYKxpEYgdtad"};

int randNumber;
String topic;
String debug_topic = "smart_plants_debug";

String tmp;
int wifi_cell=0;
const int maxTries=50;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(128)
char msg[MSG_BUFFER_SIZE];
/* END Wifi */

void printWiFiStatus() {
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      Serial.println("WiFi status: IDLE");
      break;
    case WL_NO_SSID_AVAIL:
      Serial.println("WiFi status: NO SSID AVAILABLE");
      break;
    case WL_SCAN_COMPLETED:
      Serial.println("WiFi status: SCAN COMPLETED");
      break;
    case WL_CONNECTED:
      Serial.println("WiFi status: CONNECTED");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("WiFi status: CONNECTION FAILED");
      break;
    case WL_CONNECTION_LOST:
      Serial.println("WiFi status: CONNECTION LOST");
      break;
    case WL_DISCONNECTED:
      Serial.println("WiFi status: DISCONNECTED");
      break;
    default:
      Serial.println("WiFi status: UNKNOWN");
      break;
  }
}

/* void setup_wifi() {
  int attempts = 0;

  while (true) {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssids[wifi_cell]);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssids[wifi_cell], passwords[wifi_cell]);

    attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < maxTries) {
      delay(500);
      Serial.print(".");
      printWiFiStatus(); // Print the current WiFi status for debugging
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      break;
    } else {
      Serial.println("");
      Serial.print("Failed to connect to ");
      Serial.println(ssids[wifi_cell]);
      wifi_cell++;
      if (wifi_cell >= sizeof(ssids)) {
        wifi_cell = 0; // Reset to the first SSID if all have been tried
      }
      Serial.print("Trying next SSID: ");
      Serial.println(ssids[wifi_cell]);

      // Reset the WiFi connection before trying the next SSID
      WiFi.disconnect();
      delay(2000); // Add a small delay before the next connection attempt
    }
  }

  randomSeed(micros());
} */

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssids[wifi_cell]);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssids[wifi_cell], passwords[wifi_cell]);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Convert payload to a string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is "1-shut_down"
  if (message ==  String(randNumber)+"-restart") { 
    // Reset the ESP32-C3
  tmp = "Resetting now...";
    Serial.println(tmp);
    client.publish(debug_topic.c_str(), tmp.c_str());
    esp_restart();
  }

  // Switch on the LED if an 1 was received as first character
  /* if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  } */

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32C3-Client-";
    clientId += String(randNumber, HEX);
    // Attempt to connect
    client.setKeepAlive( 90 ); // setting keep alive to 90 seconds
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(debug_topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//LED control
void ledON() {
  Serial.println("LED ON");
  digitalWrite(LED, LOW);
}

void ledOFF() {
  Serial.println("LED OFF");
  digitalWrite(LED, HIGH);
}
//END LED control

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LED, HIGH);   //turn off led
  randNumber =random(0xffff);// random(256); //0 to 255
  topic = "smart_plants/" + String(randNumber);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  /* Sensor setup */
  dht11.setDelay(delay_readings); // Set this to the desired reading delay. Default is 500ms.

  tmp = "ALL SET!";
  Serial.println(tmp);
  client.publish(debug_topic.c_str(), tmp.c_str());
  /* END Sensor setup */
  
}

void loop() {
  //Serial.println("WORKING");
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > delay_readings) {
    lastMsg = now;
    
    int temperature = 0;
    int humidity = 0;
    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);
    if (result != 0) {
        // Print error message based on the error code.
        Serial.println(DHT11::getErrorString(result));
    }
    
    
    snprintf (msg, MSG_BUFFER_SIZE, "temperature: %ld; humidity: %ld", temperature, humidity);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic.c_str(), msg);
    /* ledON();
    delay(200);
    ledOFF(); */

    

  }
}