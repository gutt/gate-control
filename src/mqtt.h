#pragma once

#include "gate_system.h"

#include <WiFiClient.h>
#include <PubSubClient.h>

class MQTT
{
public:
    MQTT(const String &ip, int port, const String &user, const String &password);

    void send_gate_open();
    void send_gate_opening();
    void send_gate_close();
    void send_gate_closing();
    void send_gate_stopped();

    void send_log(String log);
    void setup();
    void loop();
    void set_gate_system(GateSystem *s) { this->s = s; }
private:
    void connect();
    void callback(char *topic, byte *payload, unsigned int length);
    void publish_state(const char *state);

    String ip;
    int port;
    String user;
    String password;
    GateSystem *s = nullptr;

    WiFiClient esp_client;
    PubSubClient mqtt;
    int last_connect_try_milis = 0;


};
