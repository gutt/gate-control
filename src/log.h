#pragma once

#include <ArduinoLog.h>
#include "mqtt.h"


class CustomLog : public Print {
public:
    CustomLog(Stream &serial, MQTT &mqtt_client);

    virtual size_t write(uint8_t character) override;
    virtual size_t write(const uint8_t *buffer, size_t size) override;

    private:
    Stream &serial;
    MQTT &mqtt_client;
    const char* topic;

    // Buffer for building one “line” to publish.
    static const uint16_t MQTT_BUFFER_SIZE = 256;
    char mqtt_buffer[MQTT_BUFFER_SIZE];
    uint16_t mqtt_buffer_index = 0;
};

void print_timestamp(Print* log_output);
