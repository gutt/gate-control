#include "gate_system.h"
#include "led.h"
#include "ArduinoLog.h"

GateSystem::GateSystem(HardwareLayer *hw_layer) : 
    hw(hw_layer)
{
}

void GateSystem::stop_gate()
{
    if(state_.is_stopped()) {
        return;
    }
    reset_process();

    state_.set_stopped(true);

    hw->toggle_gate("GATE_STOP");

    blink_led(2000);

}

void GateSystem::open_gate()
{
    if((state_.state() == gate_state_t::GATE_OPENING && !state_.is_stopped()) || (state_.state() == gate_state_t::GATE_OPEN)) {
        return;
    }
    if(state_.state() == gate_state_t::GATE_CLOSING) {
        hw->toggle_gate("STOPPING CURRENT CLOSING");
    }
    Log.traceln("GateSystem # ---- OPEN the gate ----");
    reset_process();
    ensure_gate_opened();

    blink_led(50);
}

void GateSystem::close_gate()
{
    if((state_.state() == gate_state_t::GATE_CLOSING && !state_.is_stopped()) || (state_.state() == gate_state_t::GATE_CLOSED)) {
        return;
    }
    if(state_.state() == gate_state_t::GATE_OPENING) {
        hw->toggle_gate("STOPPING CURRENT OPENING");
    }

    Log.traceln("GateSystem # ---- CLOSE the gate ----");
    reset_process();
    ensure_gate_closed();

    blink_led(500);
}

void GateSystem::enable_contactor() 
{
    Log.traceln("GateSystem # ==== Contactor ENABLED ====");
    state_.set_contactor(true);
    
    if (state_.state() == gate_state_t::GATE_CLOSING) {
        Log.traceln("GateSystem # ==== Contactor found that gate is closed after closing procedure ====");
        reset_process();
        state_.set_state(gate_state_t::GATE_CLOSED);
    }
}

void GateSystem::disable_contactor()
{
    Log.traceln("GateSystem # ==== Contactor DISABLED ====");
    state_.set_contactor(false);

    if(state_.state() == gate_state_t::GATE_CLOSED) {
        Log.traceln("GateSystem # ==== Gate was closed when contactor changed state to False, reclose gate ====");
        reset_process();
        ensure_gate_closed();
        blink_led(1000);
    }
}

void GateSystem::ensure_gate_closed()
{
    if (state_.gate_close_check_count >= check_count_limit) {
        state_.set_state(gate_state_t::GATE_UNDEFINED);
        state_.set_stopped(true);
        reset_process();
        return;
    }

    state_.set_state(gate_state_t::GATE_CLOSING);
    state_.gate_close_check_count++;

    hw->toggle_gate("ENSURE_GATE_CLOSED");

    Log.verboseln("GateSystem # ***** Ensure gate as 'closed' check no. %d", state_.gate_close_check_count);
    ensure_gate_closed_timer.once(
        gate_state_check_interval, [this] () {             
            ensure_gate_closed(); 
    });
}

void GateSystem::ensure_gate_opened()
{
    if (state_.gate_open_check_count >= check_count_limit) {
        state_.set_state(gate_state_t::GATE_UNDEFINED);
        state_.set_stopped(true);
        reset_process();
        return;
    }

    if (!state_.is_contactor_enabled() && state_.gate_open_check_count != 0) {
        state_.set_state(gate_state_t::GATE_OPEN); 
        reset_process();
        return;
    }
    state_.set_state(gate_state_t::GATE_OPENING);
    state_.gate_open_check_count++;
    
    hw->toggle_gate("ENSURE_GATE_OPENED");

    Log.verboseln("GateSystem # ***** Ensure gate as 'opened' check no. %d", state_.gate_open_check_count);
    ensure_gate_opened_timer.once(
        gate_state_check_interval, [this] () {
            ensure_gate_opened();
    });
}

void GateSystem::stop_ensure_gate_opened() {
    ensure_gate_opened_timer.detach();    
    if(state_.gate_open_check_count == 0) {
        return;
    }
    state_.gate_open_check_count = 0;
    Log.verboseln("GateSystem # ***** End ensure gate as 'opened'.");
}

void GateSystem::stop_ensure_gate_closed() {
    ensure_gate_closed_timer.detach();    
    if(state_.gate_close_check_count == 0) {
        return;
    }
    state_.gate_close_check_count = 0;
    Log.verboseln("GateSystem # ***** End ensure gate as 'closed'.");
}

void GateSystem::reset_process()
{
    Log.verboseln("GateSystem # Reset all ensure checks.");

    stop_ensure_gate_opened();
    stop_ensure_gate_closed();

    reset_led();
}

GateState &GateSystem::state()
{
    return state_;
}