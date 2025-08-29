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
    GateState();
    GateState(const GateState &s1);


    bool is_closed() const;
    bool is_open() const;
    bool is_closing() const;
    bool is_opening() const;

    bool is_stopped() const;
    bool is_undefined() const;

    void set_closed();
    void set_open();
    void set_closing();
    void set_opening();

    void set_stopped(bool stopped);

    bool is_contactron_enabled() const;
    void set_contactron(bool state);

    String current_state_str() const;
    static String state_str(const gate_state_t &state);

private:
    void set_state(const gate_state_t &state);
    const gate_state_t &state() const;

    gate_state_t gate_state = gate_state_t::GATE_UNDEFINED;
    bool gate_stopped = false;
    bool contactron = false;



};

