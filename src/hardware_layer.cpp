#include "hardware_layer.h"
#include <Arduino.h>
#include <Button2.h>
#include "ArduinoLog.h"

#define BUTTON_CONTACTRON_PIN  3
#define BUTTON_PHISICAL_BUTTON_UP  14
#define BUTTON_PHISICAL_BUTTON_DOWN 12
#define GATE_SWITCH_PIN 16
#define GATE_PULSE_TIME_S 2
#define GATE_PULSE_TIME_S_ZERO 3

void HardwareLayer::set_gate_control_handler(GateControlHandler *gate_control_handler) 
{
    gate_control_handler_ = gate_control_handler;
}

void HardwareLayer::setup()
{
    pinMode(16, OUTPUT);

    contactron_button.begin(BUTTON_CONTACTRON_PIN);
    contactron_button.setPressedHandler([this] (Button2 &) {
        Log.noticeln("HWLayer    # Contactron ENABLED");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->enable_contactor();
    });
    contactron_button.setReleasedHandler([this] (Button2 &) {
        Log.noticeln("HWLayer    # Contactron RELEASED");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->disable_contactor();
    });
    contactron_button.setDebounceTime(2000);

    phisical_button_up.begin(BUTTON_PHISICAL_BUTTON_UP);
    phisical_button_up.setTapHandler([this] (Button2 &) {
        Log.noticeln("HWLayer    # ===========> Gate UP button clicked");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->open_gate_with_stop();
    });
    
    phisical_button_down.begin(BUTTON_PHISICAL_BUTTON_DOWN);
    phisical_button_down.setTapHandler([this] (Button2 &) {
        Log.noticeln("HWLayer    # ===========> Gate DOWN button clicked");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->close_gate_with_stop();
    });
}

void HardwareLayer::loop() 
{
    if(first_run) {
        if(contactron_button.isPressed()) {
            gate_control_handler_->enable_contactor();
        } else {
            gate_control_handler_->disable_contactor();
        }
        first_run = false;
    }

    contactron_button.loop();
    phisical_button_up.loop();
    phisical_button_down.loop();

    if(click_is_processing == false && toggle_queue > 0) {
        click_gate();
        toggle_queue--;
    }
}

void HardwareLayer::toggle_gate(String caller_debug_info) 
{
    Log.noticeln("HWLayer    # TOGGLE gate (%s)", caller_debug_info.c_str());
    toggle_queue++;
}

void HardwareLayer::click_gate()
{
    Log.noticeln("HWLayer    # Toggle gate started, queued clicks: %d", toggle_queue);

    click_is_processing = true;

    Log.noticeln("HWLayer    # gate signal up..");
    digitalWrite(GATE_SWITCH_PIN, 1);
    gate_switch_timer.once(GATE_PULSE_TIME_S, [=] () {
    Log.noticeln("HWLayer    # gate signal down..");
        digitalWrite(GATE_SWITCH_PIN, 0);
        gate_switch_interlude_timer.once(GATE_PULSE_TIME_S_ZERO, [=] () {
            Log.noticeln("HWLayer    # Toggle gate finished");
            click_is_processing = false;
        });
    });
    
}