#include <cstdlib>
#include <iostream>

#pragma once 
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/Node.h"

class PlayerStatus
{

public:

    enum State {
        UnknownState,
        Stop, 
        Play,
        Pause
    };

    enum Source {
        API_IMS,
        LTC
    };

    static State parseStateFromString(std::string type);

    PlayerStatus();
    PlayerStatus(PlayerStatus &player);
    ~PlayerStatus();

    void setState(State state) {this->state = state;}
    void setCplId(std::string cplId) {this->cplId = cplId;}
    void setCplTitle(std::string cplTitle) {this->cplTitle = cplTitle;}
    void setspeedRate(uint32_t numerator, uint32_t denominator) {speedRate = (double)numerator/(double)denominator;}
    void setEditPosition(uint32_t editPosition) {this->editPosition = editPosition;}
    void setTimestamp(uint64_t timestamp) {this->timestamp = timestamp;}

    std::string getStateToString();
    State getState() {return state;}
    std::string getCplId() {return cplId;}
    std::string getCplTitle() {return cplTitle;}
    uint64_t getTimestamp() {return timestamp;}
    
    // -- timecode calculation
    double getTimeCodeMs();
    double getSpeedRate() {return speedRate;}

    // -- TODO to XML for API
    std::string toXml();
    std::string toString();

private:

    // -- state
    State state;
    
    // -- cpls
    std::string cplId;
    std::string cplTitle;

    // -- permit to know timecode
    double speedRate;
    uint32_t editPosition;

    Source sourceTime;

    uint64_t timestamp;
};