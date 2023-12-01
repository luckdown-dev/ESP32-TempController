#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

// credencias do ponto de acesso
const char* ssid = "ESP32";
const char* passwd = "pp12345678";

void setup(){
  Serial.begin(115200);

  WiFiManager wmanager;

  wmanager.resetSettings();

  if(!wmanager.autoConnect(ssid, passwd)){
    Serial.println("Falha na conexão");
    ESP.restart();

  } else {
    Serial.println("Conectado com sucesso!");
    Serial.print("Acesse por: ");
    Serial.print(ssid);
    Serial.println(".local");
  }

  if (!MDNS.begin("ssid")) {
      Serial.println("Erro ao iniciar o serviço mDNS!");
      while(1) {
        delay(1000);
      }
  }

  Serial.println("Serviço mDNS iniciado!");

  // Start TCP (HTTP) server
  //server.begin();
  Serial.println("Servidor TCP iniciado!");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

}

void loop(){



}
