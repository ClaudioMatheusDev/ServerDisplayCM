# ServerDisplayCM

ServerDisplayCM é um projeto pessoal que une monitoramento de hardware, desenvolvimento .NET e eletrônica embarcada para criar um display físico com informações em tempo real do computador.

A ideia é simples: o agente rodando no PC coleta dados de CPU, GPU, memória e temperatura, envia esses dados para uma API local e o firmware em um display TFT exibe tudo de forma clara e visualmente organizada.

## O que o projeto faz

- Monitora uso da CPU, GPU e memória
- Exibe temperaturas de hardware
- Mostra o estado da máquina em um display físico
- Usa uma arquitetura simples com backend local e firmware embarcado
- Possui telas para conexão Wi-Fi, estado offline e dashboard de monitoramento

## Arquitetura

O projeto é dividido em dois pilares principais:

- ServerDisplayCM.Agent: aplicação ASP.NET Core que coleta métricas do hardware com a biblioteca LibreHardwareMonitor
- ServerDisplayCM.Firmware: sketch para ESP32/Arduino com TFT_eSPI e ArduinoJson para renderizar a interface no display

## Tecnologias utilizadas

- .NET 10
- ASP.NET Core
- LibreHardwareMonitor
- ArduinoJson
- TFT_eSPI
- Wi-Fi + HTTP

## Estrutura do repositório

- [ServerDisplayCM.Agent](ServerDisplayCM.Agent) - API responsável por coletar os dados do computador
- [ServerDisplayCM.Firmware](ServerDisplayCM.Firmware) - Firmware responsável por exibir os dados no display
- [ServerDisplayCM.sln](ServerDisplayCM.sln) - solução principal do projeto

## Como funciona

1. O agente roda no computador e lê os sensores de hardware
2. A API expõe os dados em um endpoint local
3. O firmware conecta-se ao Wi-Fi e consulta a API
4. O display mostra o status da máquina em tempo real

## Pré-requisitos

- .NET SDK 10
- Arduino IDE ou ambiente compatível com ESP32
- Display TFT compatível com TFT_eSPI
- Rede Wi-Fi local

## Como executar

### 1. Rodar o agente

```bash
dotnet run --project ServerDisplayCM.Agent/ServerDisplayCM.Agent.csproj
```

A API ficará disponível em:

```text
http://0.0.0.0:5050/api/monitor
```

### 2. Configurar o firmware

No arquivo [ServerDisplayCM.Firmware/ServerDisplayCM.Firmware.ino](ServerDisplayCM.Firmware/ServerDisplayCM.Firmware.ino), ajuste:

- SSID e senha do Wi-Fi
- URL da API local

### 3. Fazer upload do firmware

Abra o sketch no Arduino IDE/VS Code e faça o upload para o microcontrolador.

## Próximos passos

- Melhorias na interface do display
- Suporte a mais métricas e indicadores
- Histórico de uso
- Modo de economia de energia
- Integração com nuvem ou automações

## Observação

Este projeto foi desenvolvido como um estudo prático de integração entre backend .NET, hardware e IoT, com foco em criar uma experiência visual e funcional para monitoramento local.

