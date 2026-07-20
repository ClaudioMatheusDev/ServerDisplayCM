#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>

const char* WIFI_SSID = "NOME_DO_WIFI";
const char* WIFI_PASSWORD = "SENHA_DO_WIFI";

const char* API_URL =
    "http://<IP_ADDRESS>:5050/api/monitor";

TFT_eSPI tft = TFT_eSPI();

unsigned long ultimaConsulta = 0;
const unsigned long intervaloConsulta = 2000;

enum class TelaAtual
{
    Inicial,
    Conectando,
    Dashboard,
    Offline
};

TelaAtual telaAtual = TelaAtual::Inicial;
bool dashboardEstruturaMontada = false;
String ultimoNomeMaquina = "";

// ---------------------------------------------------------
// Utilidades de desenho
// ---------------------------------------------------------

uint16_t corPorPercentual(float pct)
{
    if (pct >= 85.0f) return TFT_RED;
    if (pct >= 60.0f) return TFT_YELLOW;
    return TFT_GREEN;
}

String extrairHora(const String& iso)
{
    int posT = iso.indexOf('T');
    if (posT == -1 || iso.length() < posT + 9)
    {
        return "--:--:--";
    }

    return iso.substring(posT + 1, posT + 9);
}

String limitarTexto(const String& texto, size_t maxLength)
{
    if (texto.length() <= maxLength)
    {
        return texto;
    }

    if (maxLength <= 3)
    {
        return texto.substring(0, maxLength);
    }

    return texto.substring(0, maxLength - 3) + "...";
}

void drawHeader(const String& titulo, uint16_t cor)
{
    tft.fillRect(0, 0, 320, 28, TFT_NAVY);
    tft.drawFastHLine(0, 27, 320, cor);

    tft.setTextColor(cor, TFT_NAVY);
    tft.drawString(titulo, 10, 8, 2);
}

void desenharCartao(int x, int y, int w, int h, uint16_t borda)
{
    tft.fillRoundRect(x, y, w, h, 4, TFT_BLACK);
    tft.drawRoundRect(x, y, w, h, 4, borda);
}

void limparAreaTexto(int x, int y, int w, int h)
{
    tft.fillRect(x, y, w, h, TFT_BLACK);
}

void desenharProgresso(int x, int y, int w, int h, float value, uint16_t cor)
{
    tft.fillRoundRect(x, y, w, h, 2, TFT_DARKGREY);

    int preenchido = static_cast<int>((value / 100.0f) * w);
    if (preenchido < 0)
    {
        preenchido = 0;
    }
    if (preenchido > w)
    {
        preenchido = w;
    }

    if (preenchido > 0)
    {
        tft.fillRoundRect(x, y, preenchido, h, 2, cor);
    }
}

void desenharEstruturaConectando()
{
    if (telaAtual == TelaAtual::Conectando)
    {
        return;
    }

    tft.fillScreen(TFT_BLACK);
    drawHeader("Conectando ao Wi-Fi", TFT_YELLOW);

    tft.fillRoundRect(34, 78, 252, 76, 10, TFT_DARKGREY);
    tft.drawRoundRect(34, 78, 252, 76, 10, TFT_YELLOW);

    tft.setTextColor(TFT_YELLOW, TFT_DARKGREY);
    tft.drawString("Aguardando rede Wi-Fi", 46, 98, 2);

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("Tentando conectar...", 58, 122, 2);

    telaAtual = TelaAtual::Conectando;
    dashboardEstruturaMontada = false;
}

void desenharEstruturaOffline()
{
    if (telaAtual == TelaAtual::Offline)
    {
        return;
    }

    tft.fillScreen(TFT_BLACK);
    drawHeader("Sem resposta", TFT_RED);

    tft.fillRoundRect(30, 60, 260, 100, 10, TFT_DARKGREY);
    tft.drawRoundRect(30, 60, 260, 100, 10, TFT_RED);

    tft.setTextColor(TFT_RED, TFT_DARKGREY);
    tft.drawString("PC OFFLINE", 76, 90, 4);

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("Verifique a API e a rede", 56, 130, 2);

    telaAtual = TelaAtual::Offline;
    dashboardEstruturaMontada = false;
}

void desenharEstruturaDashboardBase(const String& machineName)
{
    tft.fillScreen(TFT_BLACK);
    drawHeader(machineName, TFT_CYAN);

    desenharCartao(10, 32, 145, 74, TFT_CYAN);
    desenharCartao(165, 32, 145, 74, TFT_MAGENTA);
    desenharCartao(10, 112, 300, 64, TFT_GREEN);

    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("CPU", 18, 38, 1);
    tft.drawString("GPU", 173, 38, 1);
    tft.drawString("MEMORIA", 18, 118, 1);

    tft.fillRoundRect(12, 220, 296, 16, 4, TFT_NAVY);
    tft.drawRoundRect(12, 220, 296, 16, 4, TFT_DARKCYAN);

    telaAtual = TelaAtual::Dashboard;
    dashboardEstruturaMontada = true;
    ultimoNomeMaquina = machineName;
}

void atualizarCabecalhoDashboard(const String& syncLabel)
{
    tft.fillRect(236, 6, 72, 18, TFT_NAVY);
    tft.setTextColor(TFT_LIGHTGREY, TFT_NAVY);
    tft.drawString(limitarTexto(syncLabel, 10), 244, 8, 1);

    tft.fillCircle(292, 14, 4, TFT_GREEN);
}

