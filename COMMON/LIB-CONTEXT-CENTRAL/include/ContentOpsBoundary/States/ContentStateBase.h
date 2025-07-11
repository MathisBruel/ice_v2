#pragma once
#include <machine.hpp>
#include <string>
#include <iostream>
#include <map>
#include "ContentOpsApp/TransitionResponse.h"

struct UploadCISEvent {
    std::string releaseCisPath;
    std::map<std::string, std::string> params;
    
    UploadCISEvent() = default;
    UploadCISEvent(const std::string& path, const std::map<std::string, std::string>& parameters = {}) 
        : releaseCisPath(path), params(parameters) {}
};

struct UploadSyncEvent {
    std::map<std::string, std::string> params;
    
    UploadSyncEvent() = default;
    UploadSyncEvent(const std::map<std::string, std::string>& parameters) : params(parameters) {}
};

struct CreateReleaseEvent {
    std::map<std::string, std::string> params;
    
    CreateReleaseEvent() = default;
    CreateReleaseEvent(const std::map<std::string, std::string>& parameters) : params(parameters) {}
};

struct ArchiveCompleteEvent {
    std::map<std::string, std::string> params;
    
    ArchiveCompleteEvent() = default;
    ArchiveCompleteEvent(const std::map<std::string, std::string>& parameters) : params(parameters) {}
};

struct CloseReleaseEvent {
    std::map<std::string, std::string> params;
    
    CloseReleaseEvent() = default;
    CloseReleaseEvent(const std::map<std::string, std::string>& parameters) : params(parameters) {}
};

struct NewCPLEvent {
    std::map<std::string, std::string> params;
    
    NewCPLEvent() = default;
    NewCPLEvent(const std::map<std::string, std::string>& parameters) : params(parameters) {}
};

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

    template <typename TEvent, typename Control>
    void preReact(const TEvent&, Control& control) {}
    template <typename TEvent, typename Control>
    static void widePreReact(const TEvent&, Control& control) {}
    template <typename Control>
    static void widePreUpdate(Control& control) {}
    template <typename Control>
    static void preUpdate(Control& control) {}
    template <typename Control>
    static void widePostUpdate(Control& control) {}
    template <typename Control>
    static void postUpdate(Control& control) {}

    template <typename TEvent, typename Control>
    static void wideReact(const TEvent&, Control& control) {}

    template <typename Control>
    void react(const UploadCISEvent&, Control& control) {}
    template <typename Control>
    void react(const UploadSyncEvent&, Control& control) {}
    template <typename Control>
    void react(const CreateReleaseEvent&, Control& control) {}
    template <typename Control>
    void react(const ArchiveCompleteEvent&, Control& control) {}
    template <typename Control>
    void react(const CloseReleaseEvent&, Control& control) {}
    template <typename Control>
    void react(const NewCPLEvent&, Control& control) {}
    
    template <typename TEvent, typename Control>
    void postReact(const TEvent&, Control& control) {}
    template <typename TEvent, typename Control>
    static void widePostReact(const TEvent&, Control& control) {}
    
    template <typename Control>
    static void wideExitGuard(Control& control) {}
    template <typename Control>
    static void exitGuard(Control& control) {}
    template <typename Control>
    static void reenter(Control& control) {}
    template <typename Control>
    static void wideReenter(Control& control) {}

public:
    virtual ~ContentStateBase() = default;
    virtual void EntryGuard() {}
    virtual void enter() {}
    virtual void update() {}
    virtual void exit() {}
}; 