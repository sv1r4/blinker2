#include "blinker2.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#define SSID "ssid"
#define PASSWORD "password"
#define LED 13
#define NUMPIXELS 1

ESP8266WebServer http(8028);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);
Blinker2 blinker(pixels);
bool wasConnected;

void initOta()
{

    ArduinoOTA.onStart([]() {
        blinker.setMode(M_OTA);
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("end");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        blinker.loop();
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.println("Error: " + error);
        if (error == OTA_AUTH_ERROR)
            Serial.println(F("Auth Failed"));
        else if (error == OTA_BEGIN_ERROR)
            Serial.println(F("Begin Failed"));
        else if (error == OTA_CONNECT_ERROR)
            Serial.println(F("Connect Failed"));
        else if (error == OTA_RECEIVE_ERROR)
            Serial.println(F("Receive Failed"));
        else if (error == OTA_END_ERROR)
            Serial.println(F("End Failed"));
    });
    ArduinoOTA.begin();
}

void onConfig()
{
    DynamicJsonBuffer jsonBuffer(4096);
    String body = http.arg(F("plain"));
    Serial.println("got common config" + body);

    JsonObject &root = jsonBuffer.parseObject(body);
    if (!root.success())
    {
        Serial.println(F("parseObject() failed"));
        http.send(400, "text/plain", "Parse JSON Failed");
        return;
    }
    
    int seqCnt = root["seqCnt"];
    blinker.setSeqCnt(seqCnt);
    blinker.setFadeDelay(root["fadeDelay"]);
    blinker.setColorDelay(root["colorDelay"]);
    blinker.setDelta(root["delta"]);
    for(int i=0;i<seqCnt;i++){
        blinker.setSeqColor(i, root["seq"][i]);
    }
}

void onColorConfig()
{
    DynamicJsonBuffer jsonBuffer(4096);
    String body = http.arg(F("plain"));
    Serial.println("got color config" + body);

    JsonObject &root = jsonBuffer.parseObject(body);
    if (!root.success())
    {
        Serial.println(F("parseObject() failed"));
        http.send(400, "text/plain", "Parse JSON Failed");
        return;
    }
}

void setup()
{
    Serial.begin(115200);
    blinker.start();
    http.begin();

    initOta();
    String apSsidStr = "blinker2.demo-" + String(ESP.getChipId(), HEX);
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(SSID, PASSWORD);
    WiFi.softAP(apSsidStr.c_str());

    http.on("/api/config", HTTP_POST, onConfig);
}

void loop()
{
    if (!wasConnected && WiFi.status() == WL_CONNECTED)
    {
        wasConnected = true;
        Serial.printf("Connected ip:%s",
                      WiFi.localIP().toString().c_str());
    }
    else
    {
        wasConnected = false;
    }

    blinker.loop();
    http.handleClient();
    ArduinoOTA.handle();
}
