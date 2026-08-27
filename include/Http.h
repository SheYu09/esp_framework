// Http.h

#ifndef _HTTP_h
#define _HTTP_h

#include "Arduino.h"
#include <ESP8266WebServer.h>

class Http
{
private:
    static bool isBegin;
    static bool updateAuthFailed;
    static void handleRoot();
    static void handledhcp();
    static void handleHttp();
    static void handleScanWifi();
    static void handleWifi();
    static void handleOperate();
    static void handleNotFound();
    static void handleModuleSetting();
    static void handleOTA();
    static void handleGetStatus();
    static void handleUpdate();

public:
    static bool checkAuth();
    static ESP8266WebServer *server;
    static void begin();
    static void stop();
    static void loop();
    static bool captivePortal();

    static void OTA(String url);
};

#endif
