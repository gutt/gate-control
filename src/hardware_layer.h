#pragma once

#include "button.h"
#include "gate_control_handler.h"
#include <Ticker.h>

#define BUTTON_CONTACTRON_PIN  5 // D1
#define BUTTON_PHISICAL_BUTTON_UP  14 // D5
#define BUTTON_PHISICAL_BUTTON_DOWN 12 // D6
#define GATE_SWITCH_PIN 4 // D2

#define GATE_PULSE_TIME_MS 200
#define GATE_PULSE_TIME_S_ZERO_MS 1000

class HardwareLayer 
{
public:
    void setup();
    void loop();

    void toggle_gate(String caller_debug_info);

    void set_gate_control_handler(GateControlHandler *gate_control_handler);

private:    
    void click_gate();
    Button contactron_button { BUTTON_CONTACTRON_PIN, 2000 };

    Button phisical_button_up { BUTTON_PHISICAL_BUTTON_UP, 500 };
    Button phisical_button_down { BUTTON_PHISICAL_BUTTON_DOWN, 500 };

    GateControlHandler *gate_control_handler_ = nullptr;
    bool first_run = true;

    Ticker gate_switch_timer;
    Ticker gate_switch_interlude_timer;
    
    bool click_is_processing = false;
    int toggle = 0;
};