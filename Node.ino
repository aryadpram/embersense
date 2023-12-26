#include <SPI.h>
#include <LoRa.h>

// Pin Definitions
constexpr int SS_PIN = 15;
constexpr int RST_PIN = 16;
constexpr int DIO0_PIN = 2;

// LoRa Parameters
constexpr long FREQUENCY = 433E6;
constexpr long BANDWIDTH = 40.7E3;
constexpr int SPREADING_FACTOR = 12;
constexpr int TX_POWER = 20;

int counter = 0;
const char* sensorID = "sglc0003";

char packetData[18]; // Adjust the size accordingly

void setupLoRa() {
  Serial.println("LoRa Sender");
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  if (!LoRa.begin(FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSignalBandwidth(BANDWIDTH);
  LoRa.setSpreadingFactor(SPREADING_FACTOR);
  LoRa.setTxPower(TX_POWER);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x12);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // Wait for the serial monitor to open
  }
  setupLoRa();
}

void sendPacket() {
  sprintf(packetData, "%s,%d", sensorID, counter);
  
  LoRa.beginPacket();
  LoRa.print(packetData);
  LoRa.endPacket();
  
  Serial.print("Sending packet: ");
  Serial.println(packetData);
  
  counter++;
}

void loop() {
  sendPacket();
  //delay(1000);
}
