#include "mqtt.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include "log.h"

MQTT::MQTT(const String &ip, int port, const String &user, const String &password, GateSystem *s) : 
    ip(ip), port(port), user(user), password(password), s(s) 
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
}
void MQTT::send_log(String log) {
    if(!mqtt.connected()) {
        return;
    }
    mqtt.publish("garage_controller/log", log.c_str());
}
void MQTT::send_state(const GateState &s)
{
    if (s.state() == gate_state_t::GATE_OPEN) {
        Log.noticeln("MQTT       # publish -> gate OPEN");
        mqtt.publish("homeassistant/cover/garage_gate/state", "open", true);
        return;
    }
    if (s.state() == gate_state_t::GATE_CLOSED) {
        Log.noticeln("MQTT       # publish -> gate CLOSED");
        mqtt.publish("homeassistant/cover/garage_gate/state", "closed", true);
        return;
    }
    if (s.state() == gate_state_t::GATE_OPENING) {
        Log.noticeln("MQTT       # publish -> gate OPENING");
        mqtt.publish("homeassistant/cover/garage_gate/state", "opening", true);
        return;
    }
    if (s.state() == gate_state_t::GATE_CLOSING) {
        Log.noticeln("MQTT       # publish -> gate CLOSING");
        mqtt.publish("homeassistant/cover/garage_gate/state", "closing", true);
        return;
    }
}

void MQTT::callback(char *topic, byte *payload, unsigned int length)
{
    String payload_str= "";
    String topic_str(topic);
    for (unsigned int i = 0; i < length; i++) {
        payload_str += (char)payload[i];
    }
    Log.verboseln(String("MQTT       # Incoming <- topic: " + topic_str + " payload_str: " + payload_str));
        
    if(topic_str.equals("homeassistant/cover/garage_gate/set")) {
      if(payload_str.equals("OPEN")) {
        Log.noticeln("MQTT       # ===========> gate door OPEN request");
        s->open_gate();
      } else if (payload_str.equals("CLOSE")) {
        Log.noticeln("MQTT       # ===========> gate door CLOSE request");
        s->close_gate();
      } else if (payload_str.equals("STOP")) {
        Log.noticeln("MQTT       # ===========> gate door STOP request");
        s->stop_gate();
      }
    }
}

void MQTT::loop()
{
    if (!mqtt.connected()) {
        if (millis() - last_connect_try_milis < 5000) {
            return;
        }
        String client_id = "garage-door-controller-" + String(random(0xffff), HEX);
        Log.noticeln("MQTT       # Attempting MQTT connection to: %s:%d (client_id: %s)...", ip.c_str(), port, client_id.c_str());

        if (mqtt.connect(client_id.c_str(), user.c_str(), password.c_str())) {
            Log.noticeln("MQTT       # MQTT Connection established.");
            
            mqtt.subscribe("homeassistant/cover/garage_gate/set");
            mqtt.publish(
                "homeassistant/cover/garage_gate/config", 
                "{\"name\": \"Garage gate\", \
                \"device_class\": \"garage\", \
                \"command_topic\": \"homeassistant/cover/garage_gate/set\", \
                \"state_topic\": \"homeassistant/cover/garage_gate/state\"}",
                true
            );
        } else {
            Log.errorln("MQTT       # Connection failed, rc=%d", mqtt.state());
            Log.errorln("MQTT       # Retry in 5 seconds..");
            last_connect_try_milis = millis();
            return;
        }
    }
    mqtt.loop();
}
