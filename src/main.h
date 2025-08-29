#pragma once

#include <Arduino.h>
#include <Print.h>
#include <Ticker.h>
#include "EasyOta.h"
#include "log.h"
// #include "ESPAsyncUDP.h"


/* Set these to your desired credentials. */
const char *ssid = "chyba_dziala_g";
const char *password = "abrakadabra71";

// AsyncUDP udp;

String mqtt_server = "192.168.1.8";
int mqtt_port = 1883;
String mqtt_user = "darek";
String mqtt_password = "wospamdK6";

Ticker close_gate_after_start;
Ticker still_alive_timer;