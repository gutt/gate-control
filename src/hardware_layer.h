#pragma once

#include <Button2.h>
#include "gate_control_handler.h"
#include <Ticker.h>

class HardwareLayer 
{
public:
    void setup();
    void loop();

    void toggle_gate(String caller_debug_info);

    void set_gate_control_handler(GateControlHandler *gate_control_handler);

private:    
    void click_gate();
    Button2 contactron_button;
    Button2 phisical_button_up;
    Button2 phisical_button_down;

    GateControlHandler *gate_control_handler_ = nullptr;
    bool first_run = true;

    Ticker gate_switch_timer;
    Ticker gate_switch_interlude_timer;
    
    bool click_is_processing = false;
    int toggle = 0;
};