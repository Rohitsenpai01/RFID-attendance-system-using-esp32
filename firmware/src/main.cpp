#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

void setupWiFi()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("[WIFI] Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.println("[WIFI] Connected");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());
}

void setup()
{
    Serial.begin(115200);
    setupWiFi();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[WIFI] Disconnected. Reconnecting...");
        WiFi.reconnect();
        delay(2000);
        return;
    }

    Serial.print("[WIFI] Connected: ");
    Serial.println(WiFi.localIP());
    delay(5000);
}