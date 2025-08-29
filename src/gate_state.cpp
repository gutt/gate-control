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
    contactron = s.contactron;
}

const gate_state_t &GateState::state() const
{
    return gate_state;
}

void GateState::set_open()
{
    set_state(gate_state_t::GATE_OPEN);
}

void GateState::set_closed()
{
    set_state(gate_state_t::GATE_CLOSED);
}

void GateState::set_closing()
{
    set_state(gate_state_t::GATE_CLOSING);
}

void GateState::set_opening()
{
    set_state(gate_state_t::GATE_OPENING);
}

void GateState::set_state(const gate_state_t &state)
{
    Log.verboseln("GateState  # #### Change gate state from [%s]->[%s] ####",
        GateState::state_str(gate_state).c_str(), GateState::state_str(state).c_str()
    );
    gate_state = state;
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

bool GateState::is_contactron_enabled() const
{
    return contactron;
}

void GateState::set_contactron(bool state)
{
    contactron = state;
}

String GateState::current_state_str() const
{
    return state_str(gate_state);
}

bool GateState::is_closed() const
{
    return gate_state == gate_state_t::GATE_CLOSED;
}

bool GateState::is_open() const
{
    return gate_state == gate_state_t::GATE_OPEN;
}

bool GateState::is_closing() const
{
    return gate_state == gate_state_t::GATE_CLOSING;
}

bool GateState::is_opening() const
{
    return gate_state == gate_state_t::GATE_OPENING;
}

bool GateState::is_undefined() const
{
    return gate_state == gate_state_t::GATE_UNDEFINED;
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

