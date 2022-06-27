#pragma once

#include <Arduino.h>
#include <Ticker.h>

int led_state = false;

Ticker blink_led_timer;

inline void reset_led()
{
    blink_led_timer.detach();
    digitalWrite(LED_BUILTIN, 1);
}

inline void toggle_led()
{
    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
}

inline void blink_led(int ms) {
    reset_led();
    blink_led_timer.attach_ms(ms, toggle_led);
}
