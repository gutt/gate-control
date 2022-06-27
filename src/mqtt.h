#pragma once

#include "gate_system.h"

#include <WiFiClient.h>
#include <PubSubClient.h>

class MQTT
{
public:
    MQTT(const String &ip, int port, const String &user, const String &password, GateSystem *s);
    void send_state(const GateState &s);
    void send_log(String log);
    void setup();
    void loop();
private:
    void callback(char *topic, byte *payload, unsigned int length);

    String ip;
    int port;
    String user;
    String password;
    GateSystem *s;

    WiFiClient esp_client;
    PubSubClient mqtt;
    int last_connect_try_milis = 0;


};
