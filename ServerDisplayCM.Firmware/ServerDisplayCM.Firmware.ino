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

<<<<<<< HEAD
void drawHeader(const char* title, uint16_t accentColor)
{
    tft.fillRect(0, 0, 320, 44, TFT_DARKCYAN);
    tft.drawFastHLine(0, 44, 320, accentColor);
    tft.setTextColor(TFT_WHITE, TFT_DARKCYAN);
    tft.drawString(title, 12, 10, 3);
}

void drawMetricCard(
    const char* label,
    float value,
    const char* unit,
    int x,
    int y,
    uint16_t fillColor,
    uint16_t borderColor,
    uint16_t accentColor)
{
    tft.fillRoundRect(x, y, 140, 72, 8, fillColor);
    tft.drawRoundRect(x, y, 140, 72, 8, borderColor);
    tft.fillRect(x + 10, y + 10, 6, 52, accentColor);

    tft.setTextColor(TFT_WHITE, fillColor);
    tft.drawString(label, x + 24, y + 8, 2);

    tft.setTextColor(accentColor, fillColor);
    tft.drawString(String(value, 1), x + 24, y + 24, 4);

    tft.setTextColor(TFT_LIGHTGREY, fillColor);
    tft.drawString(unit, x + 24, y + 52, 2);
}

=======
>>>>>>> ece48f7678d95db55c58805ab59064038de98d5b
void conectarWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

<<<<<<< HEAD
    tft.fillScreen(TFT_BLACK);
    drawHeader("Conectando...", TFT_YELLOW);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Aguardando rede Wi-Fi", 54, 90, 2);
=======
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Conectando ao Wi-Fi...", 10, 20, 2);
>>>>>>> ece48f7678d95db55c58805ab59064038de98d5b

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    tft.fillScreen(TFT_BLACK);
<<<<<<< HEAD
    drawHeader("Wi-Fi conectado", TFT_GREEN);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Sincronizando dados...", 75, 95, 2);
    delay(700);
=======
    tft.drawString("Wi-Fi conectado", 10, 20, 2);
>>>>>>> ece48f7678d95db55c58805ab59064038de98d5b
}

void mostrarOffline()
{
    tft.fillScreen(TFT_BLACK);
<<<<<<< HEAD
    drawHeader("Sem resposta", TFT_RED);

    tft.fillRoundRect(30, 62, 260, 100, 10, TFT_DARKGREY);
    tft.drawRoundRect(30, 62, 260, 100, 10, TFT_RED);

    tft.setTextColor(TFT_RED, TFT_DARKGREY);
    tft.drawString("PC OFFLINE", 76, 84, 4);

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("Verifique a API e a rede", 56, 128, 2);
=======

    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("PC OFFLINE", 80, 100, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Verifique a API", 90, 145, 2);
>>>>>>> ece48f7678d95db55c58805ab59064038de98d5b
}

void atualizarTela(
    float cpuUsage,
    float cpuTemperature,
    float memoryUsage,
    float gpuUsage,
    float gpuTemperature)
{
    tft.fillScreen(TFT_BLACK);
<<<<<<< HEAD
    drawHeader("SERVER DISPLAY", TFT_CYAN);

    drawMetricCard("CPU", cpuUsage, "%", 12, 58, TFT_DARKGREY, TFT_CYAN, TFT_CYAN);
    drawMetricCard("GPU", gpuUsage, "%", 168, 58, TFT_DARKGREY, TFT_CYAN, TFT_MAGENTA);
    drawMetricCard("MEM", memoryUsage, "%", 12, 142, TFT_DARKGREY, TFT_CYAN, TFT_GREEN);

    tft.fillRoundRect(168, 142, 140, 72, 8, TFT_DARKGREY);
    tft.drawRoundRect(168, 142, 140, 72, 8, TFT_CYAN);
    tft.fillRect(178, 152, 6, 52, TFT_YELLOW);

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("TEMP", 198, 150, 2);
    tft.setTextColor(TFT_YELLOW, TFT_DARKGREY);
    tft.drawString(String(cpuTemperature, 1) + "C", 198, 170, 4);
    tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
    tft.drawString(String(gpuTemperature, 1) + "C", 198, 198, 2);

    tft.fillRoundRect(12, 220, 296, 16, 4, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString("API ONLINE", 20, 223, 2);
=======

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("SERVER DISPLAY", 60, 10, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("CPU", 20, 60, 2);
    tft.drawString(
        String(cpuUsage, 1) + "%  " +
        String(cpuTemperature, 1) + " C",
        20,
        80,
        4
    );

    tft.drawString("MEMORIA", 20, 125, 2);
    tft.drawString(
        String(memoryUsage, 1) + "%",
        20,
        145,
        4
    );

    tft.drawString("GPU", 175, 60, 2);
    tft.drawString(
        String(gpuUsage, 1) + "%",
        175,
        80,
        4
    );

    tft.drawString(
        String(gpuTemperature, 1) + " C",
        175,
        110,
        2
    );

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("API ONLINE", 20, 210, 2);
>>>>>>> ece48f7678d95db55c58805ab59064038de98d5b
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

    DeserializationError error =
        deserializeJson(document, payload);

    if (error)
    {
        http.end();
        mostrarOffline();
        return;
    }

    float cpuUsage =
        document["cpu"]["usagePercent"] | 0.0;

    float cpuTemperature =
        document["cpu"]["temperatureCelsius"] | 0.0;

    float memoryUsage =
        document["memory"]["usagePercent"] | 0.0;

    float gpuUsage =
        document["gpu"]["usagePercent"] | 0.0;

    float gpuTemperature =
        document["gpu"]["temperatureCelsius"] | 0.0;

    atualizarTela(
        cpuUsage,
        cpuTemperature,
        memoryUsage,
        gpuUsage,
        gpuTemperature
    );

    http.end();
}

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