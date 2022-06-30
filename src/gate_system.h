#pragma once

#include <Ticker.h>
#include <Arduino.h>
#include "hardware_layer.h"
#include "gate_state.h"
#include "gate_control_handler.h"

class String;


class GateSystem : public GateControlHandler
{
public:
    GateSystem(HardwareLayer *hw_layer);
    ~GateSystem() {}

    void open_gate();
    void close_gate();
    void stop_gate();

    void enabled_contactor();
    void disabled_contactor();

    GateState &state();

private:
    void ensure_gate_opened();
    void stop_ensure_gate_opened();

    void ensure_gate_closed();
    void stop_ensure_gate_closed();

    void reset_process();

    GateState state_;
    HardwareLayer *hw;

    int gate_state_check_interval = 25;
    int check_count_limit = 5;

    Ticker ensure_gate_closed_timer;
    Ticker ensure_gate_opened_timer;    
};