#pragma once

#include <Arduino.h>

class CustomLog : public Print 
{
    size_t write(uint8_t character);
};