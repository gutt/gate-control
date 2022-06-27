#pragma once

#include <WString.h>
#include "gate_state.h"


class GateControlHandler
{
public:

    virtual ~GateControlHandler() {}

    virtual void open_gate() = 0;
    virtual void close_gate() = 0;

    virtual void open_gate_with_stop() = 0;
    virtual void close_gate_with_stop() = 0;

    virtual void stop_gate() = 0;

    virtual void enable_contactor() = 0;
    virtual void disable_contactor() = 0;

    virtual const GateState &state() = 0;
};