#define soil_moisture_pin 0

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println(analogRead(soil_moisture_pin));
  delay(500);
}
