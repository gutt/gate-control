#pragma once

#include <WString.h>
#include <functional>


enum gate_state_t
{
    GATE_OPEN,
    GATE_CLOSED,
    GATE_UNDEFINED,
    GATE_CLOSING,
    GATE_OPENING
};

class GateState 
{
public:
    using update_state_handler_t = std::function<void(const GateState&)>;

    GateState();
    GateState(const GateState &s1);
 
    const gate_state_t &state() const;

    void set_state(const gate_state_t &state);

    bool is_stopped() const;
    void set_stopped(bool stopped);

    bool is_contactor_enabled() const;
    void set_contactor(bool state);

    void set_update_state_handler(update_state_handler_t handler);
    
    const String state_str() const;

public:
    int gate_close_check_count = 0;
    int gate_open_check_count = 0;
    
private:
    gate_state_t gate_state = gate_state_t::GATE_UNDEFINED;
    bool gate_stopped = false;    
    bool contactor = false;

    update_state_handler_t update_state_handler = nullptr;

};

