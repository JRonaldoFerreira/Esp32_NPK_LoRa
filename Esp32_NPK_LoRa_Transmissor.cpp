#include <SPI.h>
#include <LoRa.h>
#include <Arduino.h>

#define SS    5
#define RST   14
#define DIO0  26

#define BAUD_RATE 4800

#define RS485_RX 16
#define RS485_TX 17

uint8_t byteRequest[8] = {
  0x01, 0x03, 0x00, 0x00,
  0x00, 0x07, 0x04, 0x08
};
uint8_t byteResponse[19];

unsigned long lastLoRaCheck = 0;
const unsigned long loraTimeout = 15000;
unsigned long lastNPKRead = 0;
const unsigned long npkInterval = 1000;
String lastNPKData = "";
unsigned long lastSend = 0;
const unsigned long sendInterval = 1000;

void readNPKSensor();
void resetNPKSensor();

void setup() {
  Serial.begin(115200);

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Falha ao inicializar o LoRa!");
    while (1);
  }
  Serial.println("LoRa iniciado!");

  Serial2.begin(BAUD_RATE, SERIAL_8N1, RS485_RX, RS485_TX);
  Serial.println("Serial2 para sensor NPK inicializada.");

  Serial.println("LoRa + Sensor NPK -> PRONTO (Leitura Rápida)");

  lastLoRaCheck = millis();
}

void loop() {
  if (millis() - lastLoRaCheck > loraTimeout) {
    Serial.println("Atenção: Verificação de 15s - LoRa ainda em operação.");
    lastLoRaCheck = millis();
  }

  if (millis() - lastNPKRead >= npkInterval) {
    readNPKSensor();
    lastNPKRead = millis();
  }

  if (millis() - lastSend >= sendInterval) {
    String msg = "NPK Data: " + lastNPKData + " | TS: " + String(millis());

    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
    Serial.println("Enviado via LoRa: " + msg);

    lastSend = millis();
  }
}

void readNPKSensor() {
  memset(byteResponse, 0, sizeof(byteResponse));

  Serial2.write(byteRequest, sizeof(byteRequest));
  Serial2.flush();

  int bytesRead = 0;
  unsigned long t0 = millis();
  while ((millis() - t0) < 1000 && bytesRead < 19) {
    if (Serial2.available()) {
      byteResponse[bytesRead++] = Serial2.read();
    }
  }

  if (bytesRead == 19) {
    uint16_t soilHumidity    = (byteResponse[3]  << 8) | byteResponse[4];
    uint16_t soilTemperature = (byteResponse[5]  << 8) | byteResponse[6];
    uint16_t soilEC          = (byteResponse[7]  << 8) | byteResponse[8];
    uint16_t soilPH          = (byteResponse[9]  << 8) | byteResponse[10];
    uint16_t nitrogen        = (byteResponse[11] << 8) | byteResponse[12];
    uint16_t phosphorus      = (byteResponse[13] << 8) | byteResponse[14];
    uint16_t potassium       = (byteResponse[15] << 8) | byteResponse[16];

    float hum  = soilHumidity    / 10.0;
    float temp = soilTemperature / 10.0;
    float ph   = soilPH          / 10.0;

    lastNPKData = "Hum=" + String(hum)
                  + ",Temp=" + String(temp)
                  + ",EC=" + String(soilEC)
                  + ",pH=" + String(ph)
                  + ",N=" + String(nitrogen)
                  + ",P=" + String(phosphorus)
                  + ",K=" + String(potassium);

    Serial.println("Leitura NPK OK: " + lastNPKData);

  } else {
    Serial.print("Erro/timeout na leitura NPK. Recebidos ");
    Serial.print(bytesRead);
    Serial.println(" bytes.");

    resetNPKSensor();
    lastNPKData = "Erro na leitura";
  }
}

void resetNPKSensor() {
  Serial.println("Reiniciando o sensor NPK...");
  Serial2.end();
  delay(100);
  Serial2.begin(BAUD_RATE, SERIAL_8N1, RS485_RX, RS485_TX);
  delay(500);
}
