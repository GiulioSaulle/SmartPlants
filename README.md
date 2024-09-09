<h1>SmartPlants 🌱</h1>
<p><strong>SmartPlants</strong> is an Arduino-based IoT project designed to monitor environmental conditions for
   plants. It tracks temperature, humidity, and soil moisture and sends this data via MQTT to an IoT platform,
   allowing for remote plant care monitoring and automation.
</p>
<h2>Idea</h2>
<p>This project was created to help people in managing their balcony/vegetable
   garden, being able to have a smart dashboard in which plant parameters such
   as temperature, humidity, soil moisture, and brightness will be showed in a
   user-friendly interface. The primary goal of this system is to streamline and au-
   tomate plant care, providing users with an effortless way to monitor and manage
   their plants’ health. This intelligent approach not only enhances plant care but
   also reduces the time and effort needed for maintenance, allowing users to enjoy
   healthier plants with minimal involvement.
</p>
<h2>Features</h2>
<ul>
   <li><strong>Temperature & Humidity Monitoring</strong>: Monitors environmental temperature and humidity
      levels.
   </li>
   <li><strong>Soil Moisture Sensing</strong>: Tracks the moisture level in the soil to determine if the plant
      needs water.
   </li>
   <li><strong>MQTT Communication</strong>: Sends sensor data to an MQTT broker, enabling real-time monitoring from
      a remote IoT platform.
   </li>
   <li><strong>Low Power Consumption</strong>: Efficient power management for extended usage.</li>
</ul>
<h2>Components</h2>
<ul>
   <li><strong>Arduino Board</strong>: The main microcontroller used for the project (e.g., Arduino Uno, Nano, or
      similar).
   </li>
   <li><strong>DHT11/DHT22 Sensor</strong>: Measures temperature and humidity.</li>
   <li><strong>Soil Moisture Sensor</strong>: Monitors the moisture level of the soil.</li>
   <li><strong>TSL2561 Brightness Sensor</strong>:Monitor the amount of light.</li>
   <li><strong>ESP8266/ESP32 Module</strong>: Connects to Wi-Fi and sends data via MQTT.</li>
   <li><strong>LCD Display</strong>: Displays real-time data such as temperature, humidity, and soil
      moisture levels.
   </li>
</ul>
<h2>Circuit Diagram</h2>
<h4>Plant Brain</h4>
<img src="https://github.com/gabriele2romano/SmartPlants/blob/main/schematic/plant_brain.png?raw=true" alt="Schematic Plant Brain">
<p><em>Circuit diagram to visually represent the plant brains.</em></p>
<h4>Plant Sensor</h4>
<img src="https://github.com/gabriele2romano/SmartPlants/blob/main/schematic/plant_sensor.png?raw=true" alt="Schematic Plant Brain">
<p><em>Circuit diagram to visually represent the plant sensor.</em></p>
<h2>How to Install</h2>
<ol>
   <li>
      <strong>Clone the Repository:</strong>
      <pre><code>git clone https://github.com/gabriele2romano/SmartPlants.git
cd SmartPlants</code></pre>
   </li>
   <li>
      <strong>Upload Code to Arduino:</strong>
      <p>Open the <code>SmartPlants.ino</code> file in the Arduino IDE, select your Arduino board, and upload the
         code.
      </p>
   </li>
   <li>
      <strong>Install Necessary Libraries:</strong>
      <p>Install the following libraries in the Arduino IDE:</p>
      <ul>
         <li><code>PubSubClient</code> (for MQTT communication)</li>
         <li><code>ESP8266WiFi</code> or <code>WiFi</code> library (depending on your ESP module)</li>
         <li><code>DHT Sensor Library</code> by Adafruit</li>
         <li><code>Adafruit Unified Sensor</code></li>
      </ul>
      <p>You can install them via the Arduino Library Manager (<em>Sketch</em> -> <em>Include Library</em> ->
         <em>Manage Libraries...</em>).
      </p>
   </li>
   <li>
      <strong>Assemble the Hardware:</strong>
      <p>Connect the sensors, Wi-Fi module, and other components according to the circuit diagram.</p>
   </li>
   <li>
      <strong>Configure Wifi Settings:</strong>
      <p>Modify the following settings in the code to match your Wifi details:</p>
      <pre><code>const char *ssids[] = { "NetName1", "NetName2" };
const char *passwords[] = { "NetPass1", "NetPass2" };</code></pre>
   </li>
   <li>
      <strong>Configure MQTT Settings:</strong>
      <p>Modify the following settings in the code to match your MQTT broker's details:</p>
      <pre><code>const char* mqtt_server = "your_mqtt_broker_address";
const char* mqtt_user = "your_mqtt_username";
const char* mqtt_password = "your_mqtt_password";
const char* mqtt_topic = "your_mqtt_topic";</code></pre>
   </li>
   <li>
      <strong>Test the Project:</strong>
      <p>Once the hardware is assembled and the code is uploaded, open the Serial Monitor in the Arduino IDE to
         view sensor readings and MQTT connection status. You can monitor the data being sent to your MQTT broker
         from an IoT dashboard like <strong>Node-RED</strong>, <strong>Home Assistant</strong>, or
         <strong>Adafruit IO</strong>.
      </p>
   </li>
</ol>
<h2>MQTT Communication</h2>
<p>The data collected by the sensors is sent via MQTT to a specified broker. This allows you to monitor the
   environmental conditions in real-time from any compatible MQTT client.
</p>
<p>You can visualize the data or trigger automated responses using tools such as:</p>
<ul>
   <li><strong>Node-RED</strong></li>
   <li><strong>Home Assistant</strong></li>
   <li><strong>Adafruit IO</strong></li>
   <li><strong>MQTT.fx</strong> (for debugging)</li>
</ul>
<!-- <h2>License</h2>
<p>This project is licensed under the MIT License. See the <a href="LICENSE">LICENSE</a> file for more details.</p> -->
<h2>Contributions</h2>
<p>
  Gabriele Romano, Giulio Saulle
  From Politecnico of Milan
</p>
