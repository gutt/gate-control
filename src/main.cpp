#include "main.h"

#include <ESP8266WiFi.h>
#include <ArduinoLog.h>

#include "hardware_layer.h"
#include "gate_system.h"
#include "http.h"
#include "mqtt.h"
#include "version.h"

// #include <Wire.h>
// #include <VL53L0X.h>

// VL53L0X sensor;

HardwareLayer hw_layer;
GateSystem gate_system(&hw_layer);
Http http(&gate_system);
Stream &serial_out = Serial;
MQTT mqtt(mqtt_server, mqtt_port, mqtt_user, mqtt_password);
CustomLog custom_logger(serial_out, mqtt);

void print_prefix(Print* _logOutput, int logLevel) {
    print_timestamp(_logOutput);
}

void setup_wifi()
{
    Log.noticeln("Main       # Configuring wifi (SSID: %s)...", ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_BUILTIN, 0);
        delay(500);
        digitalWrite(LED_BUILTIN, 1);
        Serial.print(".");
    }
    Serial.println(".");

    Log.noticeln("---------------------------------------------------------------");
    Log.noticeln("Main       # WiFi connected to: %s", ssid);
    Log.noticeln("Main       # IP address: %s", WiFi.localIP().toString().c_str());
    Log.noticeln("---------------------------------------------------------------");
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    gate_system.set_gate_open_handler([&] () {
        Log.noticeln("Main       # Gate is OPEN");
        mqtt.send_gate_open();
    });

    gate_system.set_gate_opening_handler([&] () {
        Log.noticeln("Main       # Gate is OPENING");
        mqtt.send_gate_opening();
    });

    gate_system.set_gate_close_handler([&] () {
        Log.noticeln("Main       # Gate is CLOSED");
        mqtt.send_gate_close();
    });

    gate_system.set_gate_closing_handler([&] () {
        Log.noticeln("Main       # Gate is CLOSING");
        mqtt.send_gate_closing();
    });

    gate_system.set_gate_stopped_handler([&] () {
        Log.noticeln("Main       # Gate is STOPPED");
        mqtt.send_gate_stopped();
    });

    Serial.begin(74880);
    Log.setPrefix(print_prefix);
    Log.setShowLevel(false);
    Log.begin(LOG_LEVEL_VERBOSE, &custom_logger);

    setup_wifi();

    Log.noticeln("GarageApp version: %d.%d (build date: %s %s)", VERSION_MAJOR, VERSION_MINOR, __DATE__, __TIME__);
    Log.noticeln("");
    mqtt.set_gate_system(&gate_system);

    hw_layer.set_gate_control_handler(&gate_system);


    hw_layer.setup();

    http.setup();
    mqtt.setup();
    EasyOta.setup();
    gate_system.setup();

    still_alive_timer.attach(60, [] () {
        Log.noticeln("alive mark");
    });
}

void loop()
{
    http.loop();
    mqtt.loop();
    hw_layer.loop();
    EasyOta.checkForUpload();
    gate_system.loop();
}