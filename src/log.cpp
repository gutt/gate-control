#include "log.h"
#include <ArduinoLog.h>
#include "mqtt.h"


CustomLog::CustomLog(Stream &serial, MQTT &mqtt_client) :
    serial(serial),
    mqtt_client(mqtt_client)
{
}


size_t CustomLog::write(uint8_t character) {
    serial.write((char)character);

    if (mqtt_buffer_index < sizeof(mqtt_buffer)-1) {
        mqtt_buffer[mqtt_buffer_index++] = character;
    }

    if (character == '\n') {
        mqtt_buffer[mqtt_buffer_index] = '\0';
        mqtt_client.send_log(mqtt_buffer);
        mqtt_buffer_index = 0;
    }

    return 1;
}

size_t CustomLog::write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
        n += write(*buffer++);
    }
return n;
}


void print_timestamp(Print* _logOutput)
{
  time_t now = time(nullptr);
  struct tm* timeInfo = localtime(&now);
  char buffer[9];
  strftime(buffer, sizeof(buffer), "%T", timeInfo);
  _logOutput->print(buffer);
  _logOutput->print(" ");
}