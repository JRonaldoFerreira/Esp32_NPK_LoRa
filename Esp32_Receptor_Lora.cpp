

/************************************************************
 *  Funcao : Recebe dados do LoRa, envia ao display TFT via
 *           Serial2 e atualiza o ThingSpeak via Wi-Fi.
 ************************************************************/



#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SS    5
#define RST   14
#define DIO0  2  
#define SCK   18
#define MISO  19
#define MOSI  23

const char* ssid        = "nome_da_sua_red";
const char* password    = "senha da rede";
String writeAPIKey      = "xxxxxxxxxxxxxxx";
String server           = "api.thingspeak.com";

WiFiClient client;

void setupWiFi();
void sendToThingSpeak(float hum, float temp, int ec, float ph, int n, int p, int k);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  setupWiFi();

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Falha ao inicializar o LoRa!");
    while (1) { }
  }
  Serial.println("LoRa iniciado - Receptor pronto!");
  Serial2.println("Sistema LoRa Iniciado");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.println("Recebido via LoRa: " + received);

    if (received.startsWith("NPK Data:")) {
      String dados = received.substring(received.indexOf(":") + 2);

      int tsIndex = dados.indexOf(" | TS:");
      if (tsIndex > 0) {
        dados = dados.substring(0, tsIndex);
      }

      float humidity    = 0.0;
      float temperature = 0.0;
      float ph          = 0.0;
      int   ec          = 0;
      int   nitrogen    = 0;
      int   phosphorus  = 0;
      int   potassium   = 0;

      int pos = 0;
      while (pos < dados.length()) {
        int nextComma = dados.indexOf(',', pos);
        if (nextComma == -1) nextComma = dados.length();

        String par    = dados.substring(pos, nextComma);
        int equalPos  = par.indexOf('=');

        if (equalPos > 0) {
          String chave = par.substring(0, equalPos);
          String valor = par.substring(equalPos + 1);

          if      (chave == "Hum")   humidity    = valor.toFloat();
          else if (chave == "Temp")  temperature = valor.toFloat();
          else if (chave == "EC")    ec          = valor.toInt();
          else if (chave == "pH")    ph          = valor.toFloat();
          else if (chave == "N")     nitrogen    = valor.toInt();
          else if (chave == "P")     phosphorus  = valor.toInt();
          else if (chave == "K")     potassium   = valor.toInt();
        }
        pos = nextComma + 1;
      }

      Serial.println("\n--- Dados NPK Processados ---");
      Serial.printf("Umidade: %.1f%%\n", humidity);
      Serial.printf("Temperatura: %.1f°C\n", temperature);
      Serial.printf("Condutividade Eletrica: %d\n", ec);
      Serial.printf("pH: %.1f\n", ph);
      Serial.printf("Nitrogenio: %d\n", nitrogen);
      Serial.printf("Fosforo: %d\n", phosphorus);
      Serial.printf("Potassio: %d\n", potassium);
      Serial.println("---------------------------\n");

      Serial2.printf("Umidade: %.1f\n",              humidity);
      Serial2.printf("Temperatura: %.1f\n",          temperature);
      Serial2.printf("Condutividade Eletrica: %d\n", ec);
      Serial2.printf("pH: %.1f\n",                    ph);
      Serial2.printf("Nitrogenio: %d\n",             nitrogen);
      Serial2.printf("Fosforo: %d\n",                phosphorus);
      Serial2.printf("Potassio: %d\n",               potassium);
      Serial2.println("---");

      sendToThingSpeak(humidity, temperature, ec, ph, nitrogen, phosphorus, potassium);
    }
  }

  delay(100);
}

void setupWiFi() {
  Serial.println("Conectando ao Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void sendToThingSpeak(float hum, float temp, int ec, float ph, int n, int p, int k) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    String url = "http://" + server + "/update?api_key=" + writeAPIKey
               + "&field1=" + String(hum)
               + "&field2=" + String(temp)
               + "&field3=" + String(ec)
               + "&field4=" + String(ph)
               + "&field5=" + String(n)
               + "&field6=" + String(p)
               + "&field7=" + String(k);
    
    http.begin(client, url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP Code: " + String(httpCode));
      Serial.println("ThingSpeak retorno: " + payload);
    } else {
      Serial.println("Falha na requisicao ThingSpeak");
    }
    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Nao foi possivel enviar dados.");
  }
}
