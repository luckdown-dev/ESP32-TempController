#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>

#define rele 25

// credenciais do AP
const char *ssid = "ESP32";
// const char* passwd = "pp12345678";

float temperaturaAlvo = 25.0;
// valor inserido para simular o sensor
float temperaturaAtual = 10.0;
bool circuitoLigado = false;
bool temperaturaAlvoAtingida = false;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Função para calcular o valor da barra de progresso com base na temperatura atual e na temperatura alvo
int calcularProgresso(float temperaturaAlvo, float temperaturaAtual)
{
  // Calcular a proporção entre a temperatura atual e a temperatura alvo
  float progresso = map(constrain(temperaturaAtual, 10.0, temperaturaAlvo), 10.0, temperaturaAlvo, 0, 100);

  return static_cast<int>(progresso);
}

void setup()
{
  Serial.begin(115200);

  pinMode(rele, OUTPUT);

  WiFiManager wm;

  if (wm.autoConnect(ssid))
  {
    Serial.println("Sucesso ao conectar no AP!");
  }
  else
  {
    Serial.println("Falha ao conectar no AP");
  }

  // Serviço mDNS
  if (!MDNS.begin("esp32"))
  {
    Serial.println("Erro ao iniciar o mDNS");
    return;
  }
  else
  {
    Serial.println("Sucesso ao iniciar o mDNS!");
  }

  MDNS.addService("http", "tcp", 80);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String html = R"html(
      <!DOCTYPE html>
      <html lang="pt-br">
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Controle de Temperatura</title>
        <style>
          body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            margin: 0;
            padding: 0;
          }

          .container {
            width: 300px;
            margin: auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            text-align: center;
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
          }

          .input-field {
            width: auto;
            padding: 10px;
            margin: 10px 0;
            border: 1px solid #ccc;
            border-radius: 5px;
            text-align: center;
            font-size: 18px;
          }

          .legend {
            font-size: 13px;
            color: #666;
            margin-bottom: 10px;
          }

          .button {
            background-color: #3498db;
            color: #fff;
            border: none;
            padding: 10px 20px;
            margin: 10px 0;
            border-radius: 5px;
            cursor: pointer;
          }

          .button:hover {
            background-color: #2980b9;
          }

          .status-field {
            font-size: 16px;
            margin: 20px 0;
            color: #333;
          }

          .progress-bar-container {
            width: 100%;
            background-color: #e0e0e0;
            border-radius: 5px;
          }

          .progress-bar {
            width: 0%;
            height: 20px;
            background-color: #3498db;
            border-radius: 5px;
            transition: width 0.5s ease;
          }
        </style>
      </head>
      <body>
        <div class="container">
          <p class="legend">Insira a temperatura alvo entre 10.0°C e 70.0°C</p>
          <input type="text" id="tempInput" class="input-field" placeholder="Temperatura alvo" value="25.0"><br>
          <p id="tempAlvo" class="status-field">Temperatura Alvo: <span id="tempAlvoValor">25.0</span>°C</p>
          <button id="registrarBtn" class="button" onclick="registrarTemperatura()">Registrar Temperatura</button><br>
          <button id="ligaDesligaBtn" class="button" onclick="ligaDesligaCircuito()">Ligar Circuito</button><br>
          <p id="temperaturaAtual" class="status-field">Temperatura Atual: <span id="tempAtualValor">20.0</span>℃</p>
          <button id="reiniciarBtn" class="button" onclick="reiniciarInterface()">Reiniciar</button><br>
          <br>
          <br>
          <div class="progress-bar-container">
            <div id="progressBar" class="progress-bar"></div>
          </div>
        </div>
        <script>
          function registrarTemperatura() {
            var tempInput = document.getElementById('tempInput');
            var tempAlvo = document.getElementById('tempAlvoValor');
            tempAlvo.innerText = tempInput.value;
            tempInput.disabled = true;

            // Atualizar a variável temperaturaAlvo no servidor
            socket.send(JSON.stringify({ action: 'atualizar_alvo', temperatura: tempInput.value }));
          }

          function handleTemperaturaAlvoAtingida() {
            alert("Temperatura alvo atingida!");
            var tempInput = document.getElementById('tempInput');
            tempInput.disabled = false;

            var ligaDesligaBtn = document.getElementById('ligaDesligaBtn');
            ligaDesligaBtn.innerHTML = 'Ligar Circuito';
            ligaDesligaBtn.style.backgroundColor = '#3498db';
          }

          function ligaDesligaCircuito() {
            var ligaDesligaBtn = document.getElementById('ligaDesligaBtn');
            if (ligaDesligaBtn.innerHTML === 'Ligar Circuito') {
              ligaDesligaBtn.innerHTML = 'Desligar Circuito';
              ligaDesligaBtn.style.backgroundColor = '#e74c3c';
              // Enviar mensagem WebSocket para ligar o circuito
              socket.send(JSON.stringify({ action: 'ON' }));
              console.log('Enviou a mensagem para ligar o circuito');
            } else {
              ligaDesligaBtn.innerHTML = 'Ligar Circuito';
              ligaDesligaBtn.style.backgroundColor = '#3498db';
              // Enviar mensagem WebSocket para desligar o circuito
              socket.send(JSON.stringify({ action: 'OFF' }));
              console.log('Enviou a mensagem para desligar o circuito');
            }
          }

          function updateTemperatura(temperatura) {
            document.getElementById('tempAtualValor').innerText = temperatura;
          }

          function updateProgressBar(progresso) {
            var progressBar = document.getElementById('progressBar');
            progressBar.style.width = progresso + '%';
          }

          function reiniciarInterface() {
            // Limpar valores e reativar campos
            var tempInput = document.getElementById('tempInput');
            tempInput.value = '25.0';
            tempInput.disabled = false;

            var tempAlvo = document.getElementById('tempAlvoValor');
            tempAlvo.innerText = '25.0';

            var ligaDesligaBtn = document.getElementById('ligaDesligaBtn');
            ligaDesligaBtn.innerHTML = 'Ligar Circuito';
            ligaDesligaBtn.style.backgroundColor = '#3498db';

            // Reiniciar a barra de progresso
            updateProgressBar(0);

            // Enviar mensagem WebSocket para reiniciar no servidor
            socket.send(JSON.stringify({ action: 'RESTART' }));
          }

          // WebSocket
          var socket = new WebSocket('ws://' + window.location.hostname + '/ws');
          socket.onmessage = function (event) {
            var data = JSON.parse(event.data);
            updateTemperatura(data.temperatura);
            updateProgressBar(data.progresso);

            // Verificar se a temperatura alvo foi atingida
            if (data.temperatura >= parseFloat(document.getElementById('tempInput').value)) {
              handleTemperaturaAlvoAtingida();
            }
          };
        </script>
      </body>
      </html>
    )html";


    request->send(200, "text/html", html); });

  // Configurar WebSocket
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
             {
    if (type == WS_EVT_CONNECT)
    {
      Serial.println("WebSocket client connected");
    }
    else if (type == WS_EVT_DISCONNECT)
    {
      Serial.println("WebSocket client disconnected");
    }
    else if (type == WS_EVT_DATA)
    {
      AwsFrameInfo *info = (AwsFrameInfo *)arg;
      if (info->opcode == WS_TEXT)
      {
        // Interpretar a mensagem WebSocket
        String message = String((char *)data);
        Serial.println("Mensagem WebSocket recebida: " + message);

        // Verificar se a mensagem contém "ligar", "desligar" ou "atualizar_alvo"
        if (message.indexOf("ON") != -1)
        {
          Serial.println("Recebeu a mensagem para ligar o circuito");
          circuitoLigado = true;
        }
        else if (message.indexOf("OFF") != -1)
        {
          Serial.println("Recebeu a mensagem para desligar o circuito");
          circuitoLigado = false;
        }
        else if (message.indexOf("atualizar_alvo") != -1)
        {
          // Encontrar a posição da vírgula e extrair o valor da temperatura alvo
          int pos = message.indexOf("temperatura") + 13;
          int posFim = message.indexOf("}");
          String novoAlvoStr = message.substring(pos, posFim);

          // Remover as aspas duplas
          novoAlvoStr.replace("\"", "");

          // Converter a string para um número de ponto flutuante
          float novoAlvo = novoAlvoStr.toFloat();
          Serial.print("Nova temperatura alvo: ");
          Serial.println(novoAlvo);

          // Atualizar a variável temperaturaAlvo
          temperaturaAlvo = novoAlvo;
        }
      }
    } });

  // Iniciar o servidor
  server.addHandler(&ws);
  server.begin();
}

void loop()
{

  // implementar a atualização do sensor e do gráfico utilizando millis

  if (circuitoLigado && !temperaturaAlvoAtingida && temperaturaAtual < temperaturaAlvo)
  {
    // Simulação de leitura do sensor de temperatura
    temperaturaAtual += 0.1;

    // Aciona o RELE
    digitalWrite(rele, 1);

    // Atualizar a barra de progresso e notificar clientes WebSocket
    Serial.println(temperaturaAlvo);
    Serial.println(temperaturaAtual);
    int progresso = calcularProgresso(temperaturaAlvo, temperaturaAtual);
    Serial.println(progresso);
    ws.textAll("{\"temperatura\":" + String(temperaturaAtual) + ", \"progresso\":" + String(progresso) + "}");

    delay(1000);

    // Verificar se a temperatura alvo foi atingida
    if (temperaturaAtual >= temperaturaAlvo)
    {
      temperaturaAlvoAtingida = true;

      // Exibir mensagem no console (pode ser substituído por uma lógica para exibir no HTML)
      Serial.println("Temperatura alvo atingida!");
      delay(500);
      temperaturaAlvoAtingida = false;
    }
  }
  else
  {
    digitalWrite(rele, 0);
  }
}
