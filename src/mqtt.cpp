#include "mqtt.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include "log.h"

MQTT::MQTT(const String &ip, int port, const String &user, const String &password) :
    ip(ip), port(port), user(user), password(password)
{

}

void MQTT::setup()
{
    mqtt.setServer(ip.c_str(), port);
    mqtt.setClient(esp_client);
    mqtt.setBufferSize(512);
    mqtt.setCallback([this] (char *topic, uint8_t *payload, unsigned int length) {
        callback(topic, payload, length);
    });
    mqtt.setKeepAlive(10);

    connect();
}
void MQTT::send_log(String log) {
    if(!mqtt.connected()) {
        return;
    }
    log.trim();
    mqtt.publish("garage_controller/log", log.c_str());
}

void MQTT::send_gate_close()
{
    publish_state("closed");
}

void MQTT::send_gate_closing()
{
    publish_state("closing");
}

void MQTT::send_gate_open()
{
    publish_state("open");
}

void MQTT::send_gate_opening()
{
    publish_state("opening");
}

void MQTT::send_gate_stopped()
{
    publish_state("stopped");
}
void MQTT::publish_state(const char *state)
{
    if(!mqtt.connected()) {
        Log.warningln("MQTT       # not connected");
        return;
    }
    Log.noticeln("MQTT       # publish -> state: %s", state);
    mqtt.publish("homeassistant/cover/garage_gate/state", state);
    Log.noticeln("MQTT       # published! -> state: %s", state);
}

void MQTT::callback(char *topic, byte *payload, unsigned int length)
{
    String payload_str= "";
    String topic_str(topic);
    for (unsigned int i = 0; i < length; i++) {
        payload_str += (char)payload[i];
    }
    Log.verboseln(String("MQTT       # Incoming <- topic: " + topic_str + " payload_str: " + payload_str).c_str());

    if(topic_str.equals("homeassistant/cover/garage_gate/set")) {
        if(payload_str.equals("OPEN")) {
            Log.noticeln("MQTT       # ===========> gate door OPEN request");
            if(s != nullptr) {
                s->open_gate();
            }
        } else if (payload_str.equals("CLOSE")) {
            Log.noticeln("MQTT       # ===========> gate door CLOSE request");
            if(s != nullptr) {
                s->close_gate();
            }
        } else if (payload_str.equals("STOP")) {
            Log.noticeln("MQTT       # ===========> gate door STOP request");
            if(s != nullptr) {
                s->stop_gate();
            }
        }
    }
}
void MQTT::connect()
{
    String client_id = "garage-door-controller-" + String(random(0xffff), HEX);
    Log.noticeln("MQTT       # Attempting MQTT connection to: %s:%d (client_id: %s)...", ip.c_str(), port, client_id.c_str());

    // Last Will topic/message
    const char* lwt_topic   = "homeassistant/cover/garage_gate/LWT";
    const char* lwt_message = "offline";
    int         lwt_qoS     = 0;     // Typically QOS 0 is fine, but choose as needed
    bool        lwt_retain  = true;  // Whether the LWT message should be retained

    auto connected = mqtt.connect(
        client_id.c_str(),
        user.c_str(),
        password.c_str(),
        lwt_topic,
        lwt_qoS,
        lwt_retain,
        lwt_message
    );

    if (connected) {
        Log.noticeln("MQTT       # MQTT Connection established.");
        mqtt.publish(lwt_topic, "online", lwt_retain);

        mqtt.subscribe("homeassistant/cover/garage_gate/set");
        mqtt.publish(
            "homeassistant/cover/garage_gate/config",
            "{\"name\": \"Garage gate\", \
            \"device_class\": \"garage\", \
            \"command_topic\": \"homeassistant/cover/garage_gate/set\", \
            \"state_topic\": \"homeassistant/cover/garage_gate/state\"}", \
            true
        );
    }
    else {
        Log.errorln("MQTT       # Connection failed, rc=%d", mqtt.state());
        Log.errorln("MQTT       # Retry in 5 seconds..");
        last_connect_try_milis = millis();
        return;
    }
}
void MQTT::loop()
{
    if (!mqtt.connected()) {
        if (millis() - last_connect_try_milis < 5000) {
            return;
        }
        connect();
    }
    mqtt.loop();
}
