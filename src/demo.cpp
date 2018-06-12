#define LED 13
#define NUMPIXELS 3

#include "blinker2.h"
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#define SSID "balloon"
#define PASSWORD "balL00n17988028"
ESP8266WebServer http(80);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);
Blinker2 blinker(pixels);
bool wasConnected;

void initOta()
{

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_SPIFFS
            type = "filesystem";
        }

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
        {
            Serial.println("Auth Failed");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
            Serial.println("Begin Failed");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
            Serial.println("Connect Failed");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
            Serial.println("Receive Failed");
        }
        else if (error == OTA_END_ERROR)
        {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin();
}

void onConfig()
{
    http.sendHeader("Access-Control-Allow-Origin", "*");

    if (http.method() == HTTP_OPTIONS)
    {
        http.sendHeader("Access-Control-Allow-Methods", "POST, GET");
        http.sendHeader("Access-Control-Allow-Headers", "X-PINGOTHER, Content-Type");
        http.send(200);
    }
    else if (http.method() == HTTP_POST )
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
        for (int i = 0; i < seqCnt; i++)
        {
            blinker.setSeqColor(i, root["seq"][i]);
        }

        http.send(200, "text/plain", "got config");
    }
    else if (http.method() == HTTP_GET)
    {
        String json = "{";
        int seqCnt = blinker.getSeqCnt();
        json.concat("\"seqCnt\":");
        json.concat(seqCnt);
        json.concat(",\"fadeDelay\":");
        json.concat(blinker.getFadeDelay());
        json.concat(",\"colorDelay\":");
        json.concat(blinker.getColorDelay());
        json.concat(",\"delta\":");
        json.concat(blinker.getDelta());
        json.concat(",\"seq\":[");
        for (int i = 0; i < seqCnt - 1; i++)
        {
            json.concat(blinker.getSeqColor(i));
            json.concat(",");
        }
        json.concat(blinker.getSeqColor(seqCnt - 1));
        json.concat("]}");
        Serial.println("Get config");
        Serial.println(json);
        http.send(200, "application/json", json);
    }
    else
    {
        http.send(400, "text/plain", "bad http method");
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
    delay(1000);
    String apSsidStr = "blinker2.demo-" + String(ESP.getChipId(), HEX);

    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);
    WiFi.softAPdisconnect();
    if (WiFi.isConnected())
    {
        WiFi.disconnect();
    };
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(SSID, PASSWORD);
    WiFi.softAP(apSsidStr.c_str());

    http.stop();
    http.on("/api/config", onConfig);
    http.begin();
    blinker.start();
}

void loop()
{
    ArduinoOTA.handle();
    if (!wasConnected && WiFi.status() == WL_CONNECTED)
    {
        wasConnected = true;
        Serial.printf("Connected ip:%s\n",
                      WiFi.localIP().toString().c_str());
        http.stop();
        http.on("/api/config", onConfig);
        http.begin();
        initOta();
    }
    else if (wasConnected && WiFi.status() != WL_CONNECTED)
    {
        wasConnected = false;
    }

    blinker.loop();
    http.handleClient();
}
