#pragma once

#ifndef button_h
#define Button_h

#include <functional>
#include "Arduino.h"

#define UNDEFINED_PIN 255
#define DEFAULT_DEBOUNCE_TIME 50

class Button;
using button_callback_t = std::function<void()>;

class Button {
public:
    Button();
    Button(uint8_t pin, unsigned int debounce_duration_ms = 50, uint8_t mode = INPUT_PULLUP, bool active_low = true);
    void set_debounce_duration(unsigned int ms);

    void set_pressed_callback(button_callback_t f);
    void set_released_callback(button_callback_t f);

    void loop();

private: 
    uint8_t read_state();

    button_callback_t pressed_callback = NULL;
    button_callback_t released_callback = NULL;

    uint8_t pin_;
    uint8_t previous_state_;

    unsigned long down_time_ms_;
    unsigned int debounce_duration_ms_ = 50;

    uint8_t pressed_state_;

    bool pressed_active_;
    bool last_change_pressed_ = false;
};
#endif
