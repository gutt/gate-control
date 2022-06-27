#include "main.h"

#include <ESP8266WiFi.h>
#include <ArduinoLog.h>

#include "hardware_layer.h"
#include "gate_system.h"
#include "http.h"
#include "mqtt.h"

// #include <Wire.h>
// #include <VL53L0X.h>

// VL53L0X sensor;


HardwareLayer hw_layer;
GateSystem gate_system(&hw_layer);
Http http(&gate_system);
MQTT mqtt(mqtt_server, mqtt_port, mqtt_user, mqtt_password, &gate_system);


void print_timestamp(Print* _logOutput) {

  // Division constants
  const unsigned long MSECS_PER_SEC       = 1000;
  const unsigned long SECS_PER_MIN        = 60;
  const unsigned long SECS_PER_HOUR       = 3600;
  const unsigned long SECS_PER_DAY        = 86400;

  // Total time
  const unsigned long msecs               =  millis() ;
  const unsigned long secs                =  msecs / MSECS_PER_SEC;

  // Time in components
  const unsigned long mili_secs =  msecs % MSECS_PER_SEC;
  const unsigned long seconds = secs  % SECS_PER_MIN ;
  const unsigned long minutes = (secs  / SECS_PER_MIN) % SECS_PER_MIN;
  const unsigned long hours = (secs  % SECS_PER_DAY) / SECS_PER_HOUR;

  // Time as string
  char timestamp[20];
  sprintf(timestamp, "%02lu:%02lu:%02lu.%03lu ", hours, minutes, seconds, mili_secs);
  _logOutput->print(timestamp);
}

void print_prefix(Print* _logOutput, int logLevel) {
    print_timestamp(_logOutput);
}

void print_suffix(Print* _logOutput, int logLevel) {
    _logOutput->print("\r");
}

String last_line;

size_t CustomLog::write(uint8_t character) {
    Serial.print((char)character);
    if(character == '\n') {
        udp.broadcast(last_line.c_str());
        last_line.replace("\n", "");
        mqtt.send_log(last_line);
        last_line = "";
    }
    last_line += (char)character;
    return 1;
};

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
    Serial.begin(74880);
    delay(2000);

    Log.setPrefix(print_prefix); // set prefix similar to NLog
    Log.setSuffix(print_suffix); // set suffix 
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    // Log.begin(LOG_LEVEL_VERBOSE, &UDPLogger);
    Log.setShowLevel(false);    // Do not show loglevel, we will do this in the prefix
    
    gate_system.state().set_update_state_handler([] (const GateState &s) {
        mqtt.send_state(s);
    });
    hw_layer.set_gate_control_handler(&gate_system);

    setup_wifi();
    Log.noticeln("GarageApp version: %d.%d (build date: %s %s)", VERSION_MAJOR, VERSION_MINOR, __DATE__, __TIME__);
    Log.noticeln("");
    
    if(udp.listen(12345)) {
        Log.noticeln("Main      # UDP Listen on port 12345");
    }
    Log.begin(LOG_LEVEL_VERBOSE, &CustomLogger);

    EasyOta.setup();

    http.setup();
    mqtt.setup();
    hw_layer.setup();
    
    close_gate_after_start.once(5, [&] () { 
        gate_system.close_gate();
    });

    // Wire.begin();
    // sensor.setTimeout(500);
    // if (!sensor.init()) {
    //     Log.noticeln("Failed to detect and initialize sensor!");
    // }
    // sensor.setMeasurementTimingBudget(80000);
    // sensor.setSignalRateLimit(0.2);
    // sensor.setVcselPulsePeriod(VL53L0X::vcselPeriodType::VcselPeriodPreRange, 18);
    // sensor.setVcselPulsePeriod(VL53L0X::vcselPeriodType::VcselPeriodFinalRange, 14);
    
    // sensor.startContinuous(100);
    still_alive_timer.attach(60, [=] () {
        Log.noticeln("alive mark");
    });
}

void loop()
{
    http.loop();
    mqtt.loop();
    hw_layer.loop();
    EasyOta.checkForUpload();

    // Log.noticeln("Distance: %lu", sensor.readRangeContinuousMillimeters());
    // if (sensor.timeoutOccurred()) { 
    //     Log.warningln("Distance sensor timeout"); 
    // }

}