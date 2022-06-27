#pragma once
#include <ESP8266WebServer.h>

#include "gate_system.h"


class Http
{
public:
    Http(GateSystem *s);
    void setup();
    void loop();
private:
    void render_home(const GateState &s);

    ESP8266WebServer server {80};
    GateSystem *s;
};
