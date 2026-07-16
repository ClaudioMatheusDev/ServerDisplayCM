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

void conectarWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Conectando ao Wi-Fi...", 10, 20, 2);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    tft.fillScreen(TFT_BLACK);
    tft.drawString("Wi-Fi conectado", 10, 20, 2);
}

void mostrarOffline()
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("PC OFFLINE", 80, 100, 4);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Verifique a API", 90, 145, 2);
}

void atualizarTela(
    float cpuUsage,
    float cpuTemperature,
    float memoryUsage,
    float gpuUsage,
    float gpuTemperature)
{
    tft.fillScreen(TFT_BLACK);

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