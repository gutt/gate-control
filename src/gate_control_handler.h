#pragma once

#include <WString.h>
#include "gate_state.h"


class GateControlHandler
{
public:

    virtual ~GateControlHandler() {}

    virtual void open_gate() = 0;
    virtual void close_gate() = 0;

    virtual void stop_gate() = 0;

    virtual void enabled_contactor() = 0;
    virtual void disabled_contactor() = 0;

    virtual const GateState &state() = 0;
};