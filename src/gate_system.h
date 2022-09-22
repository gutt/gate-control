#pragma once

// #include <Ticker.h>
#include <Arduino.h>
#include "hardware_layer.h"
#include "gate_state.h"
#include "gate_control_handler.h"

class String;
enum GateSystemMode {
    ENSURE_GATE_OPEN,
    ENSURE_GATE_CLOSED,
    NORMAL_OPERATION
};

class GateSystem : public GateControlHandler
{
public:
    GateSystem(HardwareLayer *hw_layer);
    ~GateSystem() {}

    void setup();
    void loop();

    void open_gate();
    void close_gate();
    void stop_gate();

    void enabled_contactor();
    void disabled_contactor();

    GateState &state();

private:
    void set_state_undefined();

    GateState state_;
    HardwareLayer *hw;

    long unsigned int gate_state_check_interval = 25;
    int check_count_limit = 5;

    int current_check_number = 0;
    GateSystemMode current_mode = GateSystemMode::NORMAL_OPERATION;
    GateSystemMode last_mode = GateSystemMode::NORMAL_OPERATION;

    unsigned long last_check_millis = 0;
};