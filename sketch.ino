#include <WiFi.h>
#include <mDNS.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

// credencias do ponto de acesso
const char* ssid = "esp32";
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
  }

}

void loop(){



}
