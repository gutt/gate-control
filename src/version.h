#pragma once

#define VERSION_MAJOR 1
#define VERSION_MINOR 3

#include <ESP8266WiFi.h>

static String version_str = String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + " (build date: " + String(__DATE__) + " " + String(__TIME__) + ")";