void atualizarDashboard(
    const String& machineName,
    const String& cpuName,
    float cpuUsage,
    bool cpuTempValida,
    float cpuTemperature,
    float memUsedGb,
    float memTotalGb,
    float memoryUsage,
    const String& gpuName,
    float gpuUsage,
    bool gpuTempValida,
    float gpuTemperature,
    const String& horaColeta)
{
    String syncLabel = "Sync " + horaColeta;
    if (!dashboardEstruturaMontada || ultimoNomeMaquina != machineName)
    {
        desenharEstruturaDashboardBase(machineName);
    }

    atualizarCabecalhoDashboard(syncLabel);

    String nomeCpuExibido = limitarTexto(cpuName, 16);
    String nomeGpuExibido = limitarTexto(gpuName, 24);

    // Nome da CPU (substitui o rotulo "CPU" da estrutura base)
    limparAreaTexto(18, 38, 128, 10);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString(nomeCpuExibido, 18, 38, 1);

    // Percentual de uso da CPU (fonte 4 ~26px de altura)
    limparAreaTexto(18, 54, 110, 26);
    tft.setTextColor(corPorPercentual(cpuUsage), TFT_BLACK);
    tft.drawString(String(cpuUsage, 1) + "%", 18, 54, 4);

    limparAreaTexto(18, 88, 120, 10);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString(
        cpuTempValida ? (String(cpuTemperature, 1) + " C") : "temp N/D",
        18,
        88,
        1);

    desenharProgresso(18, 98, 124, 6, cpuUsage, corPorPercentual(cpuUsage));

    // Nome da GPU (substitui o rotulo "GPU" da estrutura base)
    limparAreaTexto(173, 38, 124, 10);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString(nomeGpuExibido, 173, 38, 1);

    limparAreaTexto(173, 54, 110, 26);
    tft.setTextColor(corPorPercentual(gpuUsage), TFT_BLACK);
    tft.drawString(String(gpuUsage, 1) + "%", 173, 54, 4);

    limparAreaTexto(173, 88, 124, 10);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString(
        gpuTempValida ? (String(gpuTemperature, 1) + " C") : "temp N/D",
        173,
        88,
        1);

    desenharProgresso(173, 98, 124, 6, gpuUsage, corPorPercentual(gpuUsage));

    // --- Memoria: percentual, detalhe em GB e barra, sem sobrepor ---
    limparAreaTexto(18, 128, 130, 26);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString(String(memoryUsage, 1) + "%", 18, 128, 4);

    limparAreaTexto(150, 132, 160, 14);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString(
        String(memUsedGb, 1) + " / " + String(memTotalGb, 1) + " GB",
        150,
        136,
        1);

    desenharProgresso(18, 160, 276, 8, memoryUsage, TFT_GREEN);

    tft.fillRect(14, 222, 292, 12, TFT_NAVY);
    tft.setTextColor(TFT_LIGHTGREY, TFT_NAVY);
    tft.drawString("Atualizado " + horaColeta, 16, 223, 1);

    telaAtual = TelaAtual::Dashboard;
}

// ---------------------------------------------------------
// Consulta a API
// ---------------------------------------------------------

void conectarWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    desenharEstruturaConectando();

    unsigned long inicio = millis();
    const unsigned long timeoutMs = 20000;

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - inicio > timeoutMs)
        {
            WiFi.disconnect();
            delay(500);
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            inicio = millis();
            desenharEstruturaConectando();
        }

        delay(500);
    }

    tft.fillScreen(TFT_BLACK);
    drawHeader("Wi-Fi conectado", TFT_GREEN);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Sincronizando dados...", 75, 95, 2);
    delay(700);

    telaAtual = TelaAtual::Inicial;
}

void mostrarOffline()
{
    desenharEstruturaOffline();
}

void consultarApi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        conectarWiFi();
        return;
    }

    HTTPClient http;
    http.setTimeout(3000);
    http.begin(API_URL);

    int statusCode = http.GET();

    if (statusCode != HTTP_CODE_OK)
    {
        http.end();
        mostrarOffline();
        return;
    }

    String payload = http.getString();

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload);

    if (error)
    {
        http.end();
        mostrarOffline();
        return;
    }

    String machineName = document["machineName"] | "PC";
    String status = document["status"] | "offline";

    String cpuName = document["cpu"]["name"] | "CPU";
    float cpuUsage = document["cpu"]["usagePercent"] | 0.0f;
    bool cpuTempValida = !document["cpu"]["temperatureCelsius"].isNull();
    float cpuTemperature = document["cpu"]["temperatureCelsius"] | 0.0f;

    float memUsedGb = document["memory"]["usedGb"] | 0.0f;
    float memTotalGb = document["memory"]["totalGb"] | 0.0f;
    float memoryUsage = document["memory"]["usagePercent"] | 0.0f;

    String gpuName = document["gpu"]["name"] | "GPU";
    float gpuUsage = document["gpu"]["usagePercent"] | 0.0f;
    bool gpuTempValida = !document["gpu"]["temperatureCelsius"].isNull();
    float gpuTemperature = document["gpu"]["temperatureCelsius"] | 0.0f;

    String collectedAt = document["collectedAt"] | "";
    String horaColeta = extrairHora(collectedAt);

    if (status != "online")
    {
        http.end();
        mostrarOffline();
        return;
    }

    atualizarDashboard(
        machineName,
        cpuName,
        cpuUsage,
        cpuTempValida,
        cpuTemperature,
        memUsedGb,
        memTotalGb,
        memoryUsage,
        gpuName,
        gpuUsage,
        gpuTempValida,
        gpuTemperature,
        horaColeta);

    http.end();
}

// ---------------------------------------------------------
// Setup / loop
// ---------------------------------------------------------

void setup()
{
    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    conectarWiFi();
}

void loop()
{
    unsigned long agora = millis();

    if (agora - ultimaConsulta >= intervaloConsulta)
    {
        ultimaConsulta = agora;
        consultarApi();
    }
}
