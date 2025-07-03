#pragma once
#include <machine.hpp>
#include <string>
#include <iostream>

struct ContentStateBase {
    template <typename Control>
    void entryGuard(Control&) {}
    template <typename Control>
    void enter(Control&) {}
    template <typename Control>
    void update(Control&) {}
    template <typename Control>
    void exit(Control&) {}

    // Helpers statiques pour HFSM2 obligatoires
    template <typename Control>
    static void wideEntryGuard(Control& control) {}
    template <typename Control>
    static void wideEnter(Control& control) {}
    template <typename Control>
    static void wideUpdate(Control& control) {}
    template <typename Control>
    static void wideExit(Control& control) {}

public:
    virtual ~ContentStateBase() = default;
    virtual void EntryGuard() {}
    virtual void enter() {}
    virtual void update() {}
    virtual void exit() {}
}; 