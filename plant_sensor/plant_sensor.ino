#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT11.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include "esp_system.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>  //for saving data built in
#include <ArduinoJson.h>

#define soil_moisture_pin 0
#define LED LED_BUILTIN
#define delay_readings 5000

String plant = "mimosa pudica";
String server_c = "https://open.plantbook.io/api/v1/plant/detail/";
/* String apiEndpoint = "/api/v1/plant/detail/mimosa%20pudica/";
String server = "open.plantbook.io";*/
String apiKey = "12fb8eb45d63fd1bba9518ffaac0017a0a15484e"; 

/* Sensors */
DHT11 dht11(10);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

void displaySensorDetails(void) {
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" lux");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" lux");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void configureSensor(void) {
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true); /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  Serial.println("------------------------------------");
  Serial.print("Gain:         ");
  Serial.println("Auto");
  Serial.print("Timing:       ");
  Serial.println("13 ms");
  Serial.println("------------------------------------");
}
/* END Sensors */

/* Wifi */
/* const char* ssid = "Vodafone-50632873";
const char* password = "zemaFKUdaX1."; */
const char *mqtt_server = "mqtt-dashboard.com";

const char *ssids[] = { "Vodafone-50632873", "Vodafone-A83245675", "Vodafone-A83245675-EXT", "Vodafone-CGiulia" };
const char *passwords[] = { "zemaFKUdaX1.", "3qxhfYKxpEYgdtad", "3qxhfYKxpEYgdtad", "Aeria999" };

int randNumber;
String topic;
String debug_topic = "smart_plants_debug";

String tmp;
int wifi_cell = 3;
const int maxTries = 50;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (128)
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

