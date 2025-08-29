#pragma once

#include <ESP8266WebServer.h>  // or <WebServer.h> on ESP32
#include "gate_system.h"

class Http {
public:
    explicit Http(GateSystem* system);
    void setup();
    void loop();

private:
    ESP8266WebServer server{80};  // Adjust to your board’s webserver class if needed
    GateSystem* s;

    // Route Handlers
    void handleRoot();
    void handleGateOpen();
    void handleGateClose();
    void handleGateStop();

    // Render the homepage
    void renderHome();

    // Prompt for Basic Auth
    bool requireAuth();
};