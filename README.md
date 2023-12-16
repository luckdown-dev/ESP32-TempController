# Controle de Temperatura com ESP32

Este é um projeto de controle de temperatura utilizando o ESP32. O sistema permite definir uma temperatura alvo e controlar um circuito para manter a temperatura próxima ao valor desejado.

## Funcionalidades

- Configuração fácil do dispositivo como ponto de acesso (AP).
- Interface web responsiva para definir a temperatura alvo.
- Atualização em tempo real da temperatura atual e barra de progresso.
- Controle do circuito para ligar/desligar com base na temperatura alvo.
- Utilização de WebSocket para comunicação assíncrona.

## Componentes

- ESP32 para controle e monitoramento.
- Sensor de temperatura DS18B20 para medição da temperatura.
- Relé para controlar o circuito.

## Configuração

1. Conecte o sensor DS18B20 ao pino 32 do ESP32.
2. Conecte o relé ao pino 25 do ESP32.
3. Substitua as credenciais do ponto de acesso (AP) conforme necessário.

## Instalação

1. Carregue o código para o ESP32 utilizando a Arduino IDE.
2. Abra o Monitor Serial para visualizar as mensagens e obter o endereço IP do ESP32.
3. Acesse o ESP32 através do navegador usando o endereço IP obtido.
4. Configure a temperatura alvo e clique em "Registrar Temperatura".
5. O botão "Ligar Circuito" será habilitado. Use-o para controlar o circuito.

## Estrutura do Código

- **Web Server:** Utiliza o `AsyncWebServer` para servir a interface web.
- **WebSocket:** Configura um WebSocket para comunicação em tempo real.
- **Sensor de Temperatura:** Utiliza o sensor DS18B20 para medir a temperatura.
- **Controle do Circuito:** Liga/desliga o relé com base na temperatura alvo.

## Recursos Web

- **/ (Raiz):** Página inicial com a interface gráfica.
- **WebSocket (/ws):** Comunicação assíncrona para atualização em tempo real.

## Observações

- A temperatura alvo deve estar entre 45.0°C e 100.0°C.
- A barra de progresso indica a proximidade da temperatura alvo.

## Licença

© 2023 Amaral&Becker - TRO7M. Todos os direitos reservados.
