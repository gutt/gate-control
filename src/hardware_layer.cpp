#include "hardware_layer.h"
#include <Arduino.h>
#include "button.h"
#include "ArduinoLog.h"

void HardwareLayer::set_gate_control_handler(GateControlHandler *gate_control_handler) 
{
    gate_control_handler_ = gate_control_handler;
}

void HardwareLayer::setup()
{
    pinMode(GATE_SWITCH_PIN, OUTPUT);

    contactron_button.set_pressed_callback([this] () {
        Log.noticeln("HWLayer    # Contactron ENABLED");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->enabled_contactor();
    });
    contactron_button.set_released_callback([this] () {
        Log.noticeln("HWLayer    # Contactron RELEASED");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->disabled_contactor();
    });

    phisical_button_up.set_pressed_callback([this] () {
        Log.noticeln("HWLayer    # ===========> Gate UP button clicked");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->open_gate();
    });
    phisical_button_down.set_pressed_callback([this] () {
        Log.noticeln("HWLayer    # ===========> Gate DOWN button clicked");
        if(gate_control_handler_ == nullptr) {
            return;
        } 
        gate_control_handler_->close_gate();
    });
}

void HardwareLayer::loop() 
{
    contactron_button.loop();
    phisical_button_up.loop();
    phisical_button_down.loop();

    if(click_is_processing == false && toggle != 0) {
        toggle--;
        click_gate();
    }
}

void HardwareLayer::toggle_gate(String caller_debug_info) 
{
    Log.noticeln("HWLayer    # TOGGLE gate (%s)", caller_debug_info.c_str());
    toggle++;
}

void HardwareLayer::click_gate()
{
    // Log.noticeln("HWLayer    # Toggle gate started, queued clicks: %d", toggle_queue);

    click_is_processing = true;

    Log.noticeln("HWLayer    # gate signal up..");
    digitalWrite(GATE_SWITCH_PIN, 1);
 
    gate_switch_timer.once_ms(GATE_PULSE_TIME_MS, [=] () {
    Log.noticeln("HWLayer    # gate signal down..");
        digitalWrite(GATE_SWITCH_PIN, 0);
        gate_switch_interlude_timer.once_ms(GATE_PULSE_TIME_S_ZERO_MS, [=] () {
            Log.noticeln("HWLayer    # gate signal finished");
            click_is_processing = false;
        });
    });
    
}