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
    IDLE
};

class GateSystem : public GateControlHandler
{
public:
    using gate_event_handler_t = std::function<void()>;

    GateSystem(HardwareLayer *hw_layer);
    ~GateSystem() {}

    void setup();
    void loop();

    void open_gate();
    void close_gate();
    void stop_gate();

    void enabled_contactron();
    void disabled_contactron();

    // GateState &gate_state();
    void set_gate_open_handler(gate_event_handler_t handler);
    void set_gate_opening_handler(gate_event_handler_t handler);
    void set_gate_close_handler(gate_event_handler_t handler);
    void set_gate_closing_handler(gate_event_handler_t handler);
    void set_gate_stopped_handler(gate_event_handler_t handler);

    GateState &get_gate_state() { return gate; }
private:
    void set_state_undefined();

    GateState gate;
    HardwareLayer *hw;

    long unsigned int gate_state_check_interval = 25;
    int check_count_limit = 5;

    int current_check_number = 0;
    GateSystemMode current_mode = GateSystemMode::IDLE;
    GateSystemMode last_mode = GateSystemMode::IDLE;

    unsigned long last_check_millis = 0;

    gate_event_handler_t gate_open_handler = nullptr;
    gate_event_handler_t gate_opening_handler = nullptr;
    gate_event_handler_t gate_close_handler = nullptr;
    gate_event_handler_t gate_closing_handler = nullptr;
    gate_event_handler_t gate_stopped_handler = nullptr;

    int gate_open_check_count = 0;
    int gate_close_check_count = 0;

};