#include "gate_system.h"
#include "led.h"
#include "ArduinoLog.h"

GateSystem::GateSystem(HardwareLayer *hw_layer) : 
    hw(hw_layer)
{

}

void GateSystem::stop_gate()
{
    if(state_.is_stopped() == true 
    || state_.state() == gate_state_t::GATE_OPEN 
    || state_.state() == gate_state_t::GATE_CLOSED) {
        return;
    }

    Log.noticeln("GateSystem # ---- STOP the gate ----");

    state_.set_stopped(true);
    reset_process();

    hw->toggle_gate("GATE_STOP");

    blink_led(2000);
}

void GateSystem::open_gate()
{
    if(state_.state() == gate_state_t::GATE_OPENING && state_.is_stopped()) {
        state_.set_stopped(false);
        Log.traceln("GateSystem # ---- gate was stopped -> disable stop flag, moving gate to open state----");
        hw->toggle_gate("GATE OPENING, first click from stopped");

    } else {
        if(state_.state() == gate_state_t::GATE_OPEN 
        || state_.state() == gate_state_t::GATE_OPENING) {
            return;
        }
    }

    Log.traceln("GateSystem # ---- OPEN the gate ----");
    reset_process();

    // when gate is switching directions then click one more time
    if(state_.state() == gate_state_t::GATE_CLOSING && !state_.is_stopped()) {
        Log.traceln("GateSystem # ---- gate is Closing now, switching directions ----");
        hw->toggle_gate("STOP CLOSING, then opening");
    }

    state_.set_stopped(false);
    ensure_gate_opened();

    blink_led(50);
}

void GateSystem::close_gate()
{
    if(state_.state() == gate_state_t::GATE_CLOSING && state_.is_stopped()) {
        state_.set_stopped(false);
        Log.traceln("GateSystem # ---- gate was stopped -> disable stop flag, moving gate to close state----");
        hw->toggle_gate("GATE CLOSING, first click from stopped");

    } else {
        // ignore all close requests if gate is closed or arleady closing
        if(state_.state() == gate_state_t::GATE_CLOSED 
        || state_.state() == gate_state_t::GATE_CLOSING) {
            return;
        }
    }
    Log.traceln("GateSystem # ---- CLOSE the gate ----");
    reset_process();

    // when gate is switching directions then click one more time
    if(state_.state() == gate_state_t::GATE_OPENING && !state_.is_stopped()) {
        Log.traceln("GateSystem # ---- gate is Opening now, switching directions ----");
        hw->toggle_gate("STOP OPENING, then closing");
    }
    state_.set_stopped(false);
    ensure_gate_closed();

    blink_led(200);
}

void GateSystem::open_gate_with_stop()
{
    Log.traceln("GateSystem # ---- OPEN the gate with STOP ----");
    
    if(state_.state() == gate_state_t::GATE_OPENING && !state_.is_stopped()) {
      stop_gate();
      return;
    }
    open_gate();
}

void GateSystem::close_gate_with_stop()
{
    Log.traceln("GateSystem # ---- CLOSE the gate with STOP ----");
    if(state_.state() == gate_state_t::GATE_CLOSING && !state_.is_stopped()) {
      stop_gate();
      return;
    }
    close_gate();
}

void GateSystem::enable_contactor() 
{
    Log.traceln("GateSystem # ==== Contactor ENABLED ====");
    state_.set_contactor(true);
    
    if (state_.state() == gate_state_t::GATE_CLOSING) {
        Log.traceln("GateSystem # ==== Contactor found that gate is closed after closing procedure ====");
        state_.set_state(gate_state_t::GATE_CLOSED);
        reset_process();
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

        blink_led(200);
    }
}

void GateSystem::ensure_gate_closed()
{
    if (state_.gate_close_check_count >= check_count_limit) {
        state_.set_state(gate_state_t::GATE_UNDEFINED);
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