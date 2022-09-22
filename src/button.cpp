#include "button.h"
#include "ArduinoLog.h"

#include <Arduino.h>

Button::Button() :
  pin_(UNDEFINED_PIN)
{ 

}

Button::Button(uint8_t pin, unsigned int debounce_duration_ms, uint8_t mode, bool active_low) :
    pin_(pin),
    debounce_duration_ms_(debounce_duration_ms),
    pressed_state_(active_low ? LOW : HIGH),
    pressed_active_(false)
{
    pinMode(pin, mode);
    previous_state_ = read_state();
    down_time_ms_ = millis();
}

void Button::set_debounce_duration(unsigned int ms) 
{
    debounce_duration_ms_ = ms;
}

void Button::set_pressed_callback(button_callback_t c)
{
    pressed_callback = c; 
}

void Button::set_released_callback(button_callback_t c)
{
    released_callback = c; 
}

byte Button::read_state() 
{
    return digitalRead(pin_);
}

void Button::loop() 
{
    if (pin_ == UNDEFINED_PIN) {
      return;
    }
    uint8_t state = read_state();

    unsigned long now = millis();

    // value has just been changed 
    if (previous_state_ != state) {
        down_time_ms_ = now;
        if (state == pressed_state_) {
            Log.noticeln("# MOMENTARY PRESSED PIN %d, debounce_duration_ms: %d", pin_, debounce_duration_ms_);
            last_change_pressed_ = true;
        } else {
            Log.noticeln("# MOMENTARY RELEASED PIN %d, debounce_duration_ms_: %d", pin_, debounce_duration_ms_);
            last_change_pressed_ = false;
        }
    }
    bool is_stable = (now - down_time_ms_) >= debounce_duration_ms_;

    if (pressed_active_ == false && last_change_pressed_ && is_stable) {
        Log.noticeln("# BUTTON TRIGGER PIN %d, debounce_time_ms: %d last_change_ms: %d", pin_, debounce_duration_ms_, now - down_time_ms_);
        if (pressed_callback != NULL) {
            pressed_callback ();
        }
        pressed_active_ = true;
    } else if (pressed_active_ == true && !last_change_pressed_ && is_stable) {
        Log.noticeln("# BUTTON RELEASE TRIGGER PIN %d, debounce_time_ms: %d last_change_ms: %d", pin_, debounce_duration_ms_, now - down_time_ms_);
        if (released_callback != NULL) {
            released_callback ();
        }
        pressed_active_ = false;
    }
    previous_state_ = state;
}