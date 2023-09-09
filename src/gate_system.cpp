#include "gate_system.h"
#include "led.h"
#include "ArduinoLog.h"

GateSystem::GateSystem(HardwareLayer *hw_layer) : 
    hw(hw_layer)
{
}

void GateSystem::set_state_undefined() {
    Log.verboseln("GateSystem # Set gate state to undefined.");
    current_mode = GateSystemMode::NORMAL_OPERATION;
    state_.set_state(gate_state_t::GATE_UNDEFINED);
    state_.set_stopped(true);    
    reset_led();
}

void GateSystem::setup() 
{

}

void GateSystem::loop()
{
    // in normal mode
    if(current_mode == GateSystemMode::NORMAL_OPERATION) {
        return;
    }

    // if mode changed to different mode than NORMAL
    if(current_mode != last_mode) {
        state_.gate_open_check_count = 0;
        state_.gate_close_check_count = 0;
 
        last_check_millis = millis();
        last_mode = current_mode;
    }

    if (current_mode == GateSystemMode::NORMAL_OPERATION ||
        (millis() - last_check_millis < 1000 * gate_state_check_interval)) {
        return;
    }

    last_check_millis = millis();

    if(current_mode == GateSystemMode::ENSURE_GATE_OPEN) {
        // if contactor is not enabled - switch back to normal operation
        if(!state_.is_contactor_enabled()) {
            current_mode = GateSystemMode::NORMAL_OPERATION;
            state_.set_state(gate_state_t::GATE_OPEN);
            return;
        }
        if (state_.gate_open_check_count >= check_count_limit) {
            set_state_undefined();
            return;
        }
        hw->toggle_gate("ENSURE_GATE_OPEN");
        state_.gate_open_check_count++;
        return;
    } 
    
    if(current_mode == GateSystemMode::ENSURE_GATE_CLOSED) {
        // if contactor is now enabled - switch back to normal operation
        if(state_.is_contactor_enabled()) {
            current_mode = GateSystemMode::NORMAL_OPERATION;
            return;
        }
        if (state_.gate_close_check_count >= check_count_limit) {
            set_state_undefined();
            return;
        }
        hw->toggle_gate("ENSURE_GATE_CLOSE");
        state_.gate_close_check_count++;
        return;
    }
}
void GateSystem::stop_gate()
{
    if(state_.is_stopped()) {
        return;
    }
    state_.set_stopped(true);

    hw->toggle_gate("GATE_STOP");

    blink_led(2000);

}

void GateSystem::open_gate()
{

    if((state_.state() == gate_state_t::GATE_OPENING && !state_.is_stopped()) 
    || (state_.state() == gate_state_t::GATE_OPEN)) {
        Log.verboseln("GateSystem  # #### IGNORE GATE OPEN because gate is in state [%s] ####", state_.current_state_str().c_str());
        return;
    }
    // if gate is arleady closing ..
    if(state_.state() == gate_state_t::GATE_CLOSING) {
        hw->toggle_gate("STOPPING CURRENT CLOSING");
    }

    Log.traceln("GateSystem # ---- OPEN the gate ----");
    state_.set_state(gate_state_t::GATE_OPENING);
    hw->toggle_gate("OPEN GATE");
    current_mode = GateSystemMode::ENSURE_GATE_OPEN;

    blink_led(50);
}

void GateSystem::close_gate()
{
    if((state_.state() == gate_state_t::GATE_CLOSING && !state_.is_stopped()) || (state_.state() == gate_state_t::GATE_CLOSED)) {
        Log.verboseln("GateSystem  # #### IGNORE GATE CLOSE because gate is in state [%s] ####", state_.current_state_str().c_str());
        return;
    }
    if(state_.state() == gate_state_t::GATE_OPENING) {
        hw->toggle_gate("STOPPING CURRENT OPENING");
    }

    Log.traceln("GateSystem # ---- CLOSE the gate ----");
    hw->toggle_gate("CLOSE GATE");
    current_mode = GateSystemMode::ENSURE_GATE_CLOSED;
    blink_led(500);
}

void GateSystem::enabled_contactor() 
{
    Log.traceln("GateSystem # ==== Contactor ENABLED ====");
    state_.set_contactor(true);
    
    if (state_.state() != gate_state_t::GATE_CLOSED) {
        Log.traceln("GateSystem # ==== Contactor found that gate is closed after closing procedure ====");
        current_mode = GateSystemMode::NORMAL_OPERATION;
        state_.set_state(gate_state_t::GATE_CLOSED);
    }
}

void GateSystem::disabled_contactor()
{
    Log.traceln("GateSystem # ==== Contactor DISABLED ====");
    state_.set_contactor(false);

    if(state_.state() == gate_state_t::GATE_CLOSED) {
        Log.traceln("GateSystem # ==== Gate was closed when contactor changed state to False, reclose gate ====");
        hw->toggle_gate("GATE_WAS_CLOSED_WHEN_CONTRACTOR_CHANGE_TO_FALSE - stop opening");
        state_.set_state(gate_state_t::GATE_CLOSING);
        current_mode = GateSystemMode::ENSURE_GATE_CLOSED;
        blink_led(5000);
    }
}

GateState &GateSystem::state()
{
    return state_;
}