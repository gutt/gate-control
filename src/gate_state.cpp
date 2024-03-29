#include "gate_state.h"

#include <Arduino.h>
#include <ArduinoLog.h>


GateState::GateState()
{

}

GateState::GateState(const GateState &s) 
{
    gate_state = s.gate_state; 
    gate_stopped = s.gate_stopped;
    contactor = s.contactor;

    gate_close_check_count = s.gate_close_check_count;
    gate_open_check_count = s.gate_open_check_count;
}

const gate_state_t &GateState::state() const
{
    return gate_state;
}

void GateState::set_state(const gate_state_t &state) 
{
    Log.verboseln("GateState  # #### Change gate state from [%s]->[%s] ####", 
        GateState::state_str(gate_state).c_str(), GateState::state_str(state).c_str()
    );
    gate_state = state;

    if(gate_state == gate_state_t::GATE_OPEN || gate_state == gate_state_t::GATE_CLOSED) {
        gate_stopped = true;
    }
    gate_stopped = false;

    if(update_state_handler != nullptr) {
        update_state_handler(*this);
    }
}

bool GateState::is_stopped() const
{
    return gate_stopped;
}

void GateState::set_stopped(bool stopped) 
{
    Log.verboseln("GateState  # #### Change gate stopped to: %s ####", stopped ? "true": "false");
    gate_stopped = stopped;
}

bool GateState::is_contactor_enabled() const
{
    return contactor;
}

void GateState::set_contactor(bool state) 
{
    contactor = state;
}

void GateState::set_update_state_handler(update_state_handler_t handler) {
    update_state_handler = handler;
}

String GateState::current_state_str() const
{
    return state_str(gate_state);
}

String GateState::state_str(const gate_state_t &state)
{
    String gate_state_str;
    if (state == gate_state_t::GATE_OPEN) {
        gate_state_str = "open";
    } else if (state == gate_state_t::GATE_CLOSED) {
        gate_state_str = "closed";
    } else if (state == gate_state_t::GATE_OPENING) {
        gate_state_str = "opening";
    } else if (state == gate_state_t::GATE_CLOSING) {
        gate_state_str = "closing";
    } else {
        gate_state_str = "undefined";
    }
    return gate_state_str;
}

