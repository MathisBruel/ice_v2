#pragma once
#include <hfsm2/machine.hpp>
#include <string>

class BoundaryStateMachine;

class ContentStateBase {
public:
    using Context = BoundaryStateMachine;
    virtual ~ContentStateBase() = default;
    virtual bool entryGuard(Context& context) = 0;
    virtual void entry(Context& context) = 0;
    virtual void next(Context& context) = 0;
    virtual std::string name() const = 0;
}; 