#include "gate_system.h"
#include "led.h"
#include "ArduinoLog.h"

GateSystem::GateSystem(HardwareLayer *hw_layer) :
    hw(hw_layer)
{
}

void GateSystem::set_state_undefined() {
    Log.verboseln("GateSystem # Set gate state to undefined.");

}

void GateSystem::setup()
{
    current_mode = GateSystemMode::ENSURE_GATE_CLOSED;
    gate.set_open();
    gate.set_stopped(true);
    reset_led();
}

void GateSystem::loop()
{
    // in normal mode
    if(current_mode == GateSystemMode::IDLE) {
        return;
    }

    // if mode changed to different mode than NORMAL
    if(current_mode != last_mode) {
        gate_open_check_count = 0;
        gate_close_check_count = 0;

        last_check_millis = millis();
        last_mode = current_mode;
    }

    if ((millis() - last_check_millis < 1000 * gate_state_check_interval)) {
        return;
    }

    last_check_millis = millis();

    if(current_mode == GateSystemMode::ENSURE_GATE_OPEN) {
        // if contactron is not enabled we are 100% sure that is gate is opened - switch back to idle
        if(!gate.is_contactron_enabled()) {
            current_mode = GateSystemMode::IDLE;
            gate.set_open();
            gate.set_stopped(true);
            gate_open_handler();
            return;
        }
        if (gate_open_check_count >= check_count_limit) {
            set_state_undefined();
            return;
        }
        hw->toggle_gate(F("ENSURE_GATE_OPEN"));
        gate_open_check_count++;
        return;
    }

    if(current_mode == GateSystemMode::ENSURE_GATE_CLOSED) {
        // if contactron is now enabled - switch back to normal operation
        if(gate.is_contactron_enabled()) {
            current_mode = GateSystemMode::IDLE;
            return;
        }
        if (gate_close_check_count >= check_count_limit) {
            set_state_undefined();
            return;
        }
        hw->toggle_gate(F("ENSURE_GATE_CLOSE"));
        gate_close_check_count++;
        return;
    }
}
void GateSystem::stop_gate()
{
    if(gate.is_stopped()) {
        return;
    }
    gate_stopped_handler();
    gate.set_stopped(true);
    hw->toggle_gate(F("GATE_STOP"));

    blink_led(2000);

}

void GateSystem::open_gate()
{
    if((gate.is_opening() && !gate.is_stopped()) || (gate.is_open())) {
        Log.verboseln("GateSystem  # #### IGNORE GATE OPEN because gate is in state [%s] ####", gate.current_state_str().c_str());
        return;
    }
    // Gate is opening
    gate_opening_handler();

    // If gate was previously stopped, un-stop it:
    if (gate.is_stopped()) {
        gate.set_stopped(false);
    }

    // If gate is closing, we need to toggle it once to stop that motion
    if(gate.is_closing()) {
        hw->toggle_gate(F("STOPPING CURRENT CLOSING"));
    }

    Log.traceln("GateSystem # ---- OPENING the gate ----");
    gate.set_opening();
    hw->toggle_gate(F("OPENING GATE"));
    current_mode = GateSystemMode::ENSURE_GATE_OPEN;

    blink_led(50);
}

void GateSystem::close_gate()
{
    if((gate.is_closing() && !gate.is_stopped()) || (gate.is_closed())) {
        Log.verboseln("GateSystem  # #### IGNORE GATE CLOSE because gate is in state [%s] ####", gate.current_state_str().c_str());
        return;
    }
    // CLOSING
    gate_closing_handler();

    // If gate was previously stopped, un-stop it:
    if (gate.is_stopped()) {
        gate.set_stopped(false);
    }

    // If gate is opening, we need to toggle it once to stop that motion
    if(gate.is_opening()) {
        hw->toggle_gate(F("STOPPING CURRENT OPENING"));
    }
    Log.traceln("GateSystem # ---- CLOSING the gate ----");
    gate.set_closing();
    hw->toggle_gate(F("CLOSING GATE"));
    current_mode = GateSystemMode::ENSURE_GATE_CLOSED;
    blink_led(500);
}

void GateSystem::enabled_contactron()
{
    Log.traceln("GateSystem # ==== Contactron CLOSED ====");
    gate.set_contactron(true);

    if(gate.is_undefined())
    {
        Log.verboseln("GateSystem  # #### IGNORE DISABLED CONTACTRON because gate is in state [%s] ####", gate.current_state_str().c_str());
        current_mode = GateSystemMode::IDLE;
        gate.set_closed();
        gate.set_stopped(true);
        gate_close_handler();
        return;
    }


    if (!gate.is_closed()) {
        // gate is closed
        Log.traceln("GateSystem # ==== contactron found that gate is closed after closing procedure ====");
        current_mode = GateSystemMode::IDLE;
        gate.set_closed();
        gate.set_stopped(true);
        gate_close_handler();
    }
}

void GateSystem::disabled_contactron()
{
    Log.traceln("GateSystem # ==== Contactron OPENED ====");
    gate.set_contactron(false);

    if(gate.is_undefined())
    {
        Log.verboseln("GateSystem  # #### IGNORE DISABLED CONTACTRON because gate is in state [%s] ####", gate.current_state_str().c_str());
        current_mode = GateSystemMode::IDLE;
        gate.set_open();
        gate.set_stopped(false);
        gate_open_handler();
        return;
    }

    // should reclose?
    if(gate.is_closed()) {
        Log.traceln("GateSystem # ==== Gate was closed when contactron changed state to False, reclose gate ====");
        hw->toggle_gate(F("GATE_WAS_CLOSED_WHEN_CONTRACTOR_CHANGE_TO_FALSE - stop opening"));
        gate.set_closing();
        gate.set_stopped(false);
        gate_closing_handler();
        current_mode = GateSystemMode::ENSURE_GATE_CLOSED;
        blink_led(5000);
    }

}

void GateSystem::set_gate_open_handler(gate_event_handler_t handler)
{
    gate_open_handler = handler;
}

void GateSystem::set_gate_opening_handler(gate_event_handler_t handler)
{
    gate_opening_handler = handler;
}

void GateSystem::set_gate_close_handler(gate_event_handler_t handler)
{
    gate_close_handler = handler;
}

void GateSystem::set_gate_closing_handler(gate_event_handler_t handler)
{
    gate_closing_handler = handler;
}

void GateSystem::set_gate_stopped_handler(gate_event_handler_t handler)
{
    gate_stopped_handler = handler;
}
