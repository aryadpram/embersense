#include <SPI.h>
#include <LoRa.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

// Pin Definitions
constexpr int SS_PIN = 15;
constexpr int RST_PIN = 16;
constexpr int DIO0_PIN = 2;

// LoRa Parameters
constexpr long FREQUENCY = 433E6;
constexpr long BANDWIDTH = 41.7E3;
constexpr int SPREADING_FACTOR = 12;    

// MQTT Parameters
const char* mqttServer = "f9245585683d4a0eb7352d37d63c080a.s1.eu.hivemq.cloud";
int port = 8883;
const char* mqttClientId = "ESP8266Client";
const char* mqttTopic = "dteti-embersense-c12/sensor";

WiFiManager wm;
WiFiClientSecure espClient;
PubSubClient client(espClient);



void setupLoRa() {
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("Error starting LoRa!");
    return;
  }

  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x12);
}

void setup() {
  wm.autoConnect("Gateway SGLC", "");

  espClient.setInsecure();
  client.setServer(mqttServer, port);

  Serial.begin(9600);

  setupLoRa();
}

void reconnect() {
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    if (client.connect(mqttClientId, "aryadpram", "e!ZeHq3JYnFdDbJ")) {
      //Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in a second");
      delay(1);
    }
  }
}

void processLoRaPacket() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    char data[packetSize + 1];
    for (int i = 0; i < packetSize; i++) {
      data[i] = (char)LoRa.read();
    }
    data[packetSize] = '\0'; // Null-terminate the string

    Serial.print("Received packet '");
    Serial.println(data);

    if (!client.connected()) {
      reconnect();
    }

    bool success = client.publish(mqttTopic, data);
    if (success) {
      Serial.println("mqtt sent");
    } else {
      Serial.println("mqtt not sent");
    }
    client.loop();
  }
}

void loop() {
  processLoRaPacket();
}
