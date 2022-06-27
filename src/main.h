#pragma once

#include <Arduino.h>
#include <Print.h>
#include <Ticker.h>
#include "EasyOta.h"
#include "log.h"
#include "ESPAsyncUDP.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 2

/* Set these to your desired credentials. */
const char *ssid = "x";
const char *password = "x";

AsyncUDP udp;

String mqtt_server = "192.168.1.1";
int mqtt_port = 1883;
String mqtt_user = "x";
String mqtt_password = "x";

Ticker close_gate_after_start;
Ticker still_alive_timer;

CustomLog CustomLogger;