void callback(char *topic, byte *payload, unsigned int length) {
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
  if (message == String(randNumber) + "-restart") {
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
    client.setKeepAlive(90);  // setting keep alive to 90 seconds
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

// LED control
void ledON() {
  Serial.println("LED ON");
  digitalWrite(LED, LOW);
}

void ledOFF() {
  Serial.println("LED OFF");
  digitalWrite(LED, HIGH);
}
void followRedirect(HTTPClient &http) {
  String newLocation = http.header("Location");
  Serial.println("Redirecting to: " + newLocation);

  // Close previous connection
  http.end();

  // Follow the redirect
  WiFiClientSecure client_s;
  client_s.setInsecure();
  if (http.begin(client_s, newLocation)) {
    http.addHeader("Authorization", String("Token " + apiKey));
    int httpCode = http.GET();
    Serial.println("HTTP Code: " + String(httpCode));

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Received payload:");
      Serial.println(payload);
    } else {
      Serial.printf("GET request failed after redirect, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("Unable to connect to redirected URL");
  }
}

void makeGetRequest() {
  WiFiClientSecure client_s;
  client_s.setInsecure();  // Use this only if you don't need SSL verification
  // Alternatively, use client.setCACert() to set a specific root CA certificate.

  HTTPClient http;

  if (http.begin(client_s, server_c)) {
    http.addHeader("Authorization", String("Token " + apiKey));
    //Serial.println("Added header: Authorization: " + String("Token " + apiKey));

    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("Received payload:");
        Serial.println(payload);
        http.end();

        // Parse the JSON response
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("deserializeJson() failed: ");
          Serial.println(error.c_str());
          return;
        }

        // Access and print each element in the JSON response
        const char *pid = doc["pid"];
        const char *display_pid = doc["display_pid"];
        const char *alias = doc["alias"];
        const char *category = doc["category"];
        int max_light_mmol = doc["max_light_mmol"];
        int min_light_mmol = doc["min_light_mmol"];
        int max_light_lux = doc["max_light_lux"];
        int min_light_lux = doc["min_light_lux"];
        int max_temp = doc["max_temp"];
        int min_temp = doc["min_temp"];
        int max_env_humid = doc["max_env_humid"];
        int min_env_humid = doc["min_env_humid"];
        int max_soil_moist = doc["max_soil_moist"];
        int min_soil_moist = doc["min_soil_moist"];
        int max_soil_ec = doc["max_soil_ec"];
        int min_soil_ec = doc["min_soil_ec"];
        const char *image_url = doc["image_url"];

        // Print each element
        Serial.println("Parsed JSON:");
        Serial.println("pid: " + String(pid));
        Serial.println("display_pid: " + String(display_pid));
        Serial.println("alias: " + String(alias));
        Serial.println("category: " + String(category));
        Serial.println("max_light_mmol: " + String(max_light_mmol));
        Serial.println("min_light_mmol: " + String(min_light_mmol));
        Serial.println("max_light_lux: " + String(max_light_lux));
        Serial.println("min_light_lux: " + String(min_light_lux));
        Serial.println("max_temp: " + String(max_temp));
        Serial.println("min_temp: " + String(min_temp));
        Serial.println("max_env_humid: " + String(max_env_humid));
        Serial.println("min_env_humid: " + String(min_env_humid));
        Serial.println("max_soil_moist: " + String(max_soil_moist));
        Serial.println("min_soil_moist: " + String(min_soil_moist));
        Serial.println("max_soil_ec: " + String(max_soil_ec));
        Serial.println("min_soil_ec: " + String(min_soil_ec));
        Serial.println("image_url: " + String(image_url));
      } else if (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND) {
        if (http.header("Location") != NULL)
          followRedirect(http);
        else
          Serial.println(String(httpCode) + " No redirect header");
      } else {
        Serial.println("HTTP Code: " + String(httpCode));
      }
    } else {
      Serial.printf("GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  } else {
    Serial.println("Unable to connect");
  }
}


/* void makeGetRequest() {
  WiFiClientSecure client_s;
  client_s.setInsecure(); // Use this only if you don't need SSL verification
  // Alternatively, use client.setCACert() to set a specific root CA certificate.

  if (client_s.connect(server.c_str(), 443)) { // 443 is the default port for HTTPS
    // Construct the GET request
    String getRequest = String("GET ") + apiEndpoint + " HTTP/1.1\r\n" +
                        "Authorization: Token " + apiKey + "\r\n"+
                        "Host:  "+server+"\r\n\r\n";

    // Print the raw GET request to Serial
    Serial.println("Raw GET request:");
    Serial.println(getRequest);

    // Send the GET request
    client_s.print(getRequest);

    // Wait for response and print it
    while (client_s.connected() || client_s.available()) {
      if (client_s.available()) {
        String line = client_s.readStringUntil('\n');
        Serial.println(line);
      }
    }
    
    client_s.stop();
  } else {
    Serial.println("Unable to connect");
  }
}
 */
void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);         // Initialize the BUILTIN_LED pin as an output
  digitalWrite(LED, HIGH);      // turn off led
  randNumber = random(0xffff);  // random(256); //0 to 255
  topic = "smart_plants/" + String(randNumber);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  /* DB */
  plant.replace(" ", "%20");
  server_c += plant + "/";
  Serial.println(server_c);

  makeGetRequest();
  /* END DB */

  /* Sensor setup */
  dht11.setDelay(delay_readings);  // Set this to the desired reading delay. Default is 500ms.
  if (!tsl.begin()) {
    /* There was a problem detecting the TSL2561 ... check your connections */
    String err_tsl = "Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!";
    Serial.print(err_tsl);
    client.publish(debug_topic.c_str(), err_tsl.c_str());
    while (1)
      ;
  }
  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();

  tmp = "ALL SET!";
  Serial.println(tmp);
  client.publish(debug_topic.c_str(), tmp.c_str());
  /* END Sensor setup */
}

void loop() {

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

    /* Get a new sensor event */
    sensors_event_t event;
    tsl.getEvent(&event);
    /* Display the results (light is measured in lux) */
    if (!event.light) {
      /* If event.light = 0 lux the sensor is probably saturated
        and no reliable data could be generated! */
      Serial.println("TS2561 overload");
    }
    String light_sensor = String(event.light);
    snprintf(msg, MSG_BUFFER_SIZE, "soil_moisture: %ld; temperature: %ld; humidity: %ld; light: %.2f", analogRead(soil_moisture_pin), temperature, humidity, event.light);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic.c_str(), msg);
    /* ledON();
    delay(200);
    ledOFF(); */
    // Serial.println(analogRead(soil_moisture_pin)); //print soil mooisture
  }
}
