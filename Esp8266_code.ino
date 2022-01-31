#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHT11PIN 4 //on the NodeMCU board it is the D2 PIN! 

#define DHTTYPE DHT11
DHT dht(DHT11PIN, DHTTYPE);

// Update these with values suitable for your network.

const char* ssid = "..........."; //Wifi SSID
const char* password = "............"; //WIFI password
const char* mqtt_server = "....................."; //MQTT Broker address
const int mqtt_port = 3024; //MQTT broker port

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
  Serial.print("]: ");
  char message[length];
  for (int i=0; i< length; i++){
    message[i] = (char)payload[i];
  }
  Serial.println(message);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected!");
     
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds old.
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }else{   
    /*
     * Publish the measured data:
     * temperature [°C] -> sashank/test/dht11/temp_celsius
     * temperature [°K] -> sashank/test/dht11/temp_fahrenheit
     * humidity [%] -> sashank/test/dht11/humidity
     */
    client.publish("sashank/test/dht11/temp_celsius",String(t).c_str());
    client.publish("sashank/test/dht11/temp_fahrenheit",String(f).c_str());
    client.publish("sashank/test/dht11/humidity",String(h).c_str());  
  }

}
